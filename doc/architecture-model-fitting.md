# Model-fitting architecture and review notes

Working notes from source inspection, started 2026-11-04 at `ce583df9`. These describe the implementation,
not a proposed redesign. The iterative task is the active review target; `FlexibleModelFittingTask` is legacy.
Confirmed defects and reproduction details belong in [BUGS.md](../BUGS.md).

## Configuration and identity

Python `MeasurementConfig` owns measurement images and a `ModelFitting` configuration. The latter registers
constant/free/dependent parameters, model definitions, frame-to-model assignments, priors, and requested outputs.
`PythonInterpreter` exports these dictionaries. `ModelFittingConfig` constructs corresponding C++ definitions.
`FlexibleModelFittingTaskFactory` selects the iterative task and passes its immutable configuration to it.

- A frame number is the measurement image's `PyId`, not necessarily its position in a vector.
- `PyId` is a shared increasing counter used by configuration objects. Images created after other objects or
  interleaved with parameter creation can have sparse IDs.
- Parameter definitions are shared across sources, but each source receives its own runtime parameter objects.
- Source properties are lazy: requesting a fitting result through an entangled source can execute one group task
  that populates results on every source in that group.

## Runtime state and iteration

`computeProperties(group)` creates a local `FittingState` with one `SourceState` per source in group iteration order.
Each state owns a parameter manager and engine parameter manager, plus saved initial/current values, sigmas,
access/fitted flags, diagnostics, and fitting-region dimensions.

1. Evaluate free-parameter initial values and construct runtime parameters and their dependency graph.
2. Repeat up to `meta_iterations`: fit each source in group order, using saved neighbor models for deblending.
3. Average saved per-source reduced chi-squares and compare with the previous pass for early stopping.
4. Mark unfitted free outputs NaN, publish `FlexibleModelFitting` properties, then update check images.

This is sequential source fitting within a group: later sources in a pass can use earlier sources' newly saved
values. Groups can be processed concurrently. Current task methods are `const`; their fitting state is local.
This alone does not establish thread safety of all shared dependencies.

## One source fit

`fitSource()` restores saved engine values, determines model downsampling, and builds residual providers for
geometrically valid frames. Each provider contains a copy of background-subtracted image pixels minus scaled
neighbor models, a model of the target source, inverse-standard-deviation weights, and an asinh comparator.
The shared `DataVsModelResiduals` provider counts nonzero weights at construction and skips zero-weight pixels
on each evaluation, traversing data, the full rectangular model rendering, and weights together. Shared weights
must retain their zero/nonzero pattern for the provider's lifetime. A block evaluation throws if the count changes.
Empty blocks are skipped by `ResidualEstimator`. Priors add additional residual blocks. A newly created solver
updates the runtime parameters through callbacks and relays callback exceptions after returning from the library.
The task separately computes ordinary chi-square and copies parameter values/sigmas into saved state.

Important distinction: the objective minimized is robust asinh residuals, whereas the reported diagnostic is
ordinary weighted chi-square. Differences caused by that choice are not automatically bugs.

## Geometry, weights, and rendering

- `MeasurementFrameRectangle` provides the transformed footprint; the task expands and clips it to each image.
- Alternative window modes derive square, disk, aligned-ellipse, or transformed shape-ellipse masks.
- `createWeightImage()` reads background variance, background-subtracted signal, and original pixels for saturation.
  Weights are inverse standard deviations, not inverse variances. Window masks set weights to zero.
- `createFrameModel()` applies reference-to-frame coordinates, the local Jacobian, configured component models,
  and a `DownSampledImagePsf`. Downsampling concerns internal model/PSF rendering, not the observed image grid.
- `createDeblendImage()` renders each valid neighbor into the target's rectangle, restoring that neighbor's saved
  parameters before rendering. The model renderer therefore needs to handle an arbitrary target offset/extent.

## Parameter access and uncertainty

`FlexibleModelFittingParameterManager` maps `(source, parameter definition)` to a runtime parameter. Reads also
record accesses and recursively mark dependencies. A separate index maps free runtime parameters to solver sigmas.
This access bookkeeping is mutable and has scientific meaning: the task uses it to decide whether an output was fit.

`EngineParameter` converts between physical/world values and unconstrained solver coordinates. Model dependencies
observe parameter updates. Solver engines produce marginal sigmas after converting covariance to world coordinates.
Derived-parameter propagation currently consumes marginal sigmas; see BUG-011 for shared/correlated dependencies.

## Implementation map

Paths below are relative to the repository root.

| Responsibility | Main implementation |
| --- | --- |
| Python model/parameter registration | `SEImplementation/python/sourcextractor/config/model_fitting.py` |
| Python-to-C++ definitions | `SEImplementation/src/lib/Configuration/ModelFittingConfig.cpp` |
| Task selection and configuration transfer | `SEImplementation/src/lib/Plugin/FlexibleModelFitting/FlexibleModelFittingTaskFactory.cpp` |
| Group iteration, source state, regions, weights, diagnostics | `SEImplementation/src/lib/Plugin/FlexibleModelFitting/FlexibleModelFittingIterativeTask.cpp` |
| Runtime lookup and access tracking | `SEImplementation/SEImplementation/Plugin/FlexibleModelFitting/FlexibleModelFittingParameterManager.h` |
| Parameter construction and sigma propagation | `SEImplementation/src/lib/Plugin/FlexibleModelFitting/FlexibleModelFittingParameter.cpp` |
| Component-to-frame model construction | `SEImplementation/src/lib/Plugin/FlexibleModelFitting/FlexibleModelFittingModel.cpp` |
| PSF reduction and final image placement | `SEImplementation/src/lib/Image/DownSampledImagePsf.cpp`, `ImageInterfaceTraits.cpp` |
| Solver callbacks, covariance, return status | `ModelFitting/src/lib/Engine/GSLEngine.cpp`, `LevmarEngine.cpp` |

## Findings that constrain future changes

The following behaviors were confirmed during this review; details and limitations are in the inventory.

- **Identity must survive configuration transfer.** Normalization options currently lose image IDs when appended
  to a vector, then are incorrectly retrieved by those IDs (BUG-014).
- **Saved state is not the same as live parameter state.** A skipped source can have valid runtime constants but
  zero-valued saved outputs and no failure flag (BUG-013). Solver memory failure also needs explicit handling
  before result collection (BUG-021).
- **Bookkeeping reads have side effects.** Parameter restoration, neighbor rendering, and output collection can
  all mark parameters as accessed. This invalidates the current interpretation of access as evidence of fitting
  (BUG-016). A future reset must not merely move the same contamination to another point in the cycle.
- **Geometry and masking affect scientific results.** Finite bad-variance thresholds are ignored by iterative
  weights (BUG-015), and transforming principal axis vectors independently is insufficient under shear (BUG-017).
- **Rendering has a minimum-support assumption.** Fast Lanczos-4 placement discards kernels narrower than its
  eight-sample support, although the PSF wrapper accepts those kernels (BUG-022).
- **Solver state needs an explicit handoff.** The GSL callback's last evaluation can be a derivative probe,
  not its accepted solution. The engine now restores the accepted position after every driver exit and before
  converting covariance (BUG-018). Initialization failure restores the initial guesses and throws.
  Zero-weight pixels are excluded by the shared residual provider, so neither GSL nor Levmar counts masked padding
  as additional observations in covariance scaling (BUG-019). The existing scaling convention is retained.
- **Conditional backend synchronization matters.** With `LINSOLVERS_RETAIN_MEMORY`, Levmar uses a global mutex,
  unlocks during residual callbacks, and relocks afterward. The wrapper now acquires the mutex after the
  workspace-rejection path. The conditional locking build still needs verification (BUG-020).

## Open questions and scope limits

- Is full neighbor subtraction intended for the diagnostic, although fitting uses the configurable deblend factor?
  BUG-023 records this as a candidate, not a confirmed mismatch in the scientific specification.
- Are full-resolution diagnostic/neighbor models deliberately different from the target's downsampled fitting model?
  Quantify the effect before classifying that design choice as a bug.
- Should priors count as constraints when deciding whether a parameter is fitted and whether data are sufficient?
  The current good-pixel gate runs before prior setup and counts all registered free parameters.
- Does Levmar restore accepted values on every termination path? GSL was exercised; Levmar numerical solving was not.
- Are task inputs guaranteed immutable during concurrent groups? This review did not establish a race in the
  iterative task's local state. Shared image caches and numerical backends remain separate investigation areas.

## Evidence and maintenance

The second pass used production task/model/image code in a standalone C++ harness, with synthetic source/frame
fixtures, installed GSL/FFTW, and stubs for unavailable framework plumbing and diagnostic output. The Levmar
memory-return test did not invoke a numerical Levmar implementation. See the inventory's validation section.
The BUG-018 fix now explicitly synchronizes GSL output with runtime parameters; its regression tests cover
convergence, iteration limits, and nonlinear covariance conversion. All 74 registered tests passed via `make tests`.
Update this document when another path is traced or an assumption is resolved; keep tentative interpretation visibly
separate from observed behavior and confirmed defects.

The BUG-019 fix compacts weighted residual blocks without changing image geometry. The shared provider regression
checks padding invariance in both available engines (including priors and robust asinh residuals), and provider
coverage checks alignment, refreshed models, and safe empty-block handling. The padding regression failed in both
engines before filtering was applied. `make tests` rebuilt the project and passed all 75 registered tests.
