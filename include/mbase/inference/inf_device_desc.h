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
    InfDeviceDescription(ggml_backend_dev_t in_device, const I32& in_index) noexcept;
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
    MBASE_ND(MBASE_OBS_IGNORE) I32 get_device_index() const;
    MBASE_ND(MBASE_OBS_IGNORE) device_type get_device_type() const;
    MBASE_ND(MBASE_OBS_IGNORE) ggml_backend_dev_t get_internal_dev_handle();
    /* ===== OBSERVATION METHODS END ===== */
private:
    ggml_backend_dev_t mBackendDevice;
    I32 mDeviceIndex;
};

MBASE_API mbase::vector<InfDeviceDescription> inf_query_devices();
MBASE_API mbase::vector<InfDeviceDescription> inf_query_gpu_devices();

MBASE_END

#endif // MBASE_INF_DEVICE_DESC_H