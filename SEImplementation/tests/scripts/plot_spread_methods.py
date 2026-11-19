#!/usr/bin/env python3
"""Compare spread and joint-fit scores on a paired sim12 sample under two astrometric conditions."""
import argparse
import json
from pathlib import Path

import astropy.units as u
from astropy.coordinates import SkyCoord
from astropy.io import fits
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from matplotlib.lines import Line2D
import numpy as np

from compare_joint_spread import operating_points
from plot_joint_spread import threshold_curve


METHODS = (
    ('single_spread', 'Single-frame spread · g_01', '#87929d', ':'),
    ('combined_spread', 'Combined spread', '#8051a4', '-'),
    ('combined_significance', 'Combined spread / error', '#af8b3e', '-.'),
    ('shared_radius', 'Shared center · radius', '#237fbd', '-'),
    ('offset_radius', 'Frame offsets · radius', '#00896e', '-'),
    ('shared_delta', 'Shared center · Δχ²', '#c26375', '--'),
    ('offset_delta', 'Frame offsets · Δχ²', '#84994f', '--'),
    ('offset_objective', 'Frame offsets · ΔJ', '#d87c24', '--'),
)
PREFIX = 'joint_spread_model_alt_'


def read_catalog(path):
    """Align catalogs by unchanged detection coordinates, independent of output ordering."""
    data = fits.getdata(path, 1)
    return data[np.lexsort((data['world_centroid_delta'], data['world_centroid_alpha']))]


def get_scores(shared, offsets):
    """Per-frame and combined spread are independent of the joint prior setting."""
    np.testing.assert_allclose(shared['combined_spread_model'], offsets['combined_spread_model'], equal_nan=True)
    np.testing.assert_allclose(shared['combined_spread_model'], shared['combined_spread_model_alt'], equal_nan=True)
    with np.errstate(divide='ignore', invalid='ignore'):
        significance = shared['combined_spread_model'] / shared['combined_spread_model_error']
    scores = {
        'single_spread': shared['spread_model_0'],
        'combined_spread': shared['combined_spread_model'],
        'combined_significance': significance,
        'shared_radius': shared[PREFIX + 'radius'],
        'offset_radius': offsets[PREFIX + 'radius'],
        'shared_delta': shared[PREFIX + 'delta_chi_squared'],
        'offset_delta': offsets[PREFIX + 'delta_chi_squared'],
        'offset_objective': offsets[PREFIX + 'delta_objective'],
    }
    valid = (shared['combined_spread_model_error'] > 0)
    valid &= ((shared[PREFIX + 'flags'] & ~4) == 0) & ((offsets[PREFIX + 'flags'] & ~4) == 0)
    for values in scores.values():
        valid &= np.isfinite(values)
    return scores, valid


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('directory', type=Path, help='Directory containing clean/shifted shared/offset catalogs')
    parser.add_argument('--truth', type=Path, default=Path('tests/sim12/sim12_g.list'))
    parser.add_argument('--prefix', type=Path)
    args = parser.parse_args()
    output = args.prefix or args.directory / 'comparison'
    catalogs = {name: read_catalog(args.directory / name / 'catalog.fits')
                for name in ('clean_shared', 'clean_offsets', 'shifted_shared', 'shifted_offsets')}
    reference = catalogs['clean_shared']
    for data in catalogs.values():
        for coordinate in ('world_centroid_alpha', 'world_centroid_delta'):
            np.testing.assert_allclose(data[coordinate], reference[coordinate], rtol=0, atol=1e-12)
    truth = np.loadtxt(args.truth, usecols=range(4), ndmin=2)
    observed = SkyCoord(reference['world_centroid_alpha'] * u.deg, reference['world_centroid_delta'] * u.deg)
    positions = SkyCoord(truth[:, 1] * u.deg, truth[:, 2] * u.deg)
    index, distance, _ = observed.match_to_catalog_sky(positions)
    _, second, _ = observed.match_to_catalog_sky(positions, nthneighbor=2)
    classes = truth[index, 0].astype(int)
    matched = (distance.arcsec <= 0.5) & (second.arcsec > 0.5) & np.isin(classes, (100, 200))
    common = matched.copy()
    conditions = {}
    for name in ('clean', 'shifted'):
        scores, valid = get_scores(catalogs[name + '_shared'], catalogs[name + '_offsets'])
        conditions[name] = scores
        common &= valid
    classes = classes[common]
    stars, galaxies = int(np.count_nonzero(classes == 100)), int(np.count_nonzero(classes == 200))
    if not stars or not galaxies:
        parser.error('The common valid sample must contain both stars and galaxies.')
    report = {'rows': len(reference), 'truth_matched_rows': int(matched.sum()), 'common_sources': len(classes),
              'stars': stars, 'galaxies': galaxies, 'iteration_limit': 1000,
              'offset_prior_sigma_reference_pixels': 0.2, 'injected_sigma_reference_pixels': 0.3,
              'selection': 'Same sources in both conditions and all methods; radius boundaries retained; '
                           'failed/iteration-limited joint fits excluded.',
              'limitation': 'Thresholds tuned on this sample; no independent validation.', 'conditions': {},
              'joint_flag_counts': {name: {str(int(flag)): int(np.count_nonzero(data[PREFIX + 'flags'] == flag))
                                           for flag in np.unique(data[PREFIX + 'flags'])}
                                    for name, data in catalogs.items()}}
    plt.rcParams.update({'font.size': 11, 'axes.spines.top': False, 'axes.spines.right': False,
                         'axes.titleweight': 'bold', 'savefig.facecolor': 'white'})
    fig, axes = plt.subplots(2, 2, figsize=(15.5, 12.0), gridspec_kw={'width_ratios': [1.05, 1]})
    fig.subplots_adjust(left=0.075, right=0.97, top=0.80, bottom=0.20, wspace=0.57, hspace=0.48)
    fig.suptitle('SpreadModel methods: classification and sensitivity to astrometric errors',
                 x=0.075, y=0.975, ha='left', fontsize=18, weight='bold')
    fig.text(0.075, 0.935,
             f'Exactly the same {len(classes)} sources in every panel: {stars} stars + {galaxies} galaxies · '
             '9 g/r/i frames · 1,000 iterations', color='#505b66')
    handles = [Line2D([0], [0], color=color, linestyle=style, linewidth=2, label=label)
               for _, label, color, style in METHODS]
    fig.legend(handles=handles, ncol=4, loc='upper left', bbox_to_anchor=(0.07, 0.908),
               frameon=False, fontsize=10, handlelength=2.5, columnspacing=1.9, labelspacing=0.9)
    for row, (condition, title) in enumerate((('clean', 'Original astrometry'),
                                             ('shifted', 'Injected WCS errors · σ = 0.3 reference pixels'))):
        roc, matrix = axes[row]
        summary = {}
        values = []
        annotations = []
        for name, label, color, style in METHODS:
            score = np.asarray(conditions[condition][name][common], dtype=float)
            recall, false_positive, _ = threshold_curve(score, classes)
            auc = float(np.trapezoid(np.r_[0, recall], np.r_[0, false_positive]))
            galaxies_scores = score[classes == 200][:, None]
            stars_scores = score[classes == 100][None, :]
            rank_auc = np.mean(galaxies_scores > stars_scores) + 0.5 * np.mean(galaxies_scores == stars_scores)
            np.testing.assert_allclose(auc, rank_auc, rtol=0, atol=1e-12)
            line_width = 2.4 if name in ('shared_radius', 'offset_radius') else 1.5
            roc.plot(np.r_[0, false_positive] * 100, np.r_[0, recall] * 100,
                     color=color, linestyle=style, linewidth=line_width)
            metrics = operating_points(score, classes)
            metrics['roc_auc'] = auc
            summary[name] = metrics
            at_five, at_ten = metrics['operating_points'][1:]
            values.append([auc, at_five['galaxy_completeness'], at_ten['galaxy_completeness']])
            annotations.append([f'{auc:.3f}'] + [f"{point['galaxy_completeness']:.0%}"
                                if point['threshold'] is not None else '—' for point in (at_five, at_ten)])
        report['conditions'][condition] = summary
        roc.plot([0, 100], [0, 100], ':', linewidth=1, color='#c7cdd2', zorder=0)
        roc.set(title=title, xlabel='Stars classified as galaxies [%]', ylabel='Galaxies recovered [%]',
                xlim=(0, 100), ylim=(0, 102))
        roc.title.set_fontsize(12)
        roc.grid(alpha=0.16)
        matrix.imshow(values, vmin=0, vmax=1, cmap='Blues', aspect='auto', interpolation='nearest')
        matrix.set_yticks(np.arange(len(METHODS)), labels=[entry[1] for entry in METHODS], fontsize=10)
        matrix.set_xticks([0, 1, 2], labels=['ROC AUC', 'Recovery at\n≤5% contamination',
                                          'Recovery at\n≤10% contamination'], fontsize=9)
        matrix.tick_params(length=0, pad=8)
        matrix.set_title('Higher is better · same source population', fontsize=12, pad=14)
        for i, cells in enumerate(annotations):
            for j, text in enumerate(cells):
                matrix.text(j, i, text, ha='center', va='center', fontsize=11,
                            color='white' if values[i][j] >= 0.65 else '#253341', weight='bold')
        for spine in matrix.spines.values():
            spine.set_visible(False)
    fig.text(0.075, 0.035,
             'All panels use identical sources. Thresholds are tuned on this sample; no independent validation.\n'
             'Contamination = stars among sources selected as galaxies. '
             '— means no nonempty selection meets the limit.\n'
             'ΔJ includes the Gaussian offset prior; Δχ² uses pixels only. '
             'Offset prior: σ = 0.2 reference pixels per axis.\n'
             'Alt’s unchanged per-frame / combined spread outputs coincide with the original and are not duplicated.',
             fontsize=9, color='#505b66', linespacing=1.7)
    output.parent.mkdir(parents=True, exist_ok=True)
    fig.savefig(str(output) + '.png', dpi=180)
    fig.savefig(str(output) + '.svg')
    plt.close(fig)
    Path(str(output) + '.json').write_text(json.dumps(report, indent=2, allow_nan=False) + '\n')
    print(json.dumps({'common_sources': len(classes), 'stars': stars, 'galaxies': galaxies,
                      'conditions': {condition: {name: {'auc': metrics['roc_auc'],
                          'recovery_5pct': metrics['operating_points'][1]['galaxy_completeness'],
                          'recovery_10pct': metrics['operating_points'][2]['galaxy_completeness']}
                          for name, metrics in methods.items()}
                          for condition, methods in report['conditions'].items()}},
                     indent=2))


if __name__ == '__main__':
    main()
