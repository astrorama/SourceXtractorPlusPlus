.. File Input.rst

.. include:: global.rst

.. _input:

Input files
-----------

|SExtractor| accepts images stored in |FITS|_ :cite:`1981AAS_44_363W`.
Both "Basic FITS" (one single header and one single body) and |MEF|_
files are recognized.
Binary |SExtractor| catalogs produced from |MEF| images are |MEF| files themselves.
If the catalog output format is set to ASCII, all catalogs from the individual extensions are concatenated in one big file; the :param:`EXT_NUMBER` catalog parameter can be used to tell which extension the detection belongs to.

In |SExtractor|, as in all similar programs, |FITS| axis #1 is traditionally
referred to as the *x* axis, and |FITS| axis #2 as the *y* axis.

