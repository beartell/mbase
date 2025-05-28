.. include:: ../inference/links.rst

=====================
Compiling From Source
=====================

You can compile the MBASE as a bundle compilation with `llama.cpp <llama.cpp_>`_ or standalone compile it.

The difference between two is that unlike the standalone compilation, bundled one doesn't require `llama.cpp <llama.cpp_>`_ to be present on your system
since it compiles `llama.cpp <llama.cpp_>`_ with the MBASE library.

Fortunately, you don't need to do any configuration on your part, MBASE will detect if you have a `llama.cpp <llama.cpp_>`_ in your environment
and link the necessary libraries accordingly but for the bundled compilation, you need to initialize the submodule.

-------------------------
Preparing the Environment
-------------------------

Here is a list of requirements that your system must satisfy:

* GCC 7.0 or later, Clang 5.0 or later, MSVC 15.3 or later, with support for at least C++17.
* A 64-bit operating system.
* CMake build tool with version 3.15 or later must be installed.
* Git version control software must be installed.

Here is a list of tools and libraries you need:

* CUDA Toolkit if NVIDIA GPU inference is desired.
* OpenSSL library to enable SSL support.
* `MBASE <mbase_main_>`_
* `llama.cpp <llama.cpp_>`_

Do not worry if they are not installed and set, that is what this chapter for.

^^^^^^^
Windows
^^^^^^^

- For MSVC C++ build tools, make sure to have a Visual Studio in your environment. You can download it `here <https://visualstudio.microsoft.com/vs/features/cplusplus/>`_ .
- For CMake, you can download it from `here <https://cmake.org/download/>`_.
- For Git, you can download it from `here <https://git-scm.com/downloads/win>`_.
- For CUDA Toolkit, you can download it from `here <https://developer.nvidia.com/cuda-downloads>`_.

After you download and install all those softwares, make sure they are properly set.

^^^^^^
Ubuntu
^^^^^^

To install all tools

.. code-block:: bash

    sudo apt-get -y install git gcc g++ cmake uuid-dev libssl-dev

To enable NVIDIA GPU Support, you need the CUDA toolkit installed which can be done by following(CUDA 12.8):

.. code-block:: bash

    wget https://developer.download.nvidia.com/compute/cuda/repos/ubuntu2204/x86_64/cuda-keyring_1.1-1_all.deb
    sudo dpkg -i cuda-keyring_1.1-1_all.deb
    sudo apt-get update
    sudo apt-get -y install cuda-toolkit-12-8

Instructions are obtained from NVIDIA CUDA download guide: https://developer.nvidia.com/cuda-downloads?target_os=Linux&target_arch=x86_64&Distribution=Ubuntu&target_version=22.04&target_type=deb_network

After you download and install all those softwares, make sure they are properly set.

^^^^^
MacOS
^^^^^

- Make sure you have the XCode and its command line tools are installed. You can install it from `Mac App Store <https://apps.apple.com/app/xcode/id497799835>`_.

To install cmake:

.. code-block:: bash
    
    brew install cmake

To install git:

.. code-block:: bash

    brew install git

After you download and install all those softwares, make sure they are properly set.

-------------------
MBASE CMake Options
-------------------

There are some common CMake options worth mentioning. They can be specified at the cmake configuration stage.

:code:`MBASE_SERVER_SSL`: If set, openai server program will be with SSL support. However, if you set this up, make sure you have the :code:`openssl` installed in your environment. (default=OFF)

:code:`MBASE_FORCE_BUNDLE`: It set, the compilation will compile the `llama.cpp <llama.cpp_>`_ with MBASE even if you have `llama.cpp <llama.cpp_>`_ installed in your environment. (default=OFF)

:code:`MBASE_MCP_SSL`: If set, MCP HTTPS Client/Server will be enabled.

:code:`GGML_CUDA`: `llama.cpp <llama.cpp_>`_ library option. If set, cuda compilation will be enabled.

-------------------
Bundled Compilation
-------------------

.. important::

    `llama.cpp <llama.cpp_>`_ supports multiple backends for inference.
    This documentation only mentions the normal CUDA GPU, x86_64, and Apple Metal backend builds.
    In order to build the llama.cpp library with the backend you desire or detailed configuration and for more information in general, refer to `llama.cpp <llama.cpp_>`_ repository.


.. important::

    If you have a `llama.cpp <llama.cpp_>`_ in your environment, you should do the standalone compile.

First, you need to clone the `MBASE <mbase_main_>`_ repository:

.. code-block:: bash

    git clone https://github.com/Emreerdog/mbase
    cd mbase

Then, we will initialize the `llama.cpp <llama.cpp_>`_ submodule:

.. code-block:: bash

    git submodule update --init --recursive

Which will clone the latest `llama.cpp <llama.cpp_>`_ repository from github.

After we clone the `llama.cpp <llama.cpp_>`_ repository we will create the build directory
and start compiling the project. However, since the `llama.cpp <llama.cpp_>`_ library will
be compiled with `MBASE <mbase_main_>`_, you should look at the
`llama.cpp <llama.cpp_>`_ cmake build flags in the original repository for detailed
build configuration since they both apply to MBASE and `llama.cpp <llama.cpp_>`_. 

^^^^^^^^^^^^
x86_64 BUILD
^^^^^^^^^^^^

In order to build x86_64 backend, do the following:

.. code-block:: bash

    cmake -B build_x86 -DMBASE_SERVER_SSL=ON -DMBASE_FORCE_BUNDLE=ON
    cmake --build build_x86 --config Release -j

The compilation may take some time so do not worry.

^^^^^^^^^^^^^^^^^
Apple Metal BUILD
^^^^^^^^^^^^^^^^^

In order to build Apple Metal backend, do the following:

.. code-block:: bash

    cmake -B build_metal -DGGML_METAL=ON -DGGML_METAL_USE_BF16=ON -DGGML_METAL_EMBED_LIBRARY=ON -DMBASE_SERVER_SSL=ON -DMBASE_FORCE_BUNDLE=ON
    cmake --build build_metal --config Release -j

^^^^^^^^^^^^^^
CUDA GPU BUILD
^^^^^^^^^^^^^^

NVIDIA GPUs have different CUDA compute capabilities which can be seen `here <https://developer.nvidia.com/cuda-gpus>`_. 
User must set the :code:`CMAKE_CUDA_ARCHITECTURES` option accordingly to what GPUs user wants the compile the MBASE to. In our case, we simply will compile for the native GPU we have.

Now, in order to compile with CUDA you should specify the :code:`CMAKE_CUDA_ARCHITECTURES` as native and set the :code:`GGML_CUDA` option to on:

.. code-block:: bash

   cmake -B build_gpu -DGGML_CUDA=0N -DCMAKE_CUDA_ARCHITECTURES="native" -DMBASE_SERVER_SSL=ON -DMBASE_FORCE_BUNDLE=ON
   cmake --build build_gpu --config Release -j

The compilation may take some time so do not worry.

----------------------
Standalone Compilation
----------------------

In order to build the MBASE, do the following:

.. code-block:: bash

    cmake -B build -DMBASE_SERVER_SSL=ON
    cmake --build build --config Release -j

The compilation may take some time so do not worry.

-------------
Local Install
-------------

After the compilation is complete, run (in Linux, macOS):

.. code-block:: bash

   sudo cmake --install build

In order to install it in windows, your terminal (cmd or powershell) must be run with administrator privileges. Run (in Windows):

.. code-block:: bash

   cmake --install build

--------------------------
Linux, macOS Install Paths
--------------------------

MBASE will be installed to given paths in Linux and macOS:

* Library files: **/usr/local/lib**
* CMake config modules: **/usr/local/lib/cmake/mbase.libs**
* MBASE header files: **/usr/local/include**
* Binary files: **/usr/local/bin**

---------------------
Windows Install Paths
---------------------

MBASE will be installed to given paths in Windows:

* Library files (Both static libs and import libs): **C:/Program Files (x86)/mbase.libs/lib**
* CMake config modules: **C:/Program Files (x86)/mbase.libs/lib/cmake/mbase.libs**
* MBASE header files:: **C:/Program Files (x86)/mbase.libs/include**
* Binary files (Dlls and executables): **C:/Program Files (x86)/mbase.libs/bin**
