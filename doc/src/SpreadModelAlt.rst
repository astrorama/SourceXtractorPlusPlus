Experimental joint morphology fitting
====================================

Request ``CombinedSpreadModelAlt`` in ``output-properties`` to write both the
inverse-variance combined spread measurements and the experimental joint-fit
columns. ``SpreadModelAlt`` writes the separate per-frame spread measurements.
The original ``SpreadModel`` and ``CombinedSpreadModel`` remain available for
comparison.

The joint task fits one isolated source across all eligible measurement frames.
It compares a point source against a circular exponential source with a shared
position and intrinsic half-light radius in reference-image pixels. Each frame
has its own fitted flux, position offset, PSF, noise weights and coordinate transformation.
A shared radius across filters assumes wavelength-independent morphology.
Neighbours are not fitted or subtracted by this task.

Frames must contain the initial projected source centre within their full pixel
edges, have a usable PSF and coordinate Jacobian, and contain at least two valid
pixels. Both hypotheses use exactly the same frames and pixel masks. The total
number of valid pixels must exceed the number of frames plus three. The two
additional offset parameters per frame have two Gaussian prior residuals; priors
are excluded from the reported pixel count. Fitted centres outside an included frame
are checked after applying that frame's offset and invalidate that solution; frames are not dropped after optimization.

Residuals use ordinary inverse-noise chi-square, with independent pixel noise
and fixed PSFs. The shared centroid can move up to five reference-image pixels
per axis from its initial value. The exponential fit uses two starting radii,
0.3 and 1.5 reference-image pixels, initialized from the joint point fit, including its per-frame offsets.
Its positive radius range starts at 0.01 pixels and ends at the larger of two
pixels or half the largest equivalent reference-frame stamp size. The explicit
point fit supplies the zero-radius limit without evaluating a singular
exponential. The same rendering scale is used for both hypotheses.

Per-frame astrometric offsets
-----------------------------

``spread-model-alt-position-prior-sigma`` sets the Gaussian prior's per-axis
standard deviation in reference-image pixels (default 0.2). Zero removes the
extra parameters and exactly restores the shared-position model. Negative or
non-finite values are rejected. The default is an experimental starting value,
not a calibrated uncertainty for a particular instrument.

For each frame the model projects (common_x + offset_x, common_y + offset_y)
through that frame's WCS, so the prior has the same reference-coordinate units
across different rotations and pixel scales. Both hypotheses use identical
zero-centred Gaussian priors and minimize pixel chi-square plus the sum of
(offset_x / sigma) squared and (offset_y / sigma) squared. The common centre is
not tied to a designated frame; the priors prefer zero mean offsets. Offsets
have no additional hard bounds, but fitted centres must remain inside every
included frame. Low-information frames are pulled toward the common centre.

The radius uncertainty includes the fitted offset nuisance parameters and prior
curvature through the solver covariance. It remains conditional on fixed PSFs
and pixel variances. Independent frame offsets can absorb proper motion, but
there is no explicit time-dependent motion model.

Outputs
-------

All joint columns have the prefix ``joint_spread_model_alt_``:

* ``radius``: intrinsic exponential half-light radius in reference-image pixels;
  zero when the point-source limit has the lowest penalized objective.
* ``radius_error``: formal solver uncertainty; NaN for boundary solutions or
  unavailable covariance. This is not a calibrated confidence interval near zero.
* ``delta_chi_squared``: point chi-square minus extended chi-square, including
  the point-source limit. Larger values favour extension. Its square root is
  not automatically a Gaussian detection significance. With position priors this
  raw pixel difference may be negative: selection uses ``delta_objective``.
* ``point_chi_squared`` and ``extended_chi_squared``: raw sums over weighted
  pixel residuals, not reduced chi-square.
* ``point_prior_penalty`` and ``extended_prior_penalty``: sums of squared
  position offsets divided by the squared prior sigma, for each hypothesis.
* ``delta_objective``: point minus extended (pixel chi-square + prior penalty).
  This is the quantity used for selecting the extended solution, including the
  point-source limit, and is nonnegative for a valid comparison. It is not a
  calibrated significance or a marginalized evidence ratio.
* ``point_offset_rms`` and ``extended_offset_rms``: RMS radial offset in reference
  pixels, namely sqrt(mean(dx squared + dy squared)) over contributing frames.
* ``frame_ids``: configured measurement frame IDs in the offset arrays' order.
* ``point_offset_x``, ``point_offset_y``, ``extended_offset_x`` and
  ``extended_offset_y``: fitted offsets in reference pixels, with a fixed entry
  for every configured frame. Excluded or failed-fit entries are NaN. Radius-zero
  solutions use the point fit's offsets in both sets of arrays.
* ``nframes`` and ``npixels``: counts used by both hypotheses.
* ``flags``: bit mask described below. Zero means no reported fit problem.

Flags can be combined:

* 1: solver failure or non-finite fit result.
* 2: iteration limit reached; inspect before using the model comparison.
* 4: radius boundary (zero, at most 0.02 pixels, or at least 99% of the upper bound).
* 8: a fitted centre left an included frame.
* 16: insufficient usable data.

The fitting engine, iteration limit, rendering scale and maximum fit size follow
``model-fitting-engine``, ``model-fitting-iterations``, ``sampling-scale-factor``
and ``sampling-max-fit-size`` respectively. Per-frame offsets increase the
parameter count and fitting cost; short iteration budgets such as 100 may
produce more iteration-limit flags. The normal iteration default is 1000.

These joint scores have different units and meaning from SPREAD_MODEL. The
existing combined-spread columns retain their weighted-mean definition and
frame-agreement reduced chi-square diagnostic.

Comparing on sim12
------------------

After ``make install``, run the installed executable in its installed environment
from ``tests/sim12``, for example::

  sourcextractor++ --config-file sourcextractor++.conf \
    --output-properties WorldCentroid,IsophotalFlux,CombinedSpreadModel,CombinedSpreadModelAlt \
    --model-fitting-iterations 1000 --output-catalog-filename /tmp/joint-spread-comparison.fits
  python3 ../../SEImplementation/tests/scripts/compare_joint_spread.py /tmp/joint-spread-comparison.fits \
    --output /tmp/joint-spread-comparison.json

The comparison uses the same valid truth-matched sources for every score and
reports galaxy completeness at fixed empirical stellar contamination, overall
and by magnitude and contributing frame count. Thresholds are selected on the
same sample; use independent simulations to assess generalization. Seeing and
PSF systematics require additional validation.

To generate the paired score plots and classification curves (requires
Matplotlib in addition to NumPy and Astropy)::

  python3 ../../SEImplementation/tests/scripts/plot_joint_spread.py /tmp/joint-spread-comparison.fits \
    --prefix /tmp/joint_spread_comparison

This writes PNG and SVG figures, plus a JSON summary of the plotted ROC areas
and empirical completeness values. All curves use the same truth-matched sample.

Testing injected astrometric errors
----------------------------------

The helper below copies the nine measurement images and changes their WCS
reference pixels without changing image data. It writes the injected shifts
and a Python configuration selecting those copies::

  python3 SEImplementation/tests/scripts/inject_sim12_astrometry.py \
    tests/sim12/joint_spread_offsets/injected --sigma 0.3 --seed 2026

From ``tests/sim12``, select
``--python-config-file joint_spread_offsets/injected/sersic_shifted.py`` and run
once with ``--spread-model-alt-position-prior-sigma 0`` and once with ``0.2``.
Use different output catalog filenames to preserve both runs. Keep the original
detection image and reference coordinates so the shifted WCS represents a
measurement-frame astrometric error. The injection is an approximate constant
reference-pixel displacement at the detection-image centre; each frame gets
its own corresponding measurement-pixel shift.

For a comparison of all available spread and joint-fit scores, prepare matched
1000-iteration catalogs in the ``clean_shared``, ``clean_offsets``,
``shifted_shared`` and ``shifted_offsets`` subdirectories, then run::

  python3 SEImplementation/tests/scripts/plot_spread_methods.py tests/sim12/method_comparison

The saved ``tests/sim12/method_comparison/README.md`` describes the current runs.
This figure uses one common source sample across every method and both
astrometric conditions, with ROC curves and recovery at contamination limits.
