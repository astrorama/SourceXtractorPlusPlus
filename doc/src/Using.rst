.. File Using.rst

.. include:: global.rst

.. _using-sextractor:

Using SExtractor++
==================

|SExtractor++| is run from the shell with the following syntax:

.. code-block:: console

  $ sextractor++ --detection image Image [--config-file filename] [--<option> <arg>]

The parts enclosed within brackets are optional.
Any `--<option> <arg>` statement in the command-line overrides the corresponding definition in the configuration file or any default value (see :ref:`configuration section<config_file>`).

.. toctree::

   Input
   Output
   Config

