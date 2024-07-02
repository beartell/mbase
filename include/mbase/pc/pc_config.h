#ifndef MBASE_PC_CONFIG
#define MBASE_PC_CONFIG

#include <mbase/common.h>
#include <mbase/behaviors.h>
#include <mbase/string.h>
#include <mbase/unordered_map.h>

MBASE_BEGIN

class PcConfig : public mbase::singleton<PcConfig> {
public:
	using config_map = mbase::unordered_map<mbase::string, mbase::string>;

	enum class flags : U8 {
		CONFIG_SUCCESS,
		CONFIG_ERR_MISSING_PATH,
		CONFIG_ERR_PARAM_NOT_FOUND,
		CONFIG_ERR_FILE_NOT_FOUND,
		CONFIG_WARN_KEY_OVERWRITTEN,
		CONFIG_WARN_EMPTY_CONFIG_MAP
	};

	bool initialize();
	flags set_temp_path(const mbase::string& in_path) noexcept;
	flags set_root_path(const mbase::string& in_path) noexcept;
	flags set_config_path(const mbase::string& in_path) noexcept;
	flags load_config_file(const mbase::string& in_file, config_map& out_cmap) noexcept;
	flags update() noexcept;
	flags update(config_map& in_cmap) noexcept;
	flags set_config_param(const mbase::string& in_key, const mbase::string& in_param) noexcept;

	flags get_config_param(const mbase::string& in_key, mbase::string& out_param) noexcept; 
	mbase::string get_temp_path() const noexcept;
	mbase::string get_root_path() const noexcept;
	config_map& get_config_map() noexcept;

private:
	mbase::string mTempPath;
	mbase::string mRootPath;
	mbase::string mConfigPath;
	config_map mConfigMap;
};

MBASE_END

#endif // !MBASE_PC_CONFIG
