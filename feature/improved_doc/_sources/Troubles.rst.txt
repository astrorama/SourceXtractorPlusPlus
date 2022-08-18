.. File License.rst

.. include:: global.rst

Troubleshooting
===============

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
 