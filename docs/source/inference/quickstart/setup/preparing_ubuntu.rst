.. include:: ../../links.rst

================
Preparing Ubuntu
================

-----------------
Tool Installation
-----------------

To install gcc:

.. code-block:: bash

    sudo apt install git gcc g++

To install cmake:

.. code-block:: bash

    sudo apt install git cmake

To install git:

.. code-block:: bash

    sudo apt install git

To install uuid:

.. code-block:: bash

    sudo apt install uuid-dev

------------
CUDA Toolkit
------------

To enable NVIDIA GPU Support, you need the CUDA toolkit installed which can be done by following:

.. code-block:: bash

    sudo apt install nvidia-cuda-toolkit

After you download and install all those softwares, make sure they are properly set.
Next, we will build the MBASE library.