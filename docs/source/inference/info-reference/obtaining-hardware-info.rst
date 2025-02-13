.. include:: ../links.rst

==============================
Obtaining Hardware Information
==============================

The hardware information query procedures and the object reside under the :code:`mbase/inference/inf_device_desc.h`
file.

Since the MBASE uses `ggml <ggml_>`_ library for device query, you will need to
have the `llama.cpp <llama.cpp_>`_ installed with correct backends.

-------------------------
Device Description Object
-------------------------

The device information is stored in :code:`InfDeviceDescription` object in a formatted manner
as follows:

.. code-block:: cpp

    class MBASE_API InfDeviceDescription {
    public:
        enum class device_type {
            CPU,
            CUSTOM,
            GPU,
            UNKNOWN
        };

        using size_type = SIZE_T;

        /* ===== BUILDER METHODS BEGIN ===== */
        InfDeviceDescription() noexcept;
        InfDeviceDescription(ggml_backend_dev_t in_device, const size_type& in_index) noexcept;
        InfDeviceDescription(const InfDeviceDescription& in_description) noexcept;
        ~InfDeviceDescription() noexcept;
        /* ===== BUILDER METHODS END ===== */

        /* ===== OPERATOR BUILDER METHODS BEGIN ===== */
        InfDeviceDescription& operator=(const InfDeviceDescription& in_rhs);
        /* ===== OPERATOR BUILDER METHODS END ===== */

        /* ===== OBSERVATION METHODS BEGIN ===== */
        MBASE_ND(MBASE_OBS_IGNORE) mbase::string get_device_name() const;
        MBASE_ND(MBASE_OBS_IGNORE) mbase::string get_device_description() const;
        MBASE_ND(MBASE_OBS_IGNORE) size_type get_total_memory() const;
        MBASE_ND(MBASE_OBS_IGNORE) size_type get_free_memory() const;
        MBASE_ND(MBASE_OBS_IGNORE) size_type get_device_index() const;
        MBASE_ND(MBASE_OBS_IGNORE) device_type get_device_type() const;
        MBASE_ND(MBASE_OBS_IGNORE) ggml_backend_dev_t get_internal_dev_handle();
        /* ===== OBSERVATION METHODS END ===== */
    private:
        ggml_backend_dev_t mBackendDevice;
        size_type mDeviceIndex;
    };

Most methods are self-explanatory but the "CUSTOM" and "UNKNOWN" device types, and the :code:`get_internal_dev_handle`
worth mentioning.

--------------------
Strange Device Types
--------------------

MBASE Uses `ggml <ggml_>`_ library for device query mechanism.
Some backends are accelerator backends and are interpreted as devices
by the `ggml <ggml_>`_ library such as BLAS, AMX etc. 

The :code:`CUSTOM` device type correspond to those accelerator backend devices
and the :code:`UNKNOWN` device is deprecated which will be removed in the future.

----------------------
Internal Device Handle
----------------------

Internal device handle is used to retrieve the raw device handle to be used
by the llama.cpp library.

You can use that device handle to access low-level `ggml <ggml_>`_ procedures.

---------
SDK Usage
---------

There four procedures available for querying the devices on your system.
Those procedures below returns a vector of device description objects:

* :code:`inf_query_devices`: Returns all types of devices in your system.
* :code:`inf_query_custom_devices`: Returns accelerator backend devices.
* :code:`inf_query_cpu_devices`: Returns all CPU devices.
* :code:`inf_query_gpu_devices`: Returns all GPU devices.

The code below queries all devices in your system and prints their device description:

.. code-block:: cpp

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

---------------
Header Synopsis
---------------

.. code-block:: cpp
    :caption: mbase/inference/inf_device_desc.h

    #ifndef MBASE_INF_DEVICE_DESC_H
    #define MBASE_INF_DEVICE_DESC_H

    #include <mbase/common.h>
    #include <mbase/vector.h>
    #include <ggml-backend.h>

    MBASE_BEGIN

    class MBASE_API InfDeviceDescription {
    public:
        enum class device_type {
            CPU,
            CUSTOM,
            GPU,
            UNKNOWN
        };

        using size_type = SIZE_T;

        /* ===== BUILDER METHODS BEGIN ===== */
        InfDeviceDescription() noexcept;
        InfDeviceDescription(ggml_backend_dev_t in_device, const size_type& in_index) noexcept;
        InfDeviceDescription(const InfDeviceDescription& in_description) noexcept;
        ~InfDeviceDescription() noexcept;
        /* ===== BUILDER METHODS END ===== */

        /* ===== OPERATOR BUILDER METHODS BEGIN ===== */
        InfDeviceDescription& operator=(const InfDeviceDescription& in_rhs);
        /* ===== OPERATOR BUILDER METHODS END ===== */

        /* ===== OBSERVATION METHODS BEGIN ===== */
        MBASE_ND(MBASE_OBS_IGNORE) mbase::string get_device_name() const;
        MBASE_ND(MBASE_OBS_IGNORE) mbase::string get_device_description() const;
        MBASE_ND(MBASE_OBS_IGNORE) size_type get_total_memory() const;
        MBASE_ND(MBASE_OBS_IGNORE) size_type get_free_memory() const;
        MBASE_ND(MBASE_OBS_IGNORE) size_type get_device_index() const;
        MBASE_ND(MBASE_OBS_IGNORE) device_type get_device_type() const;
        MBASE_ND(MBASE_OBS_IGNORE) ggml_backend_dev_t get_internal_dev_handle();
        /* ===== OBSERVATION METHODS END ===== */
    private:
        ggml_backend_dev_t mBackendDevice;
        size_type mDeviceIndex;
    };

    MBASE_API mbase::vector<InfDeviceDescription> inf_query_devices();
    MBASE_API mbase::vector<InfDeviceDescription> inf_query_gpu_devices();

    MBASE_END

    #endif // MBASE_INF_DEVICE_DESC_H


