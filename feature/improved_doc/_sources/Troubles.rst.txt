.. File License.rst

.. include:: global.rst

Troubleshooting
===============

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

What can happen is that inconsistent parameters are provided for `--weight-image`, `--weight-type`
and `--weight-threshold`. The `weight-threshold` is adjusted to the `weight-type` which means for the types
`rms` and `variance` you want to block large values in the `weight-image` by providing a large `weight-threshold`,
but for the type `weight` the idea is to block the low values with a small `weight-threshold`. If you mix this up
all pixels in the detection image might be masked and you end up with no sources.

With `weight-scaling` you provide a scaling factor for the entire weight image, which means `--weight-scaling=0`
blocks all pixels in the detection image and an unfavourable value may lower the SNR in the detection image such that
no sources can be found.

The safest way to find objetcs is indeed using `--weight-type=background` which determines the
entire SNR for the detection directly from the detection image. With a `--weight-image` and the correct
`--weight-type` (the defaults for `--weight-type` are set to block nothing) the `--weight-absolute arg=0`
switches on the automatic scaling of the `weight-image` with the median rms determined from the detection
image, which usually brings the SNR for detection into a reliable range for detecting objects.


Problems with temporary disk space
----------------------------------

|SourceXtractor++| attempts to store all intermediate products (images and so on) in RAM.
If there is not enough RAM or not enough RAM was offered to |SourceXtractor++| (parameter `tile-memory-limit`),
the temporary disk space of the operating system is being used. Depending on the situation (data size, available disk space)
there could be not enough space on the temporary disk, which results in an error such as:

.. code-block:: console

   2022-08-18T14:15:48CEST SourceXtractor ERROR : Failed to process the frame! 
   Couldn't allocate space for new FITS file: /tmp/80122.1.euclid.test/_tmp_ml_segd1f9bb.fits status: 106

The solution is to extend the available temporary disk space. This can be done by extending the corresponding partition
or directing the temporary disk to a partition with suffucient disk space with:

.. code-block:: console

   mkdir $HOME/tmp
   export TMPDIR=$HOME/tmp

Note that right now |SourceXtractor++| does **not** have a parameter to directly specify the directory to store
temporary data!
 