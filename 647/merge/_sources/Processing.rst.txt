.. File Processing.rst

.. include:: global.rst

.. _processing:

Processing
==========

The complete analysis of an image is fully automated (:numref:`fig_flowchart`).
There are three main steps in the processing: detection, collection and measurement:

* During the detection step, image pixels from the detection image are background-subtracted, filtered and segmented on-the-fly.
  The extracted source candidates are then deblended.

* During the collection step, a source model is fit to the detection image.
  Overlapping sources are identified, grouped and pruned ("cleaned").

* During the measurement step, sources are analysed individually or as part of their group.
  Measurements are written to the output catalog.

.. _fig_flowchart:

.. figure:: figures/flowchart.*
   :align: center
   :figwidth: 100%

   Layout of the main |SourceXtractor++| procedures. *Dashed arrows* represent
   optional inputs. 

The following sections describe each of these operations in more detail.

.. toctree::

   Background
   Weighting
   Flagging

