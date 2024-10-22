#ifndef MBASE_PC_CONFIG
#define MBASE_PC_CONFIG

#include <mbase/common.h>
#include <mbase/behaviors.h>
#include <mbase/string.h>
#include <mbase/filesystem.h>
#include <mbase/unordered_map.h>
#include <mbase/synchronization.h>
#include <mbase/pc/pc_diagnostics.h>

MBASE_BEGIN

class PcConfig;

class MBASE_API PcConfig {
public:
	using config_map = mbase::unordered_map<mbase::string, mbase::string>;

	enum class flags : U8 {
		CONFIG_SUCCESS,
		CONFIG_ERR_NOT_INITIALIZED,
		CONFIG_ERR_MISSING_PATH,
		CONFIG_ERR_MISSING_KEY,
		CONFIG_ERR_PARAM_NOT_FOUND,
		CONFIG_ERR_FILE_NOT_FOUND,
		CONFIG_WARN_KEY_OVERWRITTEN,
		CONFIG_WARN_EMPTY_CONFIG_MAP
	};

	PcConfig();
	~PcConfig() = default;

	flags get_config_param(const mbase::string& in_key, mbase::string& out_param) noexcept; 
	mbase::string get_temp_path() const noexcept; // On windows: C:/Windows/Temp, On Linux: /tmp
	mbase::string get_data_path() const noexcept; // On windows: C:/ProgramData,  On Linux: /var/lib
	const config_map& get_config_map() const noexcept;
	bool is_initialized() const noexcept;

	bool initialize(
		PcDiagnostics& in_diagnostics,
		const mbase::string& in_temp_path = "", 
		const mbase::string& in_root_path = "", 
		const mbase::string& in_data_path = "",
		bool in_main_config = false,
		const mbase::string& in_config_file_name = ""
	);
	flags set_temp_path(const mbase::string& in_path) noexcept;
	flags set_data_path(const mbase::string& in_path) noexcept;
	flags set_root_path(const mbase::string& in_path) noexcept;
	flags load_config_file(const mbase::string& in_file, config_map& out_cmap) noexcept;
	flags update() noexcept;
	flags update(config_map& in_cmap) noexcept;
	flags set_config_param(const mbase::string& in_key, const mbase::string& in_param) noexcept;
	flags dump_to_string(mbase::string& out_config_string) noexcept;
	virtual GENERIC on_initializing();
	virtual GENERIC on_initialize();
	virtual GENERIC on_initialize_fail();
	virtual GENERIC on_destroying();
	virtual GENERIC on_destroy();
	virtual GENERIC on_config_update();
	virtual GENERIC on_temp_path_update();
	virtual GENERIC on_data_path_update();
	virtual GENERIC on_root_path_update();

private:
	PcDiagnostics* mDiagnosticsManager;
	mbase::string mTempPath;
	mbase::string mDataPath;
	mbase::string mRootPath;
	mbase::string mConfigFileName;
	config_map mConfigMap;
	bool mIsInitialized;
	bool mIsUpdated;
};

MBASE_END

#endif // !MBASE_PC_CONFIG
