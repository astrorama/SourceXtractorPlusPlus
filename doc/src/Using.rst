.. File Using.rst

.. include:: global.rst

.. _using-sextractor:

Using SourceXtractor++
==================

|SourceXtractor++| is run from the shell with the following syntax:

.. code-block:: console

  $ sextractor++ --detection image Image [--config-file filename] [--<option> <arg>]

The parts enclosed within brackets are optional.
Any ``--<option> <arg>`` or ``--<option>=<arg>`` statement in the command-line overrides the corresponding definition in the :ref:`configuration file <chap_config_file>` or any default value (see :ref:`configuration section <chap_config>`).

.. toctree::

   Input
   Output
   Config

