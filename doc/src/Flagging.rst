.. File Flagging.rst

.. include:: global.rst

.. _chap_flagging:

Flagging
========

Flags are binary attributes of the detected sources.
They might be set to indicate, e.g., that a source is saturated or that it has been deblended.
Flags are grouped in columns in the |SExtractor++| output catalog.
Each column element is an integer, comprising several flag bits as a sum of powers of 2.
Many properties produce flag columns:
AperturePhotometry (:param:`aperture_flags`), AutoPhotometry (:param:`auto_flags`), FlexibleModelFitting (:param:`fmf_flags`), ...
For example, the :param:`source_flags` column is produced by the SourceFlags property; a saturated detection close to an image boundary will have :param:`source_flags` = 4+8 = 12 in decimal.


.. _flags_def:

Extraction flags
----------------

* :ref:`External flags <imaflags_iso_def>` are derived from *flag maps*. Flag maps are images in integer format having the same dimensions as the science images, with pixel values that can be used to flag some pixels (for instance, "bad" or noisy pixels). Different combinations of flags can be applied within the isophotal area that defines each object, to produce a unique value that will be written to the catalog.



.. _imaflags_iso_def:

External flags
--------------


