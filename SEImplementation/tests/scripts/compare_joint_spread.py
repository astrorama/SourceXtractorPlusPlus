#!/usr/bin/env python3
"""Compare joint morphology and weighted spread on sim12 truth-matched sources.

Run make install before producing the input catalog. Thresholds are measured on
this same sample: the reported completeness is exploratory, not held-out accuracy.
"""
import argparse
import json
from pathlib import Path

import astropy.units as u
from astropy.coordinates import SkyCoord
from astropy.io import fits
import numpy as np


def operating_points(scores, classes):
    """Find maximum galaxy completeness at each empirical stellar contamination."""
    n_galaxies = int(np.count_nonzero(classes == 200))
    n_stars = int(np.count_nonzero(classes == 100))
    if not n_galaxies or not n_stars:
        return {'galaxies': n_galaxies, 'stars': n_stars, 'operating_points': []}
    order = np.argsort(-scores, kind='stable')
    values = scores[order]
    galaxies = np.cumsum(classes[order] == 200)
    stars = np.cumsum(classes[order] == 100)
    # Evaluate only complete ties, since a scalar threshold cannot split them.
    ends = np.r_[values[:-1] != values[1:], True]
    values, galaxies, stars = values[ends], galaxies[ends], stars[ends]
    contamination = stars / (galaxies + stars)
    points = []
    for target in (0.01, 0.05, 0.10):
        eligible = np.flatnonzero(contamination <= target)
        point = {'target_stellar_contamination': target, 'galaxy_completeness': 0.0,
                 'stellar_contamination': None, 'threshold': None, 'selected_galaxies': 0, 'selected_stars': 0}
        if eligible.size:
            best = eligible[np.argmax(galaxies[eligible])]
            point.update(galaxy_completeness=float(galaxies[best] / n_galaxies),
                         stellar_contamination=float(contamination[best]), threshold=float(values[best]),
                         selected_galaxies=int(galaxies[best]), selected_stars=int(stars[best]))
        points.append(point)
    return {'galaxies': n_galaxies, 'stars': n_stars, 'operating_points': points}


def load_sample(catalog, truth_path):
    """Load scores and define the common truth-matched comparison sample."""
    data = fits.getdata(catalog, 1)
    truth = np.loadtxt(truth_path, usecols=range(4), ndmin=2)
    observed = SkyCoord(data['world_centroid_alpha'] * u.deg, data['world_centroid_delta'] * u.deg)
    reference = SkyCoord(truth[:, 1] * u.deg, truth[:, 2] * u.deg)
    index, distance, _ = observed.match_to_catalog_sky(reference)
    _, second, _ = observed.match_to_catalog_sky(reference, nthneighbor=2)
    classes = truth[index, 0].astype(int)
    matched = (distance.arcsec <= 0.5) & (second.arcsec > 0.5) & np.isin(classes, (100, 200))
    spread = np.asarray(data['combined_spread_model'], dtype=float)
    error = np.asarray(data['combined_spread_model_error'], dtype=float)
    joint = np.asarray(data['joint_spread_model_alt_delta_chi_squared'], dtype=float)
    radius = np.asarray(data['joint_spread_model_alt_radius'], dtype=float)
    flags = np.asarray(data['joint_spread_model_alt_flags'], dtype=int)
    nframes = np.asarray(data['joint_spread_model_alt_nframes'], dtype=int)
    with np.errstate(divide='ignore', invalid='ignore'):
        significance = spread / error
    # Boundary solutions remain eligible; exclude failed/incomplete fits.
    valid = matched & np.isfinite(spread) & np.isfinite(significance) & (error > 0)
    valid &= np.isfinite(joint) & np.isfinite(radius) & ((flags & ~4) == 0)
    scores = {'weighted_spread': spread, 'weighted_spread_over_error': significance,
              'joint_delta_chi_squared': joint, 'joint_radius': radius}

    if 'joint_spread_model_alt_delta_objective' in data.dtype.names:
        scores['joint_delta_objective'] = np.asarray(data['joint_spread_model_alt_delta_objective'], dtype=float)
    with np.errstate(divide='ignore', invalid='ignore'):
        snr = data['isophotal_flux'] / data['isophotal_flux_err']
    return {'rows': len(data), 'classes': classes, 'matched': matched, 'valid': valid, 'scores': scores,
            'magnitude': truth[index, 3], 'nframes': nframes, 'flags': flags, 'snr': np.asarray(snr, dtype=float)}


def compare(catalog, truth_path):
    """Use a common valid sample for all scores and return overall/binned metrics."""
    sample = load_sample(catalog, truth_path)
    classes, matched, valid = sample['classes'], sample['matched'], sample['valid']
    scores, nframes, flags = sample['scores'], sample['nframes'], sample['flags']

    def summarize(selection):
        return {name: operating_points(values[selection], classes[selection]) for name, values in scores.items()}

    magnitude = sample['magnitude']
    report = {'catalog': str(catalog), 'rows': sample['rows'], 'truth_matched_rows': int(matched.sum()),
              'paired_valid_rows': int(valid.sum()),
              'threshold_rule': 'Score >= threshold; positive class is galaxy (truth type 200)',
              'limitation': 'In-sample thresholds, no independent validation; small-sample contamination is discrete.',
              'assumption': 'One circular morphology across all filters; neighbours not modelled.',
              'joint_flag_counts': {str(int(flag)): int(np.count_nonzero(flags == flag)) for flag in np.unique(flags)},
              'overall': summarize(valid), 'by_truth_g_magnitude': {}, 'by_joint_frame_count': {}}
    for low, high in ((-np.inf, 22), (22, 24), (24, np.inf)):
        report['by_truth_g_magnitude'][f'{low}:{high}'] = summarize(valid & (magnitude >= low) & (magnitude < high))
    for low, high in ((1, 3), (4, 6), (7, 9)):
        report['by_joint_frame_count'][f'{low}:{high}'] = summarize(valid & (nframes >= low) & (nframes <= high))
    return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('catalog', type=Path)
    parser.add_argument('--truth', type=Path, default=Path('sim12_g.list'))
    parser.add_argument('--output', type=Path)
    args = parser.parse_args()
    report = json.dumps(compare(args.catalog, args.truth), indent=2, allow_nan=False) + '\n'
    if args.output:
        args.output.write_text(report)
    print(report, end='')


if __name__ == '__main__':
    main()
