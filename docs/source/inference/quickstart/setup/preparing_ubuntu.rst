.. include:: ../../links.rst

================
Preparing Ubuntu
================

-----------------
Tool Installation
-----------------

To install all tools

.. code-block:: bash

    sudo apt-get -y install git gcc g++ cmake uuid-dev libssl-dev

------------
CUDA Toolkit
------------

To enable NVIDIA GPU Support, you need the CUDA toolkit installed which can be done by following:

.. code-block:: bash

    sudo apt-get -y install nvidia-cuda-toolkit

After you download and install all those softwares, make sure they are properly set.
Next, we will build the MBASE library.