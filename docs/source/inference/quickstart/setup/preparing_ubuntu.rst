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

To enable NVIDIA GPU Support, you need the CUDA toolkit installed which can be done by following(CUDA 12.8):

.. code-block:: bash

    wget https://developer.download.nvidia.com/compute/cuda/repos/ubuntu2204/x86_64/cuda-keyring_1.1-1_all.deb
    sudo dpkg -i cuda-keyring_1.1-1_all.deb
    sudo apt-get update
    sudo apt-get -y install cuda-toolkit-12-8

Instructions are obtained from NVIDIA CUDA download guide: https://developer.nvidia.com/cuda-downloads?target_os=Linux&target_arch=x86_64&Distribution=Ubuntu&target_version=22.04&target_type=deb_network

After you download and install all those softwares, make sure they are properly set.
Next, we will build the MBASE library.
