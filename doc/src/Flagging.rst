.. File Flagging.rst

.. include:: global.rst

.. _chap_flagging:

Flagging
========

Both *internal* and *external* flags are available for each detection as catalog parameters.

* :ref:`External flags <imaflags_iso_def>` are derived from *flag maps*. Flag maps are images in integer format having the same dimensions as the science images, with pixel values that can be used to flag some pixels (for instance, "bad" or noisy pixels). Different combinations of flags can be applied within the isophotal area that defines each object, to produce a unique value that will be written to the catalog.

Each catalog parameter comprises several flag bits as a sum of powers of 2 coded in decimal. For example, a saturated detection close to an image boundary will have :param:`FLAGS` = 4+8 = 12 (see below).

.. _flags_def:

Extraction flags
----------------


.. _imaflags_iso_def:

External flags
--------------


