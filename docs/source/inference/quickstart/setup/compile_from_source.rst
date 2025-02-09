.. include:: ../../links.rst

=====================
Compiling From Source
=====================

.. important::

   Make sure you set the requirements up for development :doc:`requirements`

First, you need to clone the `MBASE <mbase_main_>`_ repository:

.. code-block:: bash

    git clone https://github.com/Emreerdog/mbase

Then, we will initialize the `llama.cpp <llama.cpp_>`_ submodule:

.. code-block:: bash

    git submodule init
    git submodule update --remote

Which will clone the latest `llama.cpp <llama.cpp_>`_ repository from github.
After we clone the `llama.cpp <llama.cpp_>`_ repository we will create the build directory
and start compiling the project. However, since the `llama.cpp <llama.cpp_>`_ library will
be compiled with `MBASE <mbase_main_>`_, you should look at the
`llama.cpp <llama.cpp_>`_ cmake build flags in the original repository for detailed
build configuration. This documentation will include build configuration only for CUDA and
x86_64 builds.

.. important::

    llama.cpp supports multiple backends for inference.
    This documentation only mentions the normal x86_64 and CUDA GPU backend builds.
    In order to build the llama.cpp library with the backend you desire or detailed configuration and for more information in general, refer to `llama.cpp <llama.cpp_>`_ repository.

------------
x86_64 BUILD
------------

In order to build x86_64 backend, do the following:

.. code-block:: bash

    cd llama.cpp
    mkdir build_cpu
    cd build_cpu
    cmake ..
    cmake --build . --config Release -j

The compilation may take some time so do not worry.

--------------
CUDA GPU BUILD
--------------

.. important::
    Before you attempt to build CUDA backend, make sure you have cuda toolkit installed in your environment.

    * :doc:`For Windows<preparing_windows>`

    * :doc:`For Ubuntu<preparing_ubuntu>`

In order to build CUDA GPU backend, do the following:

.. code-block:: bash

   cd llama.cpp
   mkdir build_gpu
   cd build_gpu
   cmake -DGGML_CUDA=0N ..
   cmake --build . --config Release -j

The compilation may take some time so do not worry.

-------------
Local Install
-------------

After the compilation is complete, run (in Linux):

.. code-block:: bash

   sudo cmake --install .

In order to install it in windows, your terminal (cmd or powershell) must be run with administrator privileges. Run (in Windows):

.. code-block:: bash

   cmake --install .

-------------------
Linux Install Paths
-------------------

MBASE will be installed to given paths in Linux:

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
