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
 
