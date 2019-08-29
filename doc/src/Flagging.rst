.. File Flagging.rst

.. include:: global.rst

.. _chap_flagging:

Flagging
========

Flags are binary attributes of the detected sources.
They are set to indicate, e.g., that a source is saturated or that it has been deblended.
Flags are grouped in columns in the |SExtractor++| output catalog.
Each column element is an integer, comprising several flag bits as a sum of powers of 2.
Many properties produce flag columns:
AperturePhotometry (:param:`aperture_flags`), AutoPhotometry (:param:`auto_flags`), FlexibleModelFitting (:param:`fmf_flags`), ...
For example, the :param:`source_flags` column is produced by the SourceFlags property; a saturated detection close to an image boundary will have :param:`source_flags` = 4+8 = 12 in decimal.

.. _isoflags_def:

External flags
--------------

External flags  are derived from :ref:`chap_inputflagmaps`. Flag maps are images in (unsigned) integer format having the same size as the detection image, where each integer represents a set of flag bits as a sum of powers of 2.

Flag images are specified as arguments to the ``--flag-image-*`` option(s), where the ``*`` represents an arbitrary character or string; for instance: ``--flag-image-cosmic cosmic.flag.fits``.

Different combinations of flags can be applied within the isophotal footprint that defines objects, to produce a unique integer that will be written to the catalog.
How the flags are combined within the isophotal footprint can bet set with the ``--flag-type-*`` option(s).
Valid ``flag-type`` values are:

* ``or`` :
  An arithmetic (bitwise) OR between pixels is applied to all flags independently.
  For instance if a pixel is set to :math:`9 = 1001_2` and another to :math:`5 = 0101_2`, then the result will be :math:`1001_2 \vee 0101_2 = 1101_2 = 13`

* ``and`` :
  An arithmetic AND between pixels is applied to all flags independently.
  For instance with inputs from the previous example the result will be :math:`1001_2 \wedge 0101_2 = 0001_2 = 1`

* ``min`` :
  The result is the minimum value of the flag combination within the isophotal footprint.
  For instance with inputs from the previous example the result will be :math:`\min(9, 5) = 5`

* ``max`` :
  The result is the maximum value of the flag combination within the isophotal footprint.
  For instance with inputs from the previous example the result will be :math:`\max(9, 5) = 9`

* ``most``:
  The result is the (non-zero) flag combination that is most represented within the isophotal footprint.
  For instance if a pixel is set to :math:`9` and two other pixels are set to :math:`5`, then the result will be :math:`5`.


