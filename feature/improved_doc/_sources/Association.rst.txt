.. File License.rst

.. include:: global.rst

Asociation Mode
===============

.. meta::
   :keywords: association mode

Similar to |SExtractor|_ there is the possibility to run |SourceXtractor++|_ in the so called **association mode**.
In this processing mode of |SourceXtractor++| the user can provide a target object list as input, and the
|SourceXtractor++| measurements are then restricted to objects which are **associated** to the objects provided by
the user.

The association between the input objects and the objects detected on the detection image is established by measuring the
distance between their positions.

Relevant parameters for the association mode and their meaning are:

.. code-block:: console

     --assoc-catalog arg                   Pathe to file with the association objects. Plain catalogs in ACII
                                           and FITS fortmat are accepted.
     --assoc-columns arg (=1,2)            Column index in the association catalog  for x/ra,y/dec[,weight].
                                           The index of the  first column is 1.
     --assoc-mode arg (=NEAREST)           The mode to establish the association between input catalog and the 
                                           detected objects. Accepted values are [FIRST, NEAREST, MEAN,  MAG_MEAN, 
                                           SUM, MAG_SUM, MIN, MAX]
     --assoc-radius arg (=2)               The radius to establish the association. The unit is always in pixels
                                           of the detection image
     --assoc-filter arg (=ALL)             The filter for the associations. Accepted values are [ALL, MATCHED, 
                                           UNMATCHED].
     --assoc-copy arg                      List of columns indices in the assoc catalog to copy to the output
                                           for associated objects .The index of the first column is 1. Only
                                           columns with numerical values can be copied.
     --assoc-coord-type arg (=PIXEL)       The oordinates type for the association. Can be [PIXEL, WORLD].

Note that the association mode of |SourceXtractor++| is **not** a forced photometry mode since the objects **must** 
be detected on the detection image to establish an association and to trigger the requested measurements.
It is also worth noting that even if the measurements are only done for the associated objects, these measurements
are done taking into account all surrounding detected but not associated ojects.


