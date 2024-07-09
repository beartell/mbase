#include <mbase/pc/pc_config.h>
#include <mbase/pc/pc_program.h>
#include <mbase/pc/pc_diagnostics.h>

#define MBASE_CONFIG_RETURN_UNINITIALIZED \
if(!is_initialized())\
{\
	return flags::CONFIG_ERR_NOT_INITIALIZED;\
}

MBASE_BEGIN

PcConfig::flags PcConfig::get_config_param(const mbase::string& in_key, mbase::string& out_param) const noexcept
{
	MBASE_CONFIG_RETURN_UNINITIALIZED;

	return flags::CONFIG_SUCCESS;
}

mbase::string PcConfig::get_temp_path() const noexcept
{
	return mTempPath;
}

mbase::string PcConfig::get_root_path() const noexcept
{
	return mRootPath;
}

typename const PcConfig::config_map& PcConfig::get_config_map() const noexcept
{
	return mConfigMap;
}

bool PcConfig::is_initialized() const noexcept
{
	return mIsInitialized;
}

bool PcConfig::initialize(mbase::string in_temp_path, mbase::string in_root_path, mbase::string in_config_path)
{
	if (is_initialized())
	{
		return true;
	}

	PcDiagnostics& pcDiag = PcDiagnostics::get_instance();
	pcDiag.log(mbase::PcDiagnostics::flags::LOGTYPE_INFO, mbase::PcDiagnostics::flags::LOGIMPORTANCE_LOW, "Initializing program config.");

	mTempPath = in_temp_path;
	mRootPath = in_root_path;
	mConfigPath = in_config_path;

	if (!in_temp_path.size())
	{
		mTempPath = std::move(mbase::get_temp_path());
	}

	if (!in_root_path.size())
	{
		mRootPath = std::move(mbase::get_current_path());
	}

	if (!in_config_path.size())
	{
		mConfigPath = std::move(mbase::get_current_path());
	}

	pcDiag.log(mbase::PcDiagnostics::flags::LOGTYPE_INFO, mbase::PcDiagnostics::flags::LOGIMPORTANCE_LOW, "Program temporary path: " + mTempPath);
	pcDiag.log(mbase::PcDiagnostics::flags::LOGTYPE_INFO, mbase::PcDiagnostics::flags::LOGIMPORTANCE_LOW, "Program root path: " + mRootPath);

	mbase::vector<mbase::FS_FILE_INFORMATION> fileInfo;
	mbase::get_directory(mConfigPath + "/*", fileInfo);
	mConfigPath += "/config";

	for (mbase::vector<mbase::FS_FILE_INFORMATION>::iterator It = fileInfo.begin(); It != fileInfo.end(); ++It)
	{
		if (It->fileName == "config")
		{
			// config folder exists
			pcDiag.log(mbase::PcDiagnostics::flags::LOGTYPE_INFO, mbase::PcDiagnostics::flags::LOGIMPORTANCE_MID, "'config' folder found and set");
			pcDiag.log(mbase::PcDiagnostics::flags::LOGTYPE_SUCCESS, mbase::PcDiagnostics::flags::LOGIMPORTANCE_LOW, "Config object initialized.");
			mIsInitialized = true;
			return true;
		}
	}

	pcDiag.log(mbase::PcDiagnostics::flags::LOGTYPE_INFO, mbase::PcDiagnostics::flags::LOGIMPORTANCE_LOW, "Creating 'config' folder under root.");
	if (mbase::create_directory(mConfigPath) != mbase::FS_ERROR::FS_SUCCESS)
	{
		pcDiag.log(mbase::PcDiagnostics::flags::LOGTYPE_ERROR, mbase::PcDiagnostics::flags::LOGIMPORTANCE_FATAL, "Unable to create 'config' folder under directory: " + mConfigPath);
	}
	mIsInitialized = true;
	return true;
}

PcConfig::flags PcConfig::set_temp_path(const mbase::string& in_path) noexcept
{
	MBASE_CONFIG_RETURN_UNINITIALIZED;

	PcDiagnostics& pcDiag = PcDiagnostics::get_instance();
	pcDiag.log(mbase::PcDiagnostics::flags::LOGTYPE_INFO, mbase::PcDiagnostics::flags::LOGIMPORTANCE_LOW, "Changing temp path: " + in_path);

	if (!in_path.size())
	{
		return flags::CONFIG_ERR_MISSING_PATH;
	}

	mTempPath = in_path;

	return flags::CONFIG_SUCCESS;
}

PcConfig::flags PcConfig::set_root_path(const mbase::string& in_path) noexcept
{
	MBASE_CONFIG_RETURN_UNINITIALIZED;

	PcDiagnostics& pcDiag = PcDiagnostics::get_instance();
	pcDiag.log(mbase::PcDiagnostics::flags::LOGTYPE_INFO, mbase::PcDiagnostics::flags::LOGIMPORTANCE_LOW, "Changing root path: " + in_path);

	if (!in_path.size())
	{
		return flags::CONFIG_ERR_MISSING_PATH;
	}

	mRootPath = in_path;
	MBASE_PROGRAM_INVOKE_EVENT_WDATA("root_path_updated", mRootPath.data());
	return flags::CONFIG_SUCCESS;
}

PcConfig::flags PcConfig::set_config_path(const mbase::string& in_path) noexcept
{
	MBASE_CONFIG_RETURN_UNINITIALIZED;

	PcDiagnostics& pcDiag = PcDiagnostics::get_instance();
	pcDiag.log(mbase::PcDiagnostics::flags::LOGTYPE_INFO, mbase::PcDiagnostics::flags::LOGIMPORTANCE_LOW, "Changing config path: " + in_path);

	if (!in_path.size())
	{
		return flags::CONFIG_ERR_MISSING_PATH;
	}

	mConfigPath = in_path;

	return flags::CONFIG_SUCCESS;
}

PcConfig::flags PcConfig::load_config_file(const mbase::string& in_file, config_map& out_cmap) noexcept
{
	// TODO, IMPLEMENT AFTER IMPLEMENTING THE IO MANAGER
	MBASE_CONFIG_RETURN_UNINITIALIZED;

	PcDiagnostics& pcDiag = PcDiagnostics::get_instance();
	return flags::CONFIG_SUCCESS;
}

PcConfig::flags PcConfig::update() noexcept
{
	// TODO, IMPLEMENT AFTER IMPLEMENTING THE IO MANAGER
	MBASE_CONFIG_RETURN_UNINITIALIZED;

	PcDiagnostics& pcDiag = PcDiagnostics::get_instance();

	return flags::CONFIG_SUCCESS;
}

PcConfig::flags PcConfig::update(config_map& in_cmap) noexcept
{
	MBASE_CONFIG_RETURN_UNINITIALIZED;
	PcDiagnostics& pcDiag = PcDiagnostics::get_instance();
	pcDiag.log(mbase::PcDiagnostics::flags::LOGTYPE_INFO, mbase::PcDiagnostics::flags::LOGIMPORTANCE_MID, "Loading new config map");

	mConfigMap = in_cmap;

	pcDiag.log(mbase::PcDiagnostics::flags::LOGTYPE_INFO, mbase::PcDiagnostics::flags::LOGIMPORTANCE_MID, "New config map loaded");
	return flags::CONFIG_SUCCESS;
}

PcConfig::flags PcConfig::set_config_param(const mbase::string& in_key, const mbase::string& in_param) noexcept
{
	MBASE_CONFIG_RETURN_UNINITIALIZED;

	PcDiagnostics& pcDiag = PcDiagnostics::get_instance();
	flags returnCode = flags::CONFIG_SUCCESS;

	pcDiag.log(mbase::PcDiagnostics::flags::LOGTYPE_INFO, mbase::PcDiagnostics::flags::LOGIMPORTANCE_MID, mbase::string::from_format("Setting up config key: %s=%s", in_key.c_str(), in_param.c_str()));

	if (!in_key.size())
	{
		returnCode = flags::CONFIG_ERR_MISSING_KEY;
		pcDiag.log(mbase::PcDiagnostics::flags::LOGTYPE_ERROR, mbase::PcDiagnostics::flags::LOGIMPORTANCE_MID, "Missing config key");
		return returnCode;
	}

	if (mConfigMap.find(in_key) != mConfigMap.end())
	{
		pcDiag.log(mbase::PcDiagnostics::flags::LOGTYPE_WARNING, mbase::PcDiagnostics::flags::LOGIMPORTANCE_MID, mbase::string::from_format("Config key: %s=%s, overwritten by value: %s", in_key.c_str(), mConfigMap[in_key].c_str(), in_param.c_str()));
		returnCode = flags::CONFIG_WARN_KEY_OVERWRITTEN;
	}
	pcDiag.log(mbase::PcDiagnostics::flags::LOGTYPE_SUCCESS, mbase::PcDiagnostics::flags::LOGIMPORTANCE_MID, mbase::string::from_format("Config key set: %s=%s", in_key.c_str(), in_param.c_str()));
	mConfigMap.insert(mbase::pair(in_key, in_param));
	return returnCode;
}

MBASE_END