.. File License.rst

.. include:: global.rst

.. index::
      single: test data

Test Data Set
===============

.. meta::
   :keywords: test data


A test data set for |SourceXtractor++| covering various data reduction scenarios was developed for the `ADASS XXXII Conference <https://www.adass2022.ca/>`_. 
The proceedings contribution is available as a `pre-print <http://arxiv.org/abs/2212.02428>`_.
The test data set includes the input data, all relevant configuration files, the |SourceXtractor++| commands and the expected output results for a reference
and is available for  download `here <https://cloud.physik.lmu.de/index.php/s/3K4KemBsw5y9yqd>`_. The data set is organized in a series of exercises
that run with  |SourceXtractor++| `v0.19 <https://github.com/astrorama/SourceXtractorPlusPlus/releases/tag/0.19>`_ (except for the last exercise).

01 The basics
-------------

* Making the connection to |SExtractor|_;
* Object detection;
* Configuration file(s);
* Object properties;
* Simple fitting (Sersic model, one image, one band);

02 Multi-band
-------------

* The bread-and-butter scenario (running on multi-band images);
* Three bands, three different WCS;
* Creating and processing in groups;
* Dependent parameters;
* User defined output columns;

03 Dependent parameters and priors
----------------------------------

* Connect data and measurements;
* Add knowledge on your data;
* Iterate and refine your processing;
* see `Quilley & de Lapparent <https://www.aanda.org/articles/aa/pdf/2022/10/aa44202-22.pdf>`_;

04 User defined functions
-------------------------

* What’s beyond Sersic and Disk+Bulge and Pointlike;
* With onnx towards galfit!

05 Working on cubes
-------------------

* Exploring the third dimension!

06 Extended associations
------------------------

* Funnel external knowledge into your measurements;
* Towards disentangling detection and measurements!

07 The future: Python control
-----------------------------

* available in the devel branch `experimental/call_from_python <https://github.com/astrorama/SourceXtractorPlusPlus/tree/experimental/call_from_python>`_;
* Run |SourceXtractor++| from python;
* Direct interaction with the objects and results;
* Run your own software as part of the |SourceXtractor++| processing;
