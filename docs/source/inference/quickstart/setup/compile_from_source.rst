.. include:: ../../links.rst

=====================
Compiling From Source
=====================

.. important::

   Make sure you set the requirements up for development :doc:`requirements`

    `llama.cpp <llama.cpp_>`_ supports multiple backends for inference.
    This documentation only mentions the normal CUDA GPU, x86_64, and Apple Metal backend builds.
    In order to build the llama.cpp library with the backend you desire or detailed configuration and for more information in general, refer to `llama.cpp <llama.cpp_>`_ repository.


You can compile the MBASE as a bundle compilation with `llama.cpp <llama.cpp_>`_ or standalone compile it.

The difference between two is that unlike the standalone compilation, bundled one doesn't require `llama.cpp <llama.cpp_>`_ to be present on your system
since it compiles `llama.cpp <llama.cpp_>`_ with the MBASE library.

Fortunately, you don't need to do any configuration on your part, MBASE will detect if you have a `llama.cpp <llama.cpp_>`_ in your environment
and link the necessary libraries accordingly but for the bundled compilation, you need to initialize the submodule.

--------------
Common Options
--------------

There are some common CMake options worth mentioning. They can be specified at the cmake configuration stage.

:code:`MBASE_SERVER_SSL`: If set, openai server program will be with SSL support. However, if you set this up, make sure you have the :code:`openssl` installed in your environment. (default=OFF)

:code:`MBASE_FORCE_BUNDLE`: It set, the compilation will compile the `llama.cpp <llama.cpp_>`_ with MBASE even if you have `llama.cpp <llama.cpp_>`_ installed in your environment. (default=OFF)

:code:`GGML_CUDA`: `llama.cpp <llama.cpp_>`_ library option. If set, cuda compilation will be enabled.

-------------------
Bundled Compilation
-------------------

.. important::

    If you have a `llama.cpp <llama.cpp_>`_ in your environment, you should do the standalone compile.

First, you need to clone the `MBASE <mbase_main_>`_ repository:

.. code-block:: bash

    git clone https://github.com/Emreerdog/mbase

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

    cd mbase
    cmake -B build_x86 -DMBASE_SERVER_SSL=ON -DMBASE_FORCE_BUNDLE=ON
    cmake --build build_x86 --config Release -j

The compilation may take some time so do not worry.

^^^^^^^^^^^^^^^^^
Apple Metal BUILD
^^^^^^^^^^^^^^^^^

In order to build Apple Metal backend, do the following:

.. code-block:: bash

    cd mbase
    cmake -B build_metal -DGGML_METAL=ON -DGGML_METAL_USE_BF16=ON -DGGML_METAL_EMBED_LIBRARY=ON -DMBASE_SERVER_SSL=ON -DMBASE_FORCE_BUNDLE=ON
    cmake --build build_metal --config Release -j

The compilation may take some time so do not worry.

^^^^^^^^^^^^^^
CUDA GPU BUILD
^^^^^^^^^^^^^^

.. important::
    Before you attempt to build CUDA backend, make sure you have the cuda toolkit installed in your environment.

    * :doc:`For Windows<preparing_windows>`

    * :doc:`For Ubuntu<preparing_ubuntu>`

NVIDIA GPUs have different CUDA compute capabilities which can be seen `here <https://developer.nvidia.com/cuda-gpus>`_. 
User must set the :code:`CMAKE_CUDA_ARCHITECTURES` option accordingly to what GPUs user wants the compile the MBASE to. In our case, we simply will compile for the native GPU we have.

Now, in order to compile with CUDA you should specify the :code:`CMAKE_CUDA_ARCHITECTURES` as native and set the :code:`GGML_CUDA` option to on:

.. code-block:: bash

   cd mbase
   cmake -B build_gpu -DGGML_CUDA=0N -DCMAKE_CUDA_ARCHITECTURES="native" -DMBASE_SERVER_SSL=ON -DMBASE_FORCE_BUNDLE=ON
   cmake --build build_gpu --config Release -j

The compilation may take some time so do not worry.

----------------------
Standalone Compilation
----------------------

In order to build the MBASE, do the following:

.. code-block:: bash

    cd mbase
    cmake -B build -DMBASE_SERVER_SSL=ON
    cmake --build build --config Release -j

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
