# Bug inventory

Reviewed 2026-11-04 against `ce583df9` (`develop`). This is a targeted source review, not an exhaustive audit.
BUG-018 and BUG-019 have since been fixed and regression-tested; other entries retain their review status.
Pure performance opportunities are excluded.
The second review pass focuses on the iterative `FlexibleModelFitting` task and its shared rendering/solver code.
See [model-fitting architecture notes](doc/architecture-model-fitting.md) for the execution and ownership model.

## How to read this inventory

- **Severity:** High includes silent scientific errors, incorrect uncertainties, silent data loss, and potential
  memory corruption. Medium covers rejected valid configurations or delayed failures without established silent errors.
- **Likelihood:** High means a routine input in the affected feature triggers the defect; Medium needs a particular
  geometry, configuration, or memory pressure; Low needs unusual inputs or timing. These are qualitative assessments,
  not measured failure rates. Each entry specifies its affected population.
- **Confirmed:** demonstrated by execution, or established by an explicit code/analytical counterexample. The evidence
  field distinguishes these. Confirmation does not imply an end-to-end production reproduction.
- **Unconfirmed:** a credible candidate requiring the stated experiment. In particular, no entry is established as
  the cause of the intermittently observed multithreading problem.
- **Next action needed:** **Fix** for confirmed defects, **Try to confirm** for candidates. Fixes should include a
  regression test at the affected interface.

## Summary

| ID | Bug | Severity | Likelihood in affected feature | Status | Next action needed |
| --- | --- | --- | --- | --- | --- |
| BUG-001 | Incorrect centroid error-ellipse angle | High | High | Confirmed | Fix |
| BUG-002 | Wrong shape angle for a descending diagonal | High | Medium | Confirmed | Fix |
| BUG-003 | Writes to evicted, retained tiles are lost | High | Medium | Confirmed | Fix |
| BUG-004 | Tile eviction can race with pixel writes | High | Low / timing-dependent | Unconfirmed | Try to confirm |
| BUG-005 | Cached FFT plans may be reused with incompatible alignment | High | Low / platform-dependent | Unconfirmed | Try to confirm |
| BUG-006 | Mismatched weight HDUs silently omit science images | High | Medium | Confirmed | Fix |
| BUG-007 | Only the first output parameter is registered | Medium | Medium | Confirmed | Fix |
| BUG-008 | Weight-cube depth check compares the science cube to itself | Medium | Medium | Confirmed | Fix |
| BUG-009 | Invalid FITS cube layer indices are never rejected | Medium | Low | Confirmed | Fix |
| BUG-010 | Graph-model construction mutates reusable configuration | High | Medium | Confirmed | Fix |
| BUG-011 | Derived-parameter uncertainty ignores shared dependencies | High | Medium | Confirmed | Fix |
| BUG-012 | Vignettes can read outside the detection image | High | Medium | Unconfirmed | Try to confirm |
| BUG-013 | Skipped iterative fits lose their failure flags | High | Medium | Confirmed | Fix |
| BUG-014 | PSF normalization indexed by global image ID | High | Medium | Confirmed | Fix |
| BUG-015 | Iterative fitting ignores finite bad-variance thresholds | High | Medium | Confirmed | Fix |
| BUG-016 | Parameter bookkeeping falsely marks parameters as fitted | High | Medium | Confirmed | Fix |
| BUG-017 | Rotated fitting ellipses transform incorrectly under shear | High | Medium | Confirmed | Fix |
| BUG-018 | GSL leaves runtime parameters at an evaluation probe | High | High with GSL | Fixed | None |
| BUG-019 | Zero-weight pixels artificially shrink solver uncertainties | High | High with masks | Fixed | None |
| BUG-020 | Levmar workspace rejection leaves a global mutex locked | High | Low / build-dependent | Fix pending verification | Verify with conditional locking enabled |
| BUG-021 | Iterative task treats solver memory failure as usable output | High | Low | Confirmed | Fix |
| BUG-022 | Small PSF kernels render point sources as zero | High | Medium | Confirmed | Fix |
| BUG-023 | Fit and stopping diagnostic use different neighbor subtraction | High if unintended | High for blends | Unconfirmed | Try to confirm |

## BUG-001: Incorrect centroid error-ellipse angle

- **Severity:** High: silently incorrect `error_ellipse_theta` in scientific catalogs.
- **Likelihood:** High when requesting error ellipses for anisotropic sources; special orientations can hide the error.
- **Status:** Confirmed by executing the production `ErrorEllipseTask` with controlled properties.
- **Next action needed:** Fix.
- **Location:** [ErrorEllipseTask.cpp:100](SEImplementation/src/lib/Plugin/ErrorEllipse/ErrorEllipseTask.cpp#L100).
- **Description:** The angle calculation takes `abs(var_x - var_y)` and does not divide `atan2` by two.
  It loses the major-axis quadrant and doubles many angles. The equal-variance fallback also ignores covariance sign.
- **Evidence / reproduction:** Give a `SimpleSource` centroid `(1,2)`, pixels `(1,0), (1,4), (0,2), (2,2)`, unit pixel
  variances, and total intensity 4. `ErrorEllipseTask::computeProperties()` reports 0 degrees although the larger
  centroid variance is along Y, so the major axis is at 90 degrees. The output converter only converts radians to
  degrees; it does not correct the angle.
- **Fix direction:** Use `0.5 * atan2(2 * cov_xy, var_x - var_y)` for a nondegenerate ellipse, with an explicit
  convention for an isotropic ellipse. Test both covariance signs and both variance orderings.

## BUG-002: Wrong shape angle for a descending diagonal

- **Severity:** High: silently incorrect `ellipse_theta`; also supplies a wrong initial model-fitting angle.
- **Likelihood:** Medium: requires exactly equal X/Y second moments and negative cross moment. Symmetric diagonal
  footprints, including small pixelized sources, can satisfy this condition.
- **Status:** Confirmed by executing the production `ShapeParametersTask` with controlled properties.
- **Next action needed:** Fix.
- **Location:** [ShapeParametersTask.cpp:75](SEImplementation/src/lib/Plugin/ShapeParameters/ShapeParametersTask.cpp#L75).
- **Description:** Equal diagonal moments unconditionally select +45 degrees. Negative covariance requires -45 degrees.
- **Evidence / reproduction:** Unit-intensity pixels `(0,2), (1,1), (2,0)`, centroid `(1,1)`, and minimum/peak
  values of 1 produce +45 degrees. The source lies along the -45-degree direction. These axes differ by 90 degrees,
  not by the harmless 180-degree ellipse ambiguity.
- **Fix direction:** Retain the covariance sign in the equal-moment case or use the general half-angle formula.

## BUG-003: Writes to evicted, retained tiles are lost

- **Severity:** High: silent corruption of saved image pixels. Known consumers include diagnostic/check images;
  corruption of catalog measurements through this path has not been established.
- **Likelihood:** Medium with writable images and cache pressure; multiple images can evict one another's current tile.
  The failing sequence itself is deterministic and does not require multiple threads.
- **Status:** Confirmed with production tile/cache/image code and an in-memory backing `ImageSource`.
- **Next action needed:** Fix.
- **Locations:** [WriteableBufferedImage.h:52](SEFramework/SEFramework/Image/WriteableBufferedImage.h#L52),
  [TileManager.cpp:163](SEFramework/src/lib/Image/TileManager.cpp#L163).
- **Description:** `WriteableBufferedImage` retains `m_current_tile` after `TileManager` saves and removes that tile.
  Later writes inside it bypass the manager, dirtying an object no longer included in saves or subsequent reads.
- **Evidence / reproduction:** Configure 512x512 float tiles and a 1 MiB cache (one tile). Write A(0,0)=1. Load a tile
  from image B, evicting A. Write A(0,0)=2 through the same writable image. Call `saveAllTiles()` and read A through
  `getChunk()`. Both persisted value and readback are **1**, not 2. The executable returned failure on this assertion.
- **Fix direction:** Coordinate tile residency and writable references; dirty retained tiles must remain discoverable
  and persistable. Add a regression test with two images competing for a one-tile cache.

## BUG-004: Tile eviction can race with pixel writes

- **Severity:** High if triggered: possible dirty-state loss, inconsistent saved pixels, or undefined behavior.
- **Likelihood:** Low / timing-dependent; requires eviction of a writable tile while another thread uses it.
- **Status:** Unconfirmed by runtime race detection. Source inspection identifies inconsistent synchronization;
  BUG-003 independently confirms that writable tiles can outlive their cache entries.
- **Next action needed:** Try to confirm.
- **Locations:** [WriteableBufferedImage.h:56](SEFramework/SEFramework/Image/WriteableBufferedImage.h#L56),
  [ImageTile.cpp:40](SEFramework/src/lib/Image/ImageTile.cpp#L40),
  [TileManager.cpp:170](SEFramework/src/lib/Image/TileManager.cpp#L170),
  [LockedWriteableImage.h](SEImplementation/SEImplementation/Image/LockedWriteableImage.h).
- **Description:** Writers set a plain `bool` dirty flag and mutate the tile buffer. Eviction tests that flag, reads
  the buffer during `saveTile()`, and clears the flag under the manager lock. The image write mutex used by
  `LockedWriteableImage` is not acquired by eviction. These locks therefore do not establish mutual exclusion between
  saving and writing. A write after the save snapshot but before dirty-flag clearing can also be forgotten.
- **Confirmation experiment:** Use two workers and a tiny shared cache. Have one modify an image while the other
  forces eviction using a different image. Instrument a backing source with barriers around its save snapshot;
  check final persisted pixels, then run the production path under ThreadSanitizer. Include writes through the
  locked wrapper so the test exercises the intended usage. Check lock ordering before proposing a fix.
- **Scope:** A plausible rare concurrency defect, not a demonstrated explanation of the user's intermittent failure.

## BUG-005: Cached FFT plans may be reused with incompatible alignment

- **Severity:** High if triggered: invalid FFT execution could crash or invalidate convolved models and fitted results.
- **Likelihood:** Low / platform-dependent; depends on allocator alignment and the FFTW build's SIMD requirements.
  Allocation patterns can change with threading, so this could resemble a threading failure without being a data race.
- **Status:** Unconfirmed in this environment. A local test of 100 independent `vector<float>` buffers found one FFTW
  alignment class, and all shared-plan executions completed successfully.
- **Next action needed:** Try to confirm.
- **Locations:** [FFT.cpp:122](SEFramework/src/lib/FFT/FFT.cpp#L122),
  [FFT.cpp:162](SEFramework/src/lib/FFT/FFT.cpp#L162),
  [FFT.cpp:193](SEFramework/src/lib/FFT/FFT.cpp#L193).
- **Description:** Forward/inverse plans are cached by dimensions only, then executed on other `std::vector` buffers.
  There is no alignment-class check or explicit aligned allocator, and plans do not request `FFTW_UNALIGNED`.
  FFTW requires equivalent alignment for new-array execution unless that flag was used.
- **Reference:** [FFTW new-array execution contract](https://www.fftw.org/doc/New_002darray-Execute-Functions.html).
- **Confirmation experiment:** Record `fftwf_alignment_of` / `fftw_alignment_of` for planning and execution buffers in
  the affected deployment. Exercise repeated sizes, allocation churn, and multiple workers. If classes differ,
  reproduce with those naturally allocated buffers and compare against an aligned-buffer implementation.
  If all supported builds guarantee equivalent classes, close this candidate rather than assume a failure.

## BUG-006: Mismatched weight HDUs silently omit science images

- **Severity:** High: silently omits measurement frames, changing available photometry and joint fitting inputs.
- **Likelihood:** Medium: mismatched multi-extension science/weight inputs or a cube with too few weight-image HDUs.
- **Status:** Confirmed by executing the existing Python loader with controlled FITS metadata.
- **Next action needed:** Fix.
- **Locations:** [measurement_config.py:125](SEImplementation/python/sourcextractor/config/measurement_config.py#L125),
  [measurement_config.py:262](SEImplementation/python/sourcextractor/config/measurement_config.py#L262).
- **Description:** `zip()` stops at the shortest sequence. File-based weight HDU counts are not checked against
  science HDUs/layers, so the loader returns a smaller, apparently valid image group.
- **Evidence / reproduction:** Three science HDUs and one weight HDU load **one** science image without an exception.
  The data-cube loader has the same unchecked truncation when weights are provided as a multi-extension file.
- **Fix direction:** Validate sequence lengths before iteration and reject mismatches explicitly. Any supported
  broadcast behavior should be explicit, rather than implemented through truncation.

## BUG-007: Only the first output parameter is registered

- **Severity:** Medium: a valid output configuration can fail during catalog conversion with a missing-parameter lookup.
- **Likelihood:** Medium: lists of derived/output-only parameters; hidden if every parameter was registered elsewhere.
- **Status:** Confirmed by Python execution and inspection of downstream `.at()` lookups.
- **Next action needed:** Fix.
- **Location:** [measurement_config.py:418](SEImplementation/python/sourcextractor/config/measurement_config.py#L418).
- **Description:** `add_output_column()` stores all requested parameters but registers only `params[0]` with fitting.
  Additional independent derived parameters need not belong to any fitted model and remain unknown.
- **Evidence / reproduction:** Register an output list with independent parameter IDs 100 and 101. The output list
  contains both IDs, while the fitting registration receives only 100. The output converter subsequently requests
  every ID through `FlexibleModelFitting::getParameterValue()`, which uses `.at()`.
- **Fix direction:** Validate and register every output parameter, including its dependencies.

## BUG-008: Weight-cube depth validation always passes

- **Severity:** Medium: accepts incompatible cubes and attempts nonexistent weight layers, generally causing a later
  I/O failure. Silent wrong weights have not been demonstrated.
- **Likelihood:** Medium for mismatched science/weight cube depths.
- **Status:** Confirmed by Python execution.
- **Next action needed:** Fix.
- **Location:** [measurement_config.py:249](SEImplementation/python/sourcextractor/config/measurement_config.py#L249).
- **Description:** The depth check compares `dims[2]` with `cube_size`, which was assigned from `dims[2]`.
  It never checks the weight cube's depth.
- **Evidence / reproduction:** Science dimensions `(10,10,3)` and weight dimensions `(10,10,1)` are accepted and
  assigned weight layers `[0,1,2]`.
- **Fix direction:** Compare `weight_dims[2]` with the science depth; validate spatial dimensions as well.

## BUG-009: Invalid FITS cube layers are never rejected

- **Severity:** Medium: invalid layer requests reach CFITSIO instead of producing the intended immediate error.
- **Likelihood:** Low for valid configuration; also reachable through BUG-008.
- **Status:** Confirmed by inspection: the guard is logically impossible for a positive cube depth.
- **Next action needed:** Fix.
- **Location:** [FitsImageSource.cpp:282](SEFramework/src/lib/FITS/FitsImageSource.cpp#L282).
- **Description:** `layer < 0 && layer >= m_depth` cannot hold. Both negative and past-the-end layers are accepted.
- **Evidence / reproduction:** For depth 3, both `setLayer(-1)` and `setLayer(3)` bypass the check and change the active
  layer. This establishes the validation bug; subsequent CFITSIO failure behavior was not exercised.
- **Fix direction:** Use an OR condition and test -1, 0, depth-1, and depth. Separately specify cache behavior if the
  active layer changes after reads; the cache key currently does not contain the layer.

## BUG-010: Graph-model construction mutates reusable configuration

- **Severity:** High: subsequent models can silently lose configured scale, aspect ratio, and angle.
- **Likelihood:** Medium when a caller reuses a parameter dictionary for several models.
- **Status:** Confirmed by executing the existing Python constructor with controlled parameter objects.
- **Next action needed:** Fix.
- **Location:** [model_fitting.py:758](SEImplementation/python/sourcextractor/config/model_fitting.py#L758).
- **Description:** `ComputeGraphModel` replaces numeric entries in the caller's dictionary, removes `_scale`,
  `_aspect_ratio`, and `_angle`, and stores the same mutable dictionary as `self.params`.
- **Evidence / reproduction:** Construct two models with the same dictionary containing `_scale = parameter(2)`.
  The first model uses that parameter; the second silently uses the default scale 1 because construction of the first
  removed the key. The caller's dictionary now contains only the nonreserved entries.
- **Fix direction:** Make an internal copy before conversion/removal; avoid the mutable default argument as part of
  the same correction. Verify that constructing another model cannot modify an earlier model's parameters.

## BUG-011: Derived-parameter uncertainties ignore shared dependencies

- **Severity:** High: silently incorrect reported scientific uncertainties, even when derived values are correct.
- **Likelihood:** Medium for expressions combining correlated fit parameters or nested/shared dependencies.
  Single-variable derived parameters do not exhibit this particular error.
- **Status:** Confirmed by an analytical counterexample to the implemented propagation formula; no engine-level
  reproduction was run.
- **Next action needed:** Fix.
- **Location:** [FlexibleModelFittingParameter.cpp:221](SEImplementation/src/lib/Plugin/FlexibleModelFitting/FlexibleModelFittingParameter.cpp#L221).
- **Description:** Propagation adds squared partial derivatives times marginal variances, treating immediate
  dependencies as independent. It loses their covariance and can double-count the same underlying free parameter.
- **Evidence / reproduction:** The API permits `DependentParameter(lambda a,b: a-b, x, x)`. This is identically zero,
  with zero uncertainty. With nonzero `sigma_x`, the implemented formula gives `sqrt(2) * sigma_x`. The same issue can
  occur when distinct derived parameters share a free ancestor. General propagation requires `J C J^T`.
- **Fix direction:** Differentiate through dependencies to distinct free parameters and propagate their covariance.
  Test repeated dependencies, shared ancestors, and correlated fit parameters; distinguish this from the separately
  documented approximation that ignores covariance between image pixels.

## BUG-012: Vignettes can read outside the detection image

- **Severity:** High if triggered: out-of-bounds access can abort, corrupt a vignette, or cause undefined behavior.
- **Likelihood:** Medium with measurement images extending beyond detection coverage, edge sources, or failed WCS
  transforms. Aligned images with interior sources conceal it.
- **Status:** Unconfirmed by a full vignette-task reproduction; the missing bounds check is visible in source.
- **Next action needed:** Try to confirm.
- **Location:** [VignetSourceTask.cpp:88](SEImplementation/src/lib/Plugin/Vignet/VignetSourceTask.cpp#L88).
- **Description:** The loop checks measurement-image bounds, transforms the coordinate into the detection frame,
  and calls `detection_thresh_image->getValue(detection_x, detection_y)` without checking detection-frame bounds.
  Valid measurement coordinates need not lie inside the detection image. `worldToImage()` can also return a large
  negative sentinel after a failed transformation.
- **Confirmation experiment:** Use partially overlapping detection/measurement images and place a requested vignette
  across the detection boundary while still inside the measurement image. Run with assertions and AddressSanitizer;
  specify the expected masking behavior outside detection coverage before fixing it.

## BUG-013: Skipped iterative fits lose their failure flags

- **Severity:** High: an unperformed fit can appear unflagged, with zero chi-square and bogus non-free outputs.
- **Likelihood:** Medium: sources outside all modeled frames, or too few usable pixels for the registered parameters.
- **Status:** Confirmed by calling the production iterative task on a source outside a configured frame.
- **Next action needed:** Fix.
- **Location:** [FlexibleModelFittingIterativeTask.cpp:723](SEImplementation/src/lib/Plugin/FlexibleModelFitting/FlexibleModelFittingIterativeTask.cpp#L723).
- **Description:** `fitSource()` sets a local `OUTSIDE` or `INSUFFICIENT_DATA` flag, then returns without updating
  `SourceState`. Initial flags and chi-square remain zero. Initial constant/dependent saved values are also zero,
  even though the runtime parameter objects may already contain their real values.
- **Evidence / reproduction:** One configured frame with an invalid/empty source rectangle and a constant parameter
  equal to 7 produces flags **0**, reduced chi-square **0**, and constant output **0**. Expected behavior includes
  `OUTSIDE` (32), an explicitly unavailable fit statistic, and meaningful or unavailable constant/derived outputs.
  The insufficient-data branch has the same early return; it was checked by inspection, not separately executed.
- **Fix direction:** Publish a complete skipped-fit state, including failure flags and defined unavailable results,
  before returning. Test outside coverage, complete masking, and insufficient usable pixels.

## BUG-014: PSF normalization is indexed by global image ID

- **Severity:** High: silently changes rendered flux normalization; out-of-range accesses also cause undefined behavior.
- **Likelihood:** Medium: image IDs have gaps, start above zero, or are interleaved with other configuration objects.
  A wrong normalization choice changes rendering when the PSF is downsampled.
- **Status:** Confirmed by production `createFrameModel()` rendering with controlled frame IDs and PSF flags.
- **Next action needed:** Fix.
- **Locations:** [FlexibleModelFittingTaskFactory.cpp:101](SEImplementation/src/lib/Plugin/FlexibleModelFitting/FlexibleModelFittingTaskFactory.cpp#L101),
  [FlexibleModelFittingIterativeTask.cpp:283](SEImplementation/src/lib/Plugin/FlexibleModelFitting/FlexibleModelFittingIterativeTask.cpp#L283).
- **Description:** The factory appends normalization flags in image-list order, but the task indexes the vector by
  frame number. Frame numbers are global `PyId` values, not dense vector positions.
- **Evidence / reproduction:** Image IDs `[1,2]` and flags `[false,true]`: rendering frame 1 with downsampling 0.5
  reads the second image's flag. A centered point source with a 33x33 PSF of integral 2 renders integral **1.02542**;
  the same frame with the correct false flag renders **2.05084**. The small interpolation offset is common to both;
  the erroneous factor of two comes from selecting the wrong flag. The largest frame ID also exceeds vector size.
- **Fix direction:** Store options keyed by image ID, or attach them directly to each fitting-frame definition.
  Test sparse IDs and different flags per frame. Audit other containers accessed with `getFrameNb()` similarly.

## BUG-015: Iterative fitting ignores finite bad-variance thresholds

- **Severity:** High: pixels explicitly marked unreliable can influence fitted parameters and uncertainties.
- **Likelihood:** Medium: a configured finite variance threshold, with pixels beyond it that remain finite after
  interpolation. Measurement frames use zero interpolation gap in the current configuration path.
- **Status:** Confirmed by executing production `createWeightImage()` through a real `MeasurementImageFrame`.
- **Next action needed:** Fix.
- **Location:** [FlexibleModelFittingIterativeTask.cpp:353](SEImplementation/src/lib/Plugin/FlexibleModelFitting/FlexibleModelFittingIterativeTask.cpp#L353).
- **Description:** The weight builder uses variance to calculate inverse sigma but never compares it with
  `MeasurementFrameInfo::getVarianceThreshold()`. Infinite variance happens to give zero weight, concealing the
  missing threshold check for finite bad variances.
- **Evidence / reproduction:** A 5x5 frame with variance 100, configured threshold 10, zero interpolation gap,
  disabled saturation, and infinite gain gives center weight **0.1**, although that pixel should be excluded.
  The configured threshold is present in both frame and property metadata.
- **Fix direction:** Honor the masking threshold using the appropriate original/current variance representation.
  Define handling of zero, negative, and non-finite variances, and verify exclusion from usable-pixel counts.

## BUG-016: Parameter bookkeeping falsely marks parameters as fitted

- **Severity:** High: suppresses partial-fit reporting and can present unconstrained parameters as fitted outputs.
- **Likelihood:** Medium with parameters belonging to frames that do not cover a source, or output/prior-only free
  parameters. Ordinary fits in which every parameter is constrained conceal the problem.
- **Status:** Confirmed by production parameter preparation and state-update methods, plus call-path inspection.
- **Next action needed:** Fix.
- **Locations:** [FlexibleModelFittingIterativeTask.cpp:706](SEImplementation/src/lib/Plugin/FlexibleModelFitting/FlexibleModelFittingIterativeTask.cpp#L706),
  [FlexibleModelFittingIterativeTask.cpp:645](SEImplementation/src/lib/Plugin/FlexibleModelFitting/FlexibleModelFittingIterativeTask.cpp#L645),
  [FlexibleModelFittingParameterManager.h:57](SEImplementation/SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingParameterManager.h#L57).
- **Description:** `getParameter()` marks a parameter and its dependencies as accessed. Restoring values before each
  fit calls it for every parameter, and state collection calls it even for an unused parameter. Access tracking is
  cleared during initial preparation only, so maintenance reads contaminate the evidence of model use.
- **Evidence / reproduction:** Prepare two free parameters, access only the first, and collect state. The second is
  initially classified unfitted. Restore its saved value and collect state again without using it in any model:
  it is now classified **fitted**. The actual `fitSource()` restore loop contaminates all parameters even before the
  first model preparation. Neighbor rendering also reads every parameter while restoring neighbor state.
- **Fix direction:** Separate nontracking lookups from model dependency tracking, reset tracking at a well-defined
  boundary, and snapshot the genuinely constrained parameter set before diagnostics/output collection.
  Also inspect solver registration: currently every free parameter is registered even if its frames are unavailable.

## BUG-017: Rotated fitting ellipses transform incorrectly under shear

- **Severity:** High: selects an incorrect scientific fitting region, changing which data constrain the fit.
- **Likelihood:** Medium with `ROTATED_ELLIPSE` and a Jacobian that does not preserve the orthogonality of the source's
  principal axes. Pure rotation and uniform scaling conceal it.
- **Status:** Confirmed by executing production `transformEllipse()` with a controlled Jacobian.
- **Next action needed:** Fix.
- **Location:** [FlexibleModelFittingIterativeTask.cpp:231](SEImplementation/src/lib/Plugin/FlexibleModelFitting/FlexibleModelFittingIterativeTask.cpp#L231).
- **Description:** The method transforms the two original axis vectors, takes their lengths as new semiaxes, and uses
  the transformed major vector as the new angle. Under shear those vectors are no longer orthogonal, so this does
  not describe the transformed ellipse.
- **Evidence / reproduction:** An ellipse with semiaxes `(2,1)`, angle 0, transformed by `J=[[1,1],[0,1]]` returns
  semiaxes `(2,sqrt(2))`, angle 0. Its area factor becomes **2.82843**, instead of **2**, despite `det(J)=1`.
  The correct squared-axis matrix is `[[5,1],[1,1]]`, which has nonzero rotation.
- **Fix direction:** Transform the ellipse's symmetric shape matrix with `J Q J^T`, then obtain semiaxes and angle
  by eigendecomposition. Test rotation, anisotropic scaling, shear, and reflection.

## BUG-018: GSL leaves runtime parameters at an evaluation probe

- **Severity:** High: silently reports perturbed parameter values and evaluates diagnostics at those values.
- **Likelihood:** High with GSL's numerical differentiation; magnitude depends on derivative step and parameter scale.
- **Status:** Fixed; originally confirmed by executing the production `GSLEngine` with the installed GSL library.
- **Next action needed:** None.
- **Locations:** [GSLEngine.cpp:161](ModelFitting/src/lib/Engine/GSLEngine.cpp#L161),
  [GSLEngine.cpp:262](ModelFitting/src/lib/Engine/GSLEngine.cpp#L262).
- **Description:** Residual callbacks update the runtime parameter manager for every evaluation, including numerical
  derivative probes. After solving, the engine does not restore the accepted solution before converting covariance
  or returning. The iterative task reads those runtime parameters directly.
- **Evidence / reproduction:** One parameter initialized to 1 and two unit-weight residuals `(x-3)` have exact
  optimum 3. With the default derivative step, the solver reports success but the runtime parameter is
  **3.00029999993**, consistent with a derivative probe near 3 rather than the accepted solution.
- **Fix direction:** Restore the solver's accepted position to `EngineParameterManager` before covariance conversion
  and before returning. Check both successful and early/error termination paths. The analogous Levmar callback-only
  restoration pattern deserves its own test; this execution confirms GSL only.
- **Resolution:** After the driver returns, copy `gsl_multifit_nlinear_position(workspace)` into the runtime
  parameter manager before covariance conversion. This also applies to iteration-limit and other driver errors.
  Initialization errors restore the input guesses and throw before using the solver's output.
- **Regression coverage:** `ModelFitting.GSLEngine_test` exercises accepted values and dependent parameters,
  iteration-limit cost consistency, and nonlinear covariance conversion. All three cases failed before the fix
  and pass afterward. `make tests` rebuilt the project and passed all 74 registered tests.

## BUG-019: Zero-weight pixels artificially shrink solver uncertainties

- **Severity:** High: silently underestimates scientific parameter uncertainties.
- **Likelihood:** High with GSL or Levmar when masks, saturation, or nonrectangular windows leave zero-weight pixels.
- **Status:** Fixed; confirmed by executing both production engines with the shared weighted residual provider.
- **Locations:** [DataVsModelResiduals.icpp](ModelFitting/ModelFitting/Engine/_impl/DataVsModelResiduals.icpp),
  [GSLEngine.cpp](ModelFitting/src/lib/Engine/GSLEngine.cpp),
  [LevmarEngine.cpp](ModelFitting/src/lib/Engine/LevmarEngine.cpp).
- **Next action needed:** None.
- **Description:** Every pixel in a rectangular stamp remained a solver residual, even when its weight was zero.
  GSL's wrapper explicitly scales covariance by squared residuals divided by residual count minus parameter count;
  Levmar performs the equivalent scaling internally for full-rank problems. Adding excluded pixels therefore
  increased the denominator without adding information or changing the objective.
- **Evidence / reproduction:** Fit one parameter to targets 1 and 3 with unit weights. Reported sigma is **1**.
  Append 98 zero-weight pixels: the objective is unchanged, but sigma becomes **0.100504** in both engines.
  The ratio is `sqrt(1/99)`. The iterative diagnostic's usable-pixel count does not repair solver-provided sigmas.
- **Resolution:** `DataVsModelResiduals` counts nonzero weights at construction and skips zero-weight pixels
  during each residual evaluation. Data, weights, and model rendering retain their rectangular geometry and are
  traversed together to preserve alignment. Both engines now receive the usable residual count, with priors still
  adding their existing residuals. Shared weights must retain their zero/nonzero pattern for the provider's
  lifetime. Evaluation throws if the emitted count differs from the cached count. Both solvers relay callback
  exceptions after the library call returns. `ResidualEstimator` safely skips empty blocks. Moffat skips the
  solver if there are fewer usable pixels than free parameters and reports an unavailable fit.
- **Regression coverage:** `ModelFitting.DataVsModelResiduals_test` checks uncertainty, fitted-value, and cost
  invariance under masked padding for both available engines, with and without a prior, and with the iterative
  task's robust asinh comparator. Provider tests cover pixel alignment with input iterators, model refresh,
  zero-valued usable residuals, masked non-finite data, empty/mixed blocks, size validation, and usable counts at
  insufficient-data boundaries. The padding regression failed for both engines before the fix.
  `make tests` rebuilt the project and passed all 75 registered tests, including BUG-018's existing regressions.
- **Scope:** Retains the existing covariance scaling convention and prior treatment. It does not redefine
  uncertainty estimates for rank-deficient fits or fits without positive residual degrees of freedom.

## BUG-020: Levmar workspace rejection leaves a global mutex locked

- **Severity:** High: subsequent solves can deadlock, preventing the run from completing.
- **Likelihood:** Low: requires a Levmar build with `LINSOLVERS_RETAIN_MEMORY` and a workspace allocation failure or
  rejection by the workspace-size cap. Relevant to the rare multithreading investigation only under these conditions.
- **Status:** Reproduced before the lock was moved. The conditional locking build still needs verification.
- **Next action needed:** Repeat the forced workspace rejection with conditional locking enabled.
- **Location:** [LevmarEngine.cpp](ModelFitting/src/lib/Engine/LevmarEngine.cpp).
- **Description:** The conditional global mutex was locked before workspace allocation. The `MEMORY` early return
  bypassed the unlock, so future calls could block. The wrapper now acquires the mutex after workspace allocation
  and rejection handling, immediately before calling Levmar.
- **Evidence / reproduction:** Compile this source with `LINSOLVERS_RETAIN_MEMORY` and a zero workspace cap to force
  rejection without allocating large memory. First solve returns `MEMORY`; the second prints no completion and is
  terminated by a two-second timeout (exit 124). An unreachable numerical-entry stub replaces the absent Levmar
  library; neither call reaches it. This confirms the engine's lock path, not a failure in the numerical library.
- **Verification:** The default build passes the solver callback exception regression. Check workspace rejection
  and callback exception handling with conditional locking enabled. Check the deployed build option before treating
  this as an explanation for the user's observed rare failure.

## BUG-021: Iterative task treats solver memory failure as usable output

- **Severity:** High: unchanged guesses and zero uncertainties can be published without a memory/error flag.
- **Likelihood:** Low: Levmar workspace rejection/allocation failure; separate from BUG-020's conditional locking.
- **Status:** Confirmed by inspection of the engine return and iterative caller; no full task failure injection run.
- **Next action needed:** Fix.
- **Locations:** [LevmarEngine.cpp:170](ModelFitting/src/lib/Engine/LevmarEngine.cpp#L170),
  [FlexibleModelFittingIterativeTask.cpp:756](SEImplementation/src/lib/Plugin/FlexibleModelFitting/FlexibleModelFittingIterativeTask.cpp#L756).
- **Description:** The engine returns `LeastSquareSummary::MEMORY` with zero-filled parameter sigmas and has not
  optimized the parameters. The iterative task checks only `ERROR`, then computes diagnostics and collects values
  and sigmas through the normal result path. `Flags::MEMORY` exists but is not set here.
- **Evidence / reproduction:** The confirmed workspace-rejection harness returns status 3 (`MEMORY`). This status
  does not satisfy the caller's sole failure check against `ERROR`; allocation of the sigma vector initializes its
  entries to zero. The propagation is evident in source, independent of whether the conditional mutex is enabled.
- **Fix direction:** Handle every solver status explicitly. Memory failure must retain failure flags and unavailable
  uncertainties, and must not present a skipped optimization as a successful fit.

## BUG-022: Small PSF kernels render point sources as zero

- **Severity:** High: silently removes point-source model signal and can invalidate fitted fluxes.
- **Likelihood:** Medium: square odd PSFs of size 1, 3, 5, or 7, including kernels reduced below eight pixels by
  downsampling. The PSF wrapper accepts these sizes.
- **Status:** Confirmed by rendering through production iterative `createFrameModel()` and image interpolation.
- **Next action needed:** Fix.
- **Location:** [ImageInterfaceTraits.cpp:274](SEImplementation/src/lib/Image/ImageInterfaceTraits.cpp#L274).
- **Description:** The fast Lanczos-4 resampler requires its entire eight-sample support inside the source image.
  For a dimension below eight, every row/column is skipped and the destination remains zero. Point-model rendering
  uses this resampler to place the PSF, even without a requested size reduction.
- **Evidence / reproduction:** An accepted 5x5 PSF with a central value 2 and zeros elsewhere, centered in an 11x11
  model stamp at unit scale, produces rendered point-source integral **0**, instead of **2**. Reducing a 5x5 kernel
  to 3x3 also encounters this path; normalization and later rendering can compound the failure.
- **Fix direction:** Provide suitable padding/boundary handling or a valid small-kernel interpolation path.
  Test PSF sizes below the interpolation support and PSFs crossing that boundary during downsampling.

## BUG-023: Fit and stopping diagnostic use different neighbor subtraction

- **Severity:** High if unintended: affects reported chi-square and potentially meta-iteration stopping.
- **Likelihood:** High for blended groups with the default deblend factor 0.95; absent for isolated sources or factor 1.
- **Status:** Unconfirmed as a bug: the code paths differ, but the intended statistical meaning needs confirmation.
- **Next action needed:** Try to confirm.
- **Locations:** [FlexibleModelFittingIterativeTask.cpp:585](SEImplementation/src/lib/Plugin/FlexibleModelFitting/FlexibleModelFittingIterativeTask.cpp#L585),
  [FlexibleModelFittingIterativeTask.cpp:873](SEImplementation/src/lib/Plugin/FlexibleModelFitting/FlexibleModelFittingIterativeTask.cpp#L873).
- **Description:** Residual construction subtracts `deblend_factor * neighbor_model`. Chi-square evaluation subtracts
  the full neighbor model. The latter diagnostic also drives meta-iteration stopping. This difference is additional
  to the intentional distinction between robust asinh fitting and ordinary chi-square diagnostics.
- **Counterexample / confirmation experiment:** With target model 10, neighbor model 20, observed value 29, and
  factor 0.95, the fit's untransformed residual is `29-19-10=0`; the diagnostic residual is `29-20-10=-1`.
  Establish whether a physical full-group residual is intended, then test blended synthetic sources and the stopping
  criterion across deblend factors. Do not automatically make the diagnostic match the fitting objective.

## Validation and follow-up: first review pass

- Executed isolated C++ harnesses against the production error-ellipse, shape, tile-manager, tile, and buffered-image
  implementations. The tile harness supplied an in-memory backing source; the numerical harness used `SimpleSource`.
  Only unavailable Elements logging/exception dependencies were stubbed. These were not full FITS integration runs.
- Executed the actual Python class definitions with stubbed FITS metadata, image wrappers, and parameter registration.
  This confirms loader/constructor control flow, not FITS I/O or fitting-engine behavior.
- Executed the production FFT wrapper on 100 independent float vectors; the alignment candidate did not reproduce.
- Harnesses were temporary files under `/tmp/sourcextractor-bug-audit`; the recipes and observed results above are
  recorded here so confirmation does not depend on retaining that temporary directory.
- No full project build, `make tests`, sanitizer run, or end-to-end catalog comparison was performed for this
  documentation-only change. Confirmed defects still need committed regression tests when fixed.
- Investigate BUG-004 first for a genuine concurrency failure. Check BUG-005 on the affected deployment as a possible
  allocation-sensitive alternative. Neither is identified as the cause of the reported rare failure.

## Validation and follow-up: iterative fitting review

- Compiled and executed the unmodified production iterative task, parameter implementation, frame/image layers,
  PSF wrapper/resampler, and GSL backend in an isolated harness. Used the existing friend-fixture seam for private
  methods and `SimpleSource`/`SimpleSourceGroup` for property inputs.
- Stubbed unavailable Elements logging/exception and Alexandria allocation helpers. A no-output `CheckImages` stub
  disabled diagnostic file output. The numerical GSL, FFTW, rendering, and fitting-task implementations were real.
  Frame/PSF/model fixtures are synthetic; this was not an end-to-end FITS catalog test.
- Separately compiled the real Levmar wrapper with its conditional lock enabled and a forced zero workspace cap.
  Numerical entry was an aborting stub and was never called. The two-call timeout confirms only the memory-return
  locking path; it does not establish the deployed Levmar configuration.
- Temporary harnesses and captured output are under `/tmp/sourcextractor-fitting-audit`. Reproduction inputs and
  limitations are recorded in the entries above. No production fixes, permanent regression tests, or full project
  `make tests` run are included in this documentation update.
- Highest-value follow-up: investigate the conditional deadlock (BUG-020) on the affected build, and address silent
  model/uncertainty errors with targeted regression tests. The legacy non-iterative task was not re-audited.
