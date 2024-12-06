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

    InfDeviceDescription();
    InfDeviceDescription(ggml_backend_dev_t in_device, const I32& in_index);
    InfDeviceDescription(const InfDeviceDescription& in_description);
    ~InfDeviceDescription();

    InfDeviceDescription& operator=(const InfDeviceDescription& in_rhs);

    mbase::string get_device_name();
    mbase::string get_device_description();
    size_type get_total_memory();
    size_type get_free_memory();
    I32 get_device_index();
    device_type get_device_type();
    ggml_backend_dev_t get_internal_dev_handle();
private:
    ggml_backend_dev_t mBackendDevice;
    I32 mDeviceIndex;
};

MBASE_API mbase::vector<InfDeviceDescription> inf_query_devices();
MBASE_API mbase::vector<InfDeviceDescription> inf_query_gpu_devices();

MBASE_END

#endif // MBASE_INF_DEVICE_DESC_H