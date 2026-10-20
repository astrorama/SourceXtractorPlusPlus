#!/usr/bin/env python3
"""Copy sim12 measurement images with reproducible WCS offsets; leave pixel data unchanged."""
import argparse
import json
from pathlib import Path

from astropy.io import fits
from astropy.wcs import WCS
import numpy as np


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('output', type=Path)
    parser.add_argument('--sim12', type=Path, default=Path('tests/sim12'))
    parser.add_argument('--sigma', type=float, default=0.3, help='Injected per-axis offset RMS in reference pixels')
    parser.add_argument('--seed', type=int, default=2026)
    args = parser.parse_args()
    if not np.isfinite(args.sigma) or args.sigma < 0:
        parser.error('sigma must be finite and nonnegative')
    root, output = args.sim12.resolve(), args.output.resolve()
    if output == root:
        parser.error('output must differ from the original sim12 directory')
    output.mkdir(parents=True, exist_ok=True)
    frames = sorted(root.glob('sim12_[gri]_0[1-3].fits'))
    if len(frames) != 9:
        parser.error('Expected nine sim12 g/r/i measurement frames')
    reference = WCS(fits.getheader(root / 'sim12_g_01.fits'))
    offsets = np.random.default_rng(args.seed).normal(0, args.sigma, (len(frames), 2))
    offsets -= offsets.mean(axis=0)
    records = []
    for path, offset in zip(frames, offsets):
        with fits.open(path) as hdus:
            frame_wcs = WCS(hdus[0].header)
            # A constant measurement-pixel WCS shift approximates the chosen
            # reference-pixel offset locally at the detection-image centre.
            world = reference.all_pix2world([[768, 768], [768 + offset[0], 768 + offset[1]]], 0)
            projected = frame_wcs.all_world2pix(world, 0)
            delta = projected[1] - projected[0]
            hdus[0].header['CRPIX1'] += delta[0]
            hdus[0].header['CRPIX2'] += delta[1]
            hdus.writeto(output / path.name, overwrite=True)
        records.append({'frame': path.name, 'reference_dx': float(offset[0]), 'reference_dy': float(offset[1]),
                        'measurement_dx': float(delta[0]), 'measurement_dy': float(delta[1])})
    config = (root / 'sersic.py').read_text()
    image_expression = "sorted(glob('sim12_[gri]_0[1-3].fits'))"
    psf_expression = "sorted(glob('sim12_[gri]_0[1-3].psf'))"
    if image_expression not in config or psf_expression not in config:
        parser.error('sersic.py no longer has the expected measurement-image expressions')
    config = config.replace(image_expression, repr([str(output / path.name) for path in frames]), 1)
    config = config.replace(psf_expression, repr([str(path.with_suffix('.psf')) for path in frames]), 1)
    (output / 'sersic_shifted.py').write_text(config)
    (output / 'injections.json').write_text(json.dumps({'seed': args.seed, 'sigma': args.sigma,
                                                       'frames': records}, indent=2) + '\n')
    print(output / 'sersic_shifted.py')


if __name__ == '__main__':
    main()
