#include <mbase/pc/pc_config.h>
#include <mbase/pc/pc_program.h>
#include <mbase/pc/pc_diagnostics.h>
#include <mbase/io_file.h>

#define MBASE_CONFIG_RETURN_UNINITIALIZED \
if(!is_initialized())\
{\
	return flags::CONFIG_ERR_NOT_INITIALIZED;\
}

MBASE_BEGIN

PcConfig::PcConfig() :
	mTempPath(),
	mDataPath(),
	mRootPath(),
	mConfigMap(),
	mIsInitialized(false),
	mIsUpdated(false),
	mDiagnosticsManager(NULL)
{

}

PcConfig::~PcConfig()
{
	if(mDiagnosticsManager)
	{
		update();
	}
}

PcConfig::flags PcConfig::get_config_param(const mbase::string& in_key, mbase::string& out_param) noexcept
{
	MBASE_CONFIG_RETURN_UNINITIALIZED;

	if(!in_key.size())
	{
		return flags::CONFIG_ERR_MISSING_KEY;
	}

	if(mConfigMap.find(in_key) == mConfigMap.end())
	{
		return flags::CONFIG_ERR_PARAM_NOT_FOUND;
	}

	out_param = mConfigMap.at(in_key);

	return flags::CONFIG_SUCCESS;
}

mbase::wstring PcConfig::get_temp_path() const noexcept
{
	return mTempPath;
}

mbase::wstring PcConfig::get_data_path() const noexcept
{
	return mDataPath;
}

typename const PcConfig::config_map& PcConfig::get_config_map() const noexcept
{
	return mConfigMap;
}

bool PcConfig::is_initialized() const noexcept
{
	return mIsInitialized;
}

bool PcConfig::initialize(PcDiagnostics& in_diagnostics, const mbase::wstring& in_temp_path, const mbase::wstring& in_root_path, const mbase::wstring& in_data_path, bool in_main_config, const mbase::wstring& in_config_file_name)
{
	if (is_initialized())
	{
		return true;
	}
	mDiagnosticsManager = &in_diagnostics;

	on_initializing();

	mTempPath = in_temp_path;
	mDataPath = in_data_path;
	mRootPath = in_root_path;

	if (!in_temp_path.size())
	{
		mTempPath = std::move(mbase::get_temp_path());
	}

	if (!in_root_path.size())
	{
		mDataPath = std::move(mbase::get_current_path());
	}

	if (!in_root_path.size())
	{
		mRootPath = std::move(mbase::get_current_path());
	}

	if(mTempPath.back() != '/')
	{
		mTempPath.push_back('/');
	}

	if(mDataPath.back() != '/')
	{
		mDataPath.push_back('/');
	}

	if(mRootPath.back() != '/')
	{
		mRootPath.push_back('/');
	}

	mConfigFileName = in_config_file_name;

	if(in_main_config)
	{
		mDiagnosticsManager->log(mbase::PcDiagnostics::flags::LOGTYPE_INFO, mbase::PcDiagnostics::flags::LOGIMPORTANCE_LOW, "Initializing program config.");
		mDiagnosticsManager->log(mbase::PcDiagnostics::flags::LOGTYPE_INFO, mbase::PcDiagnostics::flags::LOGIMPORTANCE_LOW, "Program temporary path: " + mbase::to_utf8(mTempPath));
		mDiagnosticsManager->log(mbase::PcDiagnostics::flags::LOGTYPE_INFO, mbase::PcDiagnostics::flags::LOGIMPORTANCE_LOW, "Program data path: " + mbase::to_utf8(mDataPath));
		mDiagnosticsManager->log(mbase::PcDiagnostics::flags::LOGTYPE_INFO, mbase::PcDiagnostics::flags::LOGIMPORTANCE_LOW, "Program execution path: " + mbase::to_utf8(mRootPath));

		mbase::vector<mbase::FS_FILE_INFORMATION> fileInfo;
		mbase::get_directory(mDataPath + '*', fileInfo);

		for (mbase::vector<mbase::FS_FILE_INFORMATION>::iterator It = fileInfo.begin(); It != fileInfo.end(); ++It)
		{
			if (It->fileName == L"main_config.txt")
			{
				mConfigFileName = L"main_config.txt";
				mDiagnosticsManager->log(mbase::PcDiagnostics::flags::LOGTYPE_INFO, mbase::PcDiagnostics::flags::LOGIMPORTANCE_MID, "Main configuration folder found.");
				mIsInitialized = true;
				load_config_file(It->fileName);
				
				mDiagnosticsManager->log(mbase::PcDiagnostics::flags::LOGTYPE_SUCCESS, mbase::PcDiagnostics::flags::LOGIMPORTANCE_LOW, "Config object initialized.");

				on_initialize();

				return true;
			}
		}
	}
	
	mDiagnosticsManager->log(mbase::PcDiagnostics::flags::LOGTYPE_SUCCESS, mbase::PcDiagnostics::flags::LOGIMPORTANCE_LOW, "Config object initialized.");
	on_initialize();
	mIsInitialized = true;
	return true;
}

PcConfig::flags PcConfig::set_temp_path(const mbase::wstring& in_path) noexcept
{
	MBASE_CONFIG_RETURN_UNINITIALIZED;

	mDiagnosticsManager->log(mbase::PcDiagnostics::flags::LOGTYPE_INFO, mbase::PcDiagnostics::flags::LOGIMPORTANCE_LOW, "Changing temp path: " + mbase::to_utf8(in_path));

	if (!in_path.size())
	{
		return flags::CONFIG_ERR_MISSING_PATH;
	}
	mIsUpdated = true;
	mTempPath = in_path;
	on_temp_path_update();
	return flags::CONFIG_SUCCESS;
}

PcConfig::flags PcConfig::set_data_path(const mbase::wstring& in_path) noexcept
{
	MBASE_CONFIG_RETURN_UNINITIALIZED;

	mDiagnosticsManager->log(mbase::PcDiagnostics::flags::LOGTYPE_INFO, mbase::PcDiagnostics::flags::LOGIMPORTANCE_LOW, "Changing root path: " + mbase::to_utf8(in_path));

	if (!in_path.size())
	{
		return flags::CONFIG_ERR_MISSING_PATH;
	}
	mIsUpdated = true;
	mDataPath = in_path;
	on_data_path_update();
	return flags::CONFIG_SUCCESS;
}

PcConfig::flags PcConfig::set_root_path(const mbase::wstring& in_path) noexcept
{
	MBASE_CONFIG_RETURN_UNINITIALIZED;
	
	mDiagnosticsManager->log(mbase::PcDiagnostics::flags::LOGTYPE_INFO, mbase::PcDiagnostics::flags::LOGIMPORTANCE_LOW, "Changing config path: " + mbase::to_utf8(in_path));

	if (!in_path.size())
	{
		return flags::CONFIG_ERR_MISSING_PATH;
	}
	mIsUpdated = true;
	mRootPath = in_path;
	on_root_path_update();
	return flags::CONFIG_SUCCESS;
}

bool PcConfig::load_config_file(const mbase::wstring& in_file, config_map& out_cmap) noexcept
{
	mbase::io_file mainConfigFile;
	mainConfigFile.open_file(in_file, mbase::io_file::access_mode::READ_ACCESS, mbase::io_file::disposition::OPEN);

	if (mainConfigFile.is_file_open())
	{
		out_cmap.clear();

		mbase::deep_char_stream dcs(mainConfigFile.get_file_size());
		mainConfigFile.read_data(dcs);
		mbase::string fileString(dcs.get_buffer(), dcs.buffer_length());

		mbase::vector<mbase::string> configLines;
		fileString.split("\r\n", configLines);

		for (mbase::string& configLine : configLines)
		{
			if (!configLine.size())
			{
				continue;
			}

			mbase::vector<mbase::string> configKval;
			configLine.split("=", configKval);

			if (configKval.size() == 2)
			{
				mbase::string configKey = configKval[0];
				configKey.remove_all(' ');
				mbase::string configValue = configKval[1];
				if(configValue.front() == ' ')
				{
					I32 beginIndex = 0;
					I32 endIndex = configValue.size();
					for(mbase::string::iterator It = configValue.begin(); It != configValue.end(); ++It)
					{
						if (*It != ' ') 
						{
							break;
						}
						beginIndex++;
					}

					for(mbase::string::reverse_iterator It = configValue.rbegin(); It != configValue.rend(); ++It)
					{
						if(*It != ' ')
						{
							break;
						}
						endIndex--;
					}

					mbase::string newConfigValue;
					for(beginIndex; beginIndex < endIndex; ++beginIndex)
					{
						newConfigValue.push_back(configValue[beginIndex]);
					}
					configValue = std::move(newConfigValue);
				}

				out_cmap[configKey] = configValue;
			}
		}
		return true;
	}
	return false;
}

bool PcConfig::load_config_file(const mbase::wstring& in_file) noexcept
{
	mConfigFileName = this->get_data_path() + in_file;
	return load_config_file(mConfigFileName, mConfigMap);
}

PcConfig::flags PcConfig::update() noexcept
{
	MBASE_CONFIG_RETURN_UNINITIALIZED;

	if(mIsUpdated)
	{
		mbase::string totalConfigString;
		for(config_map::iterator It = mConfigMap.begin(); It != mConfigMap.end(); ++It)
		{
			totalConfigString += It->first + "=" + It->second + MBASE_PLATFORM_NEWLINE;
		}
		U32 serializedSize = totalConfigString.get_serialized_size();
		mbase::deep_char_stream dcs(serializedSize);
		totalConfigString.serialize(dcs);
		
		mbase::io_file updatedConfigFile;
		updatedConfigFile.open_file(mDataPath + mConfigFileName);
		updatedConfigFile.write_data(dcs.get_buffer(), dcs.buffer_length());
		mIsUpdated = false;
	}

	return flags::CONFIG_SUCCESS;
}

PcConfig::flags PcConfig::set_config_param(const mbase::string& in_key, const mbase::string& in_param) noexcept
{
	MBASE_CONFIG_RETURN_UNINITIALIZED;

	flags returnCode = flags::CONFIG_SUCCESS;

	mDiagnosticsManager->log(mbase::PcDiagnostics::flags::LOGTYPE_INFO, mbase::PcDiagnostics::flags::LOGIMPORTANCE_MID, mbase::string::from_format("Setting up config key: %s=%s", in_key.c_str(), in_param.c_str()));

	if (!in_key.size())
	{
		returnCode = flags::CONFIG_ERR_MISSING_KEY;
		mDiagnosticsManager->log(mbase::PcDiagnostics::flags::LOGTYPE_ERROR, mbase::PcDiagnostics::flags::LOGIMPORTANCE_MID, "Missing config key");
		return returnCode;
	}

	if (mConfigMap.find(in_key) != mConfigMap.end())
	{
		mDiagnosticsManager->log(mbase::PcDiagnostics::flags::LOGTYPE_WARNING, mbase::PcDiagnostics::flags::LOGIMPORTANCE_MID, mbase::string::from_format("Config key: %s=%s, overwritten by value: %s", in_key.c_str(), mConfigMap[in_key].c_str(), in_param.c_str()));
		returnCode = flags::CONFIG_WARN_KEY_OVERWRITTEN;
	}
	mIsUpdated = true;

	mDiagnosticsManager->log(mbase::PcDiagnostics::flags::LOGTYPE_SUCCESS, mbase::PcDiagnostics::flags::LOGIMPORTANCE_MID, mbase::string::from_format("Config key set: %s=%s", in_key.c_str(), in_param.c_str()));
	mConfigMap.insert(mbase::pair(in_key, in_param));
	return returnCode;
}

PcConfig::flags PcConfig::dump_to_string(mbase::string& out_config_string) noexcept
{
	MBASE_CONFIG_RETURN_UNINITIALIZED;
	
	mDiagnosticsManager->log(mbase::PcDiagnostics::flags::LOGTYPE_INFO, mbase::PcDiagnostics::flags::LOGIMPORTANCE_MID, "Dumping config map to string");

	for(config_map::iterator It = mConfigMap.begin(); It != mConfigMap.end(); ++It)
	{
		out_config_string += It->first + "=" + It->second + MBASE_PLATFORM_NEWLINE;
	}
	mDiagnosticsManager->log(mbase::PcDiagnostics::flags::LOGTYPE_SUCCESS, mbase::PcDiagnostics::flags::LOGIMPORTANCE_MID, "Dump complete");
	return flags::CONFIG_SUCCESS;
}

GENERIC PcConfig::on_initializing(){}
GENERIC PcConfig::on_initialize(){}
GENERIC PcConfig::on_initialize_fail(){}
GENERIC PcConfig::on_destroying(){}
GENERIC PcConfig::on_destroy(){}
GENERIC PcConfig::on_config_update() {}
GENERIC PcConfig::on_temp_path_update(){}
GENERIC PcConfig::on_data_path_update(){}
GENERIC PcConfig::on_root_path_update(){}
MBASE_END