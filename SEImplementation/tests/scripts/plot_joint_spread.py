#!/usr/bin/env python3
"""Plot paired sim12 scores, ROC curves and completeness at contamination limits."""
import argparse
import json
from pathlib import Path

import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import numpy as np

from compare_joint_spread import load_sample


def threshold_curve(scores, classes):
    """Evaluate all score thresholds, preserving ties, with galaxies positive."""
    order = np.argsort(-scores, kind='stable')
    values = scores[order]
    ends = np.r_[values[:-1] != values[1:], True]
    galaxies = np.cumsum(classes[order] == 200)[ends]
    stars = np.cumsum(classes[order] == 100)[ends]
    recall = galaxies / np.count_nonzero(classes == 200)
    false_positive = stars / np.count_nonzero(classes == 100)
    contamination = stars / (galaxies + stars)
    return recall, false_positive, contamination


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('catalog', type=Path)
    parser.add_argument('--truth', type=Path, default=Path('sim12_g.list'))
    parser.add_argument('--prefix', type=Path, default=Path('joint_spread_comparison'))
    args = parser.parse_args()
    sample = load_sample(args.catalog, args.truth)
    valid = sample['valid']
    classes = sample['classes'][valid]
    n_galaxies = int(np.count_nonzero(classes == 200))
    n_stars = int(np.count_nonzero(classes == 100))
    if not n_galaxies or not n_stars:
        parser.error('The common valid sample must contain both stars and galaxies.')
    scores = {name: values[valid] for name, values in sample['scores'].items()}
    snr = sample['snr'][valid]
    plt.rcParams.update({'font.size': 11, 'axes.spines.top': False, 'axes.spines.right': False,
                         'axes.titleweight': 'bold', 'savefig.facecolor': 'white'})
    fig, axes = plt.subplots(2, 2, figsize=(13.4, 10.6), gridspec_kw={'hspace': 0.43, 'wspace': 0.25})
    fig.subplots_adjust(left=0.08, right=0.97, top=0.86, bottom=0.18)
    fig.suptitle('The joint radius separates stars and galaxies better in this sim12 sample',
                 x=0.08, y=0.97, ha='left', fontsize=17, weight='bold')
    fig.text(0.08, 0.925, f'Same {len(classes)} valid, truth-matched sources: {n_stars} stars + '
             f'{n_galaxies} galaxies · 9 configured g/r/i exposures', fontsize=11, color='#515c69')
    fig.text(0.08, 0.895, 'Δχ² improves the overall ranking, but fails high-purity cuts. '
             'Thresholds are selected on this sample.',
             fontsize=10, color='#515c69')
    scatter_axes = axes[0]
    for axis, name, title, xlabel in zip(
            scatter_axes, ('weighted_spread', 'joint_radius'),
            ('Original: inverse-variance combined spread', 'Alt: jointly fitted intrinsic radius'),
            ('Combined SPREAD_MODEL', 'Exponential half-light radius [reference pixels]')):
        for code, label, color in ((100, 'Stars', '#2879b0'), (200, 'Galaxies', '#d76832')):
            selected = (classes == code) & np.isfinite(snr) & (snr > 0)
            axis.scatter(scores[name][selected], snr[selected], s=27, c=color, label=label,
                         alpha=0.8, edgecolors='white', linewidths=0.3)
        axis.set_yscale('log')
        axis.set_xlabel(xlabel)
        axis.set_title(title, loc='left', fontsize=12)
        axis.grid(axis='y', alpha=0.15)
    scatter_axes[0].set_xscale('symlog', linthresh=0.002)
    scatter_axes[1].set_xscale('symlog', linthresh=0.1)
    scatter_axes[1].set_xlim(left=-0.02)
    scatter_axes[0].set_ylabel('Detection SNR (isophotal flux / error)')
    ylim = (max(0.1, np.nanmin(snr[snr > 0]) / 1.5), np.nanmax(snr) * 1.5)
    for axis in scatter_axes:
        axis.set_ylim(ylim)
    scatter_axes[0].axvline(0, color='#67717b', linewidth=0.8, linestyle='--')
    scatter_axes[0].legend(frameon=False, fontsize=10, loc='upper left')
    scatter_axes[1].text(0.02, 0.03, 'Zero radius = point-source limit', transform=scatter_axes[1].transAxes,
                         fontsize=9, color='#515c69')
    roc, purity = axes[1]
    metrics = {}
    budgets = np.linspace(0, 0.20, 401)
    for name, label, color, style in (
            ('weighted_spread', 'Original spread', '#8254a5', '-'),
            ('joint_radius', 'Joint radius', '#07856f', '-'),
            ('joint_delta_chi_squared', 'Joint Δχ²', '#828a96', '--')):
        recall, false_positive, contamination = threshold_curve(scores[name], classes)
        auc = float(np.trapezoid(np.r_[0, recall], np.r_[0, false_positive]))
        roc.plot(np.r_[0, false_positive] * 100, np.r_[0, recall] * 100, color=color, linestyle=style,
                 linewidth=2.1, label=f'{label} · AUC {auc:.3f}')
        best = np.array([np.max(recall[contamination <= budget], initial=0) for budget in budgets])
        purity.step(budgets * 100, best * 100, where='post', color=color, linestyle=style,
                    linewidth=2.1, label=label)
        metrics[name] = {'roc_auc': auc}
        for budget in (0.01, 0.05, 0.10):
            key = f'completeness_at_{int(100 * budget)}pct_contamination'
            metrics[name][key] = float(np.max(recall[contamination <= budget], initial=0))
        if name != 'joint_delta_chi_squared':
            at_five = metrics[name]['completeness_at_5pct_contamination']
            purity.scatter([5], [100 * at_five], s=40, color=color, zorder=5)
            purity.annotate(f'{100 * at_five:.0f}%', (5, 100 * at_five), xytext=(7, 7),
                            textcoords='offset points', color=color, weight='bold', fontsize=10)
    roc.plot([0, 100], [0, 100], ':', color='#c5cad0', linewidth=1, zorder=0)
    roc.set(title='Ranking across all score thresholds', xlabel='Stars classified as galaxies [%]',
            ylabel='Galaxies recovered [%]', xlim=(0, 100), ylim=(0, 102))
    roc.legend(frameon=False, fontsize=9, loc='lower right')
    purity.axvline(5, color='#b5bbc4', linestyle=':', linewidth=1)
    purity.set(title='Recovery at a chosen contamination limit', xlabel='Allowed stars in selected galaxy sample [%]',
               ylabel='Maximum galaxies recovered [%]', xlim=(0, 20), ylim=(-3, 102))
    purity.legend(frameon=False, fontsize=9, loc='upper left')
    for axis in (roc, purity):
        axis.grid(alpha=0.15)
        axis.title.set_fontsize(12)
    fig.text(0.08, 0.045,
             'Common sample excludes failed / iteration-limited fits; radius-boundary solutions are retained.\n'
             '0% recovery means no nonempty threshold selection meets the limit. No independent validation yet.',
             fontsize=9, color='#515c69', linespacing=1.6)
    args.prefix.parent.mkdir(parents=True, exist_ok=True)
    fig.savefig(str(args.prefix) + '.png', dpi=180)
    fig.savefig(str(args.prefix) + '.svg')
    plt.close(fig)
    result = {'paired_sources': len(classes), 'stars': n_stars, 'galaxies': n_galaxies, 'scores': metrics}
    Path(str(args.prefix) + '.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps(result, indent=2))


if __name__ == '__main__':
    main()
