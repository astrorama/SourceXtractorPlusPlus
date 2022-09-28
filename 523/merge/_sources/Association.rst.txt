.. File License.rst

.. include:: global.rst

.. index::
      single: association mode

Asociation Mode
===============

.. meta::
   :keywords: association mode

Similar to |SExtractor|_ there is the possibility to run |SourceXtractor++|_ in the so called **ASSOCiation mode**.
In this processing mode the user provides a catalog of target objects as input, and |SourceXtractor++| measurements
are then restricted to objects matched with those provided by the user.

The association between objects detected on the detection image and user targets is established by measuring the distance
between their coordinates.

The relevant parameters for the association mode are:

.. code-block:: console

     --assoc-catalog arg                   Path to the file containing the association objects. Plain catalogs in ASCII
                                           or FITS format are accepted.
     --assoc-columns arg (=1,2)            Column index in the association catalog  for x/ra,y/dec[,weight].
                                           The index of the first column is 1.
     --assoc-mode arg (=NEAREST)           Mode to establish the association between input catalog and the detected
                                           objects. Accepted values are [FIRST, NEAREST, MEAN,  MAG_MEAN, 
                                           SUM, MAG_SUM, MIN, MAX].
     --assoc-radius arg (=2)               Association radius. The unit is always in pixels of the detection image.
                                           
     --assoc-filter arg (=ALL)             Association filter. Accepted values are [ALL, MATCHED, UNMATCHED].
                                           
     --assoc-copy arg                      List of indices of the columns from the input catalog that will be copied to
                                           the output catalog for associated objects .The index of the first column is 1.
                                           Only columns with numerical values can be copied.
     --assoc-coord-type arg (=PIXEL)       Coordinate type for the association. Can be [PIXEL, WORLD].

Note that the association mode of |SourceXtractor++| is **not** a forced photometry mode since the objects **must** 
be detected on the detection image to establish an association and to trigger the requested measurements.
It is also worth noting that even if measurements are only done for the associated objects, these measurements
are done taking into account all the surrounding detections, not only those that are associated.


