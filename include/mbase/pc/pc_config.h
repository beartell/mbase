#ifndef MBASE_PC_CONFIG
#define MBASE_PC_CONFIG

#include <mbase/common.h>
#include <mbase/behaviors.h>
#include <mbase/string.h>
#include <mbase/filesystem.h>
#include <mbase/unordered_map.h>
#include <mbase/synchronization.h>
#include <mbase/pc/pc_diagnostics.h>
#include <mbase/smart_conversion.h>

MBASE_BEGIN

struct PcConfigDescription {
	mbase::string mConfigKey;
	mbase::string mConfigValue;
};

class PcConfig;

class MBASE_API PcConfig {
public:
	using config_descriptions = mbase::vector<PcConfigDescription>;

	enum class flags : U8 {
		CONFIG_SUCCESS,
		CONFIG_ERR_NOT_INITIALIZED,
		CONFIG_ERR_MISSING_PATH,
		CONFIG_ERR_MISSING_KEY,
		CONFIG_ERR_MISSING_VALUE,
		CONFIG_ERR_PARAM_NOT_FOUND,
		CONFIG_ERR_FILE_NOT_FOUND,
		CONFIG_WARN_KEY_OVERWRITTEN,
		CONFIG_WARN_EMPTY_CONFIG_MAP
	};

	PcConfig();
	~PcConfig();

	template<typename Type>
	flags get_config_param(const mbase::string& in_key, Type& out_param) noexcept
	{
		if(!is_initialized())
		{
			return flags::CONFIG_ERR_NOT_INITIALIZED;
		}

		if(!in_key.size())
		{
			return flags::CONFIG_ERR_MISSING_KEY;
		}

		for(config_descriptions::const_iterator cIt = mConfigDescriptions.begin(); cIt != mConfigDescriptions.end(); ++cIt)
		{
			if(cIt->mConfigKey == in_key)
			{
				out_param = mbase::smart_conversion<Type>::apply(cIt->mConfigValue.c_str());
				return flags::CONFIG_SUCCESS;
			}
		}

		return flags::CONFIG_ERR_PARAM_NOT_FOUND;
	}
	const config_descriptions& get_config_descriptions() const noexcept;
	PcDiagnostics* get_assigned_diagnostics() noexcept;
	bool is_initialized() const noexcept;
	bool initialize(
		PcDiagnostics& in_diagnostics,
		const mbase::wstring& in_config_file_name
	);
	bool load_config_file(const mbase::wstring& in_file, config_descriptions& out_cmap) noexcept;
	bool load_config_file(const mbase::wstring& in_file) noexcept;
	flags update() noexcept;
	flags set_config_param(const mbase::string& in_key, const mbase::string& in_param) noexcept;
	flags dump_to_string(mbase::string& out_config_string) noexcept;

private:
	PcDiagnostics* mDiagnosticsManager;
	mbase::wstring mConfigFileName;
	config_descriptions mConfigDescriptions;
	bool mIsInitialized;
	bool mIsUpdated;
};

MBASE_END

#endif // !MBASE_PC_CONFIG
