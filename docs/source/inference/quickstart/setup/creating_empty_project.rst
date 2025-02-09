.. include:: ../../links.rst

=========================
Creating an Empty Project
=========================

-----
About
-----

In this page, you will create an empty CMake project and link the inference library into your project.

----------------
Setting up CMake 
----------------

Now in your working directory, create a directory named 'mbase_simple_project' and go into that directory:

.. code-block:: bash

    mkdir mbase_simple_project

In that directory, open your favorite IDE or using terminal, create two files given as:

* CMakeLists.txt
* main.cpp

.. code-block:: bash
    
    cd mbase_simple_project
    touch CMakeLists.txt
    touch main.cpp

Now, properly configure your CMakeLists.txt by setting up a project and minimum version etc.

.. code-block:: cmake
    :caption: CMakeLists.txt

    cmake_minimum_required(VERSION 3.15...3.31)
    project("mbase_simple_project" LANGUAGES CXX)

Then create an executable named **simple_project** target for main.cpp.

.. code-block:: cmake
    :caption: CMakeLists.txt

    add_executable(simple_project main.cpp)

Then, we will set the C++ version as 17 (as long as the minimum is 17, higher versions are fine) and then,
we will find the MBASE libraries and link the inference library and specify the include path. Here is how you do it:

.. code-block:: cmake
    :caption: CMakeLists.txt

    find_package(mbase.libs REQUIRED COMPONENTS std inference)

    target_compile_features(simple_project PUBLIC cxx_std_17)
    target_link_libraries(simple_project PRIVATE mbase-std mbase-inference)
    target_include_directories(simple_project PUBLIC mbase-std mbase-inference)

After all those operations, your CMakeLists.txt should look like this:

.. code-block:: cmake
    :caption: CMakeLists.txt

    cmake_minimum_required(VERSION 3.15...3.31)
    project("mbase_simple_project" LANGUAGES CXX)

    add_executable(simple_project main.cpp)

    find_package(mbase.libs REQUIRED COMPONENTS std inference)

    target_compile_features(simple_project PUBLIC cxx_std_17)
    target_link_libraries(simple_project PRIVATE mbase-std mbase-inference)
    target_include_directories(simple_project PUBLIC mbase-std mbase-inference)

-------------------
Setting up main.cpp
-------------------

After we set cmake configuration, as a hello world, we will print the names of the devices that are available on our system using the inference library to see if everything works correctly and libraries are linked properly.
Here is how we do it:

.. code-block:: cpp
    :caption: main.cpp

    #include <mbase/inference/inf_device_desc.h>
    #include <mbase/vector.h>
    #include <iostream>

    int main()
    {
        mbase::vector<mbase::InfDeviceDescription> deviceDesc = mbase::inf_query_devices();

        for(mbase::vector<mbase::InfDeviceDescription>::iterator It = deviceDesc.begin(); It != deviceDesc.end(); It++)
        {
            std::cout << It->get_device_description() << std::endl;
        }

        return 0;
    }

After you run this program, you will see the available devices in your system for inference.


.. important::
    If you have an NVIDIA or AMD GPU and its is not being displayed by the program,
    make sure you have installed the necessary drivers and installed the `llama.cpp <llama.cpp_>`_ library with proper configuration.

    Refer to: :doc:`Setting up<setting_up>`
    For bundled install: :doc:`Download <download>`

------------------------------
Possible Shared Library Errors
------------------------------

If the program is not run due to a shared library or dll not found error, 
Make sure to set the shared lib path for unix or for the windows keep the dlls in the same directory as your executable.

^^^^^^^^^^^^^^^^^^^
UNIX Short-term Fix
^^^^^^^^^^^^^^^^^^^

Run this in the terminal session where you run the program.

.. code-block:: bash

    LD_LIBRARY_PATH=/usr/local/lib
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH

^^^^^^^^^^^^^^^^^^
UNIX Long-term Fix
^^^^^^^^^^^^^^^^^^

In long-term fix, we will modify our bash config file (assuming ~/.bashrc) and put the export
line there. Open your :code:`~/.bashrc` and put that line:

.. code-block:: bash
    :caption: ~/.bashrc

    export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH

Too apply changes and keep your session:

.. code-block:: bash

    source ~/.bashrc

^^^^^^^^^^^^^^^^^^^^^^
Windows Short-term Fix
^^^^^^^^^^^^^^^^^^^^^^

Copy and paste the dlls that are produced by installing `llama.cpp <llama.cpp_>`_ and `MBASE <mbase_main_>`_.

* llama.cpp dll location (drag-drop all dlls): **C:/Program Files (x86)/llama.cpp/bin**
* MBASE libraries dll location (drag-drop all dlls): **C:/Program Files (x86)/mbase.libs/bin**

^^^^^^^^^^^^^^^^^^^^^
Windows Long-term Fix
^^^^^^^^^^^^^^^^^^^^^

In windows, you need to add the paths of binaries to your PATH variable.

* llama.cpp dll location (drag-drop all dlls): **C:/Program Files (x86)/llama.cpp/bin**
* MBASE libraries dll location (drag-drop all dlls): **C:/Program Files (x86)/mbase.libs/bin**