.. File Model.rst

.. include:: global.rst

Model fitting
=============

Fitting procedure
-----------------

SExtractor++ can fit models to the images of detected objects. The fit is performed by minimizing the loss function

.. math::
  :label: loss_func

  \lambda(\boldsymbol{q}) = \sum_i \left(g\left(\frac{p_i - \tilde{m}_i(\boldsymbol{q})}{\sigma_i}\right)\right)^2 + \sum_j \left(\frac{q_j - \mu_j}{s_j}\right)^2

with respect to components of the model parameter vector :math:`\boldsymbol{q}`. :math:`\boldsymbol{q}` comprises parameters describing the shape of the model and the model pixel coordinates :math:`\boldsymbol{x}`.

Modified least squares
~~~~~~~~~~~~~~~~~~~~~~

The first term in :eq:`loss_func` is a modified `weighted sum of squares <http://en.wikipedia.org/wiki/Least_squares#Weighted_least_squares>`_ that aims at minimizing the residuals of the fit. :math:`p_i`, :math:`\tilde{m}_i(\boldsymbol{q})` and :math:`\sigma_i` are respectively the pixel value above the background, the value of the resampled model, and the pixel value uncertainty at image pixel :math:`i`.
:math:`g(u)` is a derivable monotonous function that reduces the influence of large deviations from the model, such as the contamination by neighbors (:numref:`fig_robustgalfit`):

.. math::
  :label: modified_lsq

  g(u) = u_0\,\mathrm{arsinh}\,\frac{u}{u_0}.

:math:`u_0` sets the level below which :math:`g(u)\approx u`.
In practice, choosing :math:`u_0 = \kappa \sigma_i` with :math:`\kappa = 10` makes the first term in :eq:`loss_func` behave like a traditional weighted sum of squares for residuals close to the noise level.

.. caution::
  The cost function in :eq:`loss_func` is optimized for noise distributions with a Gaussian core and makes model-fitting in |SExtractor++| appropriate only for image noise with a |pdf| symmetrical around the mean.

.. _fig_robustgalfit:

.. figure:: figures/robustgalfit.*
   :figwidth: 100%
   :align: center

   Effect of the modified least squares loss function on fitting a model to a galaxy with a bright neighbor. *Left*: the original image; *Middle*: residuals of the model fitting with a regular least squares (:math:`\kappa = +\infty`); *Right*: modified least squares with :math:`\kappa = 10`.

The vector :math:`\tilde{\boldsymbol{m}}(\boldsymbol{q})` is obtained by convolving the high resolution model :math:`\boldsymbol{m}(\boldsymbol{q})` with the local PSF model :math:`\boldsymbol{\phi}` and applying a resampling operator :math:`\mathbf{R}(\boldsymbol{x})` to generate the final model raster at position :math:`\boldsymbol{x}` at the nominal image resolution:

.. math::
  :label: model_convolution

  \tilde{\boldsymbol{m}}(\boldsymbol{q}) = \mathbf{R}(\boldsymbol{x}) (\boldsymbol{m}(\boldsymbol{q})*\boldsymbol{\phi}).

:math:`\mathbf{R}(\boldsymbol{x})` depends on the pixel coordinates :math:`\boldsymbol{x}` of the model centroid:

.. math::
  :label: model_resampling

  \mathbf{R}_{ij}(\boldsymbol{x}) =  h\left(\boldsymbol{x}_j - \eta.(\boldsymbol{x}_i - \boldsymbol{x})\right),

where :math:`h` is a 2-dimensional interpolant (interpolating function), :math:`\boldsymbol{x}_i` is the coordinate vector of image pixel :math:`i`, :math:`\boldsymbol{x}_j` the coordinate vector of model sample :math:`j`, and :math:`\eta` is the image-to-model sampling step ratio (sampling factor) which is by default defined by the PSF model sampling.
We adopt a Lánczos-4 function :cite:`duchon1979` as interpolant.

Change of variables
~~~~~~~~~~~~~~~~~~~

Many model parameters are valid only over a restricted domain.
Fluxes, for instance, cannot be negative. 
In order to avoid invalid values and also to facilitate convergence, a change of variables is applied individually to each model parameter:

.. math::
  :label: change_of_variables

  q_j = f_j(a_j, b_j, Q_j).

The "model" variable :math:`q_j` is bounded by the lower limit :math:`a_j` and the upper limit :math:`b_j` by construction.
The "engine" variable :math:`Q_j` can take any value, and is actually the parameter that is being adjusted in the fit, although it does not have any physical meaning.

In |SExtractor++| three different types of transforms :math:`f_j()` can be applied, depending on the parameter (:numref:`change_of_variable_table`).

.. _change_of_variable_table:

.. list-table:: Types of changes of variables applied to model parameters
  :header-rows: 1

  * - Type
    - Model :math:`\stackrel{f^{-1}}{\to}` Engine
    - Engine :math:`\stackrel{f}{\to}` Model
    - Examples
  * - Unbounded (linear)
    - :math:`Q_j = q_j`
    - :math:`q_j = Q_j`
    - | Position angles
      | 
  * - :param:`RangeType.LINEAR`
    - :math:`Q_j = \ln \frac{q_j - a_j}{b_j - q_j}`
    - :math:`q_j = \frac{b_j - a_j}{1 + \exp -Q_j} + a_j`
    - | positions
      | Sersic index
  * - :param:`RangeType.EXPONENTIAL`
    - :math:`Q_j = \ln \frac{\ln q_j - \ln a_j}{\ln b_j - \ln q_j}`
    - :math:`q_j = a_j \frac{\ln b_j - \ln a_j}{1 + \exp -Q_j}`
    - | fluxes
      | aspect ratios

In practice, this approach works well, and was found to be much more reliable than a box constrained algorithm :cite:`Kanzow2004375`.

Regularization
~~~~~~~~~~~~~~

Although minimizing the (modified) weighted sum of least squares gives a solution that fits best the data, it does not necessarily correspond to the most probable solution given what we know about celestial objects.
The discrepancy is particularly significant in very faint (|SNR| :math:`\le 20`) and barely resolved galaxies.
For instance, there is a tendency to overestimate the elongation of such galaxies, known as the "noise bias" in the weak-lensing community :cite:`2004MNRAS_353_529H,2012MNRAS_424_2757M,2012MNRAS_425_1951R,2012MNRAS_427_2711K`.
The second term in :eq:`loss_func` implements a simple `Tikhonov regularization <https://en.wikipedia.org/wiki/Tikhonov_regularization>`_ scheme to mitigate this issue.
This penalty term acts as a Gaussian prior on the selected model parameters.
Sometimes a Gaussian prior is unsuited to some parameters (e.g., to disfavor excessively low or high values).
In that case a change of variable must be applied (:numref:`fig_aspectprior`).

.. _fig_aspectprior:

.. figure:: figures/aspectprior.*
   :figwidth: 100%
   :align: center

   Effect of the Gaussian prior on a model parameter controlling aspect ratio. *Left:* change of variables between the model (in abscissa) and the engine (in ordinate) parameters. *Right*: equivalent (improper) prior applied to the aspect ratio for :math:`\mu_{\rm aspect} = 0` and :math:`s_{\rm aspect} = 1` in equation :eq:`loss_func`.

.. _model_minimization_def:

Minimization
~~~~~~~~~~~~

Minimization of the loss function :math:`\lambda(\boldsymbol{q})` is carried out using the `Levenberg-Marquardt algorithm <http://en.wikipedia.org/wiki/Levenberg%E2%80%93Marquardt_algorithm>`_, and more specifically the |LevMar|_ implementation :cite:`lourakis04LM`.
The library approximates the Jacobian matrix of the model from finite differences using Broyden's :cite:`Broyden1965ACo` rank one updates.
The fit is done inside a disk which diameter is scaled to include the isophotal footprint of the object, plus the FWHM of the PSF, plus a 20 % margin.

:math:`1\,\sigma` error estimates are provided for most measurement parameters; they are obtained from the full covariance matrix of the fit, which is itself computed by inverting the approximate `Hessian matrix <https://en.wikipedia.org/wiki/Hessian_matrix>`_ of :math:`\lambda(\boldsymbol{q})` at the solution.

.. _models_def:

Models
------

.. _spread_model_def:

Model-based star-galaxy separation
----------------------------------


