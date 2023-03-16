.. File License.rst

.. include:: global.rst

.. index::
   single: Troubleshooting
 
Troubleshooting
===============

.. index::
   single: NO SOURCES DETECTED

NO SOURCES DETECTED
-------------------

If you unexpectedly find `NO SOURCES DETECTED` on your data, you should check your weight parameters, specifically:

.. code-block:: console

   Weight image:
     --weight-image arg                    Path to a fits format image to be used 
                                           as weight image.
     --weight-type arg (=none)             Weight image type [none|background|rms|
                                           variance|weight].
     --weight-threshold arg                Threshold for pixels to be considered 
                                           bad pixels. In same units as weight 
                                           map.
     --weight-absolute arg (=0)            Is the weight map provided as absolute 
                                           values or relative to background.
     --weight-scaling arg (=1)             Weight map scaling factor.

What can happen is that :index:`inconsistent` parameters are provided for `--weight-image`, `--weight-type`
and `--weight-threshold`. The `weight-threshold` is adjusted to the `weight-type`. This means that for the
`rms` and `variance` types one wants to mask out large values in the `weight-image` by providing a large `weight-threshold`.
but for the `weight` type the low values should be masked out with a small `weight-threshold`. Mixing this up
will result in all pixels in the detection image being masked out and no sources detected.

With `weight-scaling` one provides a scaling factor for the entire weight image. For instance `--weight-scaling=0`
masks out all pixels in the detection image and an inappropriate value may lower the SNR in the detection image to the point
that no sources can be found.

The safest way to find objetcs is indeed using `--weight-type=background` which determines the
noise amplitude and therefore the entire SNR for the detection directly from the detection image. With a `--weight-image`
and the correct `--weight-type` (the defaults for `--weight-type` are set to mask out nothing),
`--weight-absolute arg=0` switches on automatic scaling of the `weight-image` with the median rms determined from the detection
image, which usually brings the SNR for detection into a reliable range for detecting objects.


.. index::
   single: temporary disk space

Problems with temporary disk space
----------------------------------

|SourceXtractor++| attempts to store all intermediate products (images and so on) in memory.
If not enough memory is available or provided to |SourceXtractor++| (with the `tile-memory-limit` option),
disk space is put to contribution through temporary files. Depending on the situation (data size, available disk space)
there could be no space left for these temporary files, resulting in errors such as:

.. code-block:: console

   2022-08-18T14:15:48CEST SourceXtractor ERROR : Failed to process the frame! 
   Couldn't allocate space for new FITS file: /tmp/80122.1.euclid.test/_tmp_ml_segd1f9bb.fits status: 106

The solution is to extend the available temporary disk space. This can be done by extending the corresponding partition
or directing the operating system to use a partition with sufficient disk space with:

.. code-block:: console

   mkdir $HOME/tmp
   export TMPDIR=$HOME/tmp

Note that right now |SourceXtractor++| does **not** have a parameter to directly specify the directory to store
temporary data.


.. index::
   single: sourcextractor++ stops suddenly

|SourceXtractor++| stops shortly after the first measurements are done
----------------------------------------------------------------------

It might happen that |SourceXtractor++| stops or dumps off when the first measurements are due or when the first few measurements have been done.
The log file then ends similar to this example:

.. code-block:: console

	2023-02-09T16:06:50CET Progress  INFO : Segmentation: 487 / 7200 (6.76%)
	2023-02-09T16:06:50CET Progress  INFO : Detected: 1200
	2023-02-09T16:06:50CET Progress  INFO : Deblended: 0
	2023-02-09T16:06:50CET Progress  INFO : Measured: 0
	2023-02-09T16:06:50CET Progress  INFO : Elapsed: 00:01:36
	2023-02-09T16:06:58CET Progress  INFO : Segmentation: 502 / 7200 (6.97%)
	2023-02-09T16:06:58CET Progress  INFO : Detected: 1234
	2023-02-09T16:06:58CET Progress  INFO : Deblended: 1
	2023-02-09T16:06:58CET Progress  INFO : Measured: 1 / 1 (100.00%)
	2023-02-09T16:06:58CET Progress  INFO : Elapsed: 00:01:43
	2023-02-09T16:07:04CET Progress  INFO : Segmentation: 502 / 7200 (6.97%)
	2023-02-09T16:07:04CET Progress  INFO : Detected: 1235
	2023-02-09T16:07:04CET Progress  INFO : Deblended: 4
	2023-02-09T16:07:04CET Progress  INFO : Measured: 4 / 4 (100.00%)
	2023-02-09T16:07:04CET Progress  INFO : Elapsed: 00:01:50
	2023-02-09T16:07:09CET Progress  INFO : Segmentation: 504 / 7200 (7.00%)
	2023-02-09T16:07:09CET Progress  INFO : Detected: 1240
	2023-02-09T16:07:09CET Progress  INFO : Deblended: 7
	2023-02-09T16:07:09CET Progress  INFO : Measured: 7 / 7 (100.00%)
	2023-02-09T16:07:09CET Progress  INFO : Elapsed: 00:01:55

A possible cause could be a race condition created  by the concurrent calls to lower level libraries such as openMP or openBLAS.
This can happen in already multi-threaded applications such as |SourceXtractor++|
(see `here <https://github.com/xianyi/OpenBLAS/wiki/faq#multi-threaded>`_ for details). 

A confirmation that the dump is caused by a race condition in lower level libraries is if |SourceXtractor++| runs with multi-threading
switched off (`--thread-count=0`), meaning that |SourceXtractor++| does not dump at least beyound a couple of dozen measurements in.

A solution is to switch off the multi-threading of all lower level libraries by setting the following environmental variables
*before the call* to |SourceXtractor++|:

.. code-block:: console

	export MKL_NUM_THREADS=1
	export NUMEXPR_NUM_THREADS=1
	export OMP_NUM_THREADS=1
	export OPENBLAS_NUM_THREADS=1
	export VECLIB_MAXIMUM_THREADS=1
	export NTHREADS=1
	sourcextractor++ --conf ...

This switches off the multi-threading in the lower level libraries and should prevent the race conditions. In version 0.20 we
are trying to make these settings inside |SourceXtractor++|
(see `here <https://gitlab.euclid-sgs.uk/EuclidLibs/SourceXtractorPlusPlus/-/blob/develop/SEMain/src/program/SourceXtractor.cpp#L136>`_),
however we are not sure whether this will work in all situations and modes.
