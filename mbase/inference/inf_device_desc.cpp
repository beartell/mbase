#include <mbase/inference/inf_device_desc.h>

MBASE_BEGIN

InfDeviceDescription::InfDeviceDescription() noexcept :
    mBackendDevice(NULL),
    mDeviceIndex(-1)
{

}

InfDeviceDescription::InfDeviceDescription(ggml_backend_dev_t in_device, const I32& in_index) noexcept :
    mBackendDevice(in_device),
    mDeviceIndex(in_index)
{
}

InfDeviceDescription::InfDeviceDescription(const InfDeviceDescription& in_description) noexcept:
    mBackendDevice(in_description.mBackendDevice),
    mDeviceIndex(in_description.mDeviceIndex)
{
}

InfDeviceDescription::~InfDeviceDescription() noexcept
{
}

InfDeviceDescription& InfDeviceDescription::operator=(const InfDeviceDescription& in_rhs)
{
    mBackendDevice = in_rhs.mBackendDevice;
    mDeviceIndex = in_rhs.mDeviceIndex;

    return *this;
}

MBASE_ND(MBASE_OBS_IGNORE) mbase::string InfDeviceDescription::get_device_name() const
{
    if(mBackendDevice)
    {
        return ggml_backend_dev_name(mBackendDevice);
    }
    return mbase::string();
}

MBASE_ND(MBASE_OBS_IGNORE) mbase::string InfDeviceDescription::get_device_description() const
{
    if(mBackendDevice)
    {
        return ggml_backend_dev_description(mBackendDevice);
    }
    return mbase::string();
}

MBASE_ND(MBASE_OBS_IGNORE) typename InfDeviceDescription::size_type InfDeviceDescription::get_total_memory() const
{
    if(mBackendDevice)
    {
        size_type freeMem = 0;
        size_type totalMem = 0;
        ggml_backend_dev_memory(mBackendDevice, &freeMem, &totalMem);
        return totalMem;
    }
    return 0;
}

MBASE_ND(MBASE_OBS_IGNORE) typename InfDeviceDescription::size_type InfDeviceDescription::get_free_memory() const
{
    if(mBackendDevice)
    {
        size_type freeMem = 0;
        size_type totalMem = 0;
        ggml_backend_dev_memory(mBackendDevice, &freeMem, &totalMem);
        return freeMem;
    }
    return 0;
}

MBASE_ND(MBASE_OBS_IGNORE) I32 InfDeviceDescription::get_device_index() const
{
    return mDeviceIndex;
}

MBASE_ND(MBASE_OBS_IGNORE) InfDeviceDescription::device_type InfDeviceDescription::get_device_type() const
{
    if(mBackendDevice)
    {
        switch (ggml_backend_dev_type(mBackendDevice))
        {
        case GGML_BACKEND_DEVICE_TYPE_CPU:
            return device_type::CPU;
        case GGML_BACKEND_DEVICE_TYPE_ACCEL:
            return device_type::CUSTOM;
        case GGML_BACKEND_DEVICE_TYPE_GPU:
            return device_type::GPU;
        default:
            return device_type::UNKNOWN;
        }
    }

    return device_type::UNKNOWN;
}

MBASE_ND(MBASE_OBS_IGNORE) ggml_backend_dev_t InfDeviceDescription::get_internal_dev_handle()
{
    return mBackendDevice;
}

mbase::vector<InfDeviceDescription> inf_query_devices()
{
    mbase::vector<InfDeviceDescription> deviceDescriptions;
    for(SIZE_T i = 0; i < ggml_backend_dev_count(); ++i)
    {
        ggml_backend_dev_t tmpDevice = ggml_backend_dev_get(i);
        deviceDescriptions.push_back(InfDeviceDescription(tmpDevice, i));
    }
    return deviceDescriptions;
}

mbase::vector<InfDeviceDescription> inf_query_gpu_devices()
{
    mbase::vector<InfDeviceDescription> deviceDescriptions;
    for(SIZE_T i = 0; i < ggml_backend_dev_count(); ++i)
    {
        ggml_backend_dev_t tmpDevice = ggml_backend_dev_get(i);
        switch (ggml_backend_dev_type(tmpDevice))
        {
        case GGML_BACKEND_DEVICE_TYPE_CPU:
            break;
        case GGML_BACKEND_DEVICE_TYPE_ACCEL:
            break;
        case GGML_BACKEND_DEVICE_TYPE_GPU:
            deviceDescriptions.push_back(InfDeviceDescription(tmpDevice, i));
            break;
        default:
            break;
        }
    }
    return deviceDescriptions;
}

MBASE_END