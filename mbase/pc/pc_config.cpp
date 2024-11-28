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
	mDiagnosticsManager(NULL),
	mIsInitialized(false),
	mIsUpdated(false)
{

}

PcConfig::~PcConfig()
{
	if(mDiagnosticsManager)
	{
		update();
	}
}

const typename PcConfig::config_descriptions& PcConfig::get_config_descriptions() const noexcept
{
	return mConfigDescriptions;
}

PcDiagnostics* PcConfig::get_assigned_diagnostics() noexcept
{
	return mDiagnosticsManager;
}

bool PcConfig::is_initialized() const noexcept
{
	return mIsInitialized;
}

bool PcConfig::initialize(PcDiagnostics& in_diagnostics, const mbase::wstring& in_config_file_name)
{
	if (is_initialized())
	{
		return true;
	}

	if(!in_config_file_name.size())
	{
		return false;
	}

	in_diagnostics.log(mbase::PcDiagnostics::flags::LOGTYPE_INFO, mbase::PcDiagnostics::flags::LOGIMPORTANCE_LOW, "Initializing config object: " + mbase::to_utf8(in_config_file_name));
	mbase::io_file iof;
	iof.open_file(in_config_file_name, mbase::io_file::access_mode::READ_ACCESS, mbase::io_file::disposition::OPEN);
	if(!iof.is_file_open())
	{
		in_diagnostics.log(mbase::PcDiagnostics::flags::LOGTYPE_INFO, mbase::PcDiagnostics::flags::LOGIMPORTANCE_LOW, "Can not open file: " + mbase::to_utf8(in_config_file_name));
		in_diagnostics.log(mbase::PcDiagnostics::flags::LOGTYPE_INFO, mbase::PcDiagnostics::flags::LOGIMPORTANCE_LOW, "Config init failed: " + mbase::to_utf8(in_config_file_name));
	}
	iof.close_file();
	mIsInitialized = true;
	mDiagnosticsManager = &in_diagnostics;
	load_config_file(in_config_file_name);

	in_diagnostics.log(mbase::PcDiagnostics::flags::LOGTYPE_INFO, mbase::PcDiagnostics::flags::LOGIMPORTANCE_LOW, "Config object initialized.");
	
	mDiagnosticsManager = &in_diagnostics;
	mConfigFileName = in_config_file_name;

	return true;
}

bool PcConfig::load_config_file(const mbase::wstring& in_file, config_descriptions& out_cmap) noexcept
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
		fileString.split(MBASE_PLATFORM_NEWLINE, configLines);

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
					for(; beginIndex < endIndex; ++beginIndex)
					{
						newConfigValue.push_back(configValue[beginIndex]);
					}
					configValue = std::move(newConfigValue);
					std::cout << configKey << "---" << configValue << std::endl;
				}

				set_config_param(configKey, configValue);
			}
		}
		return true;
	}
	return false;
}

bool PcConfig::load_config_file(const mbase::wstring& in_file) noexcept
{
	if(load_config_file(in_file, mConfigDescriptions))
	{
		mConfigFileName = in_file;
		return true;
	}
	return false;
}

PcConfig::flags PcConfig::update() noexcept
{
	MBASE_CONFIG_RETURN_UNINITIALIZED;

	if(mIsUpdated)
	{
		mbase::string totalConfigString;
		for(config_descriptions::iterator It = mConfigDescriptions.begin(); It != mConfigDescriptions.end(); ++It)
		{
			totalConfigString += It->mConfigKey + "=" + It->mConfigValue + MBASE_PLATFORM_NEWLINE;
		}
		U32 serializedSize = totalConfigString.get_serialized_size();
		mbase::deep_char_stream dcs(serializedSize);
		totalConfigString.serialize(dcs);
		
		mbase::io_file updatedConfigFile;
		updatedConfigFile.open_file(mConfigFileName);
		updatedConfigFile.write_data(dcs.get_buffer(), dcs.buffer_length());
		mIsUpdated = false;
	}

	return flags::CONFIG_SUCCESS;
}

PcConfig::flags PcConfig::set_config_param(const mbase::string& in_key, const mbase::string& in_param) noexcept
{
	MBASE_CONFIG_RETURN_UNINITIALIZED;
	
	if (!in_key.size())
	{
		return flags::CONFIG_ERR_MISSING_KEY;
	}

	if(!in_param.size())
	{
		return flags::CONFIG_ERR_MISSING_VALUE;
	}

	for(config_descriptions::iterator cIt = mConfigDescriptions.begin(); cIt != mConfigDescriptions.end(); ++cIt)
	{
		if(cIt->mConfigKey == in_key)
		{
			mDiagnosticsManager->log(mbase::PcDiagnostics::flags::LOGTYPE_WARNING, mbase::PcDiagnostics::flags::LOGIMPORTANCE_MID, mbase::string::from_format("Config key/value: %s=%s, overwritten by value: %s", cIt->mConfigKey.c_str(), cIt->mConfigValue.c_str(), in_param.c_str()));	
			cIt->mConfigValue = in_param;
			return flags::CONFIG_WARN_KEY_OVERWRITTEN;
		}
	}
		
	mIsUpdated = true;
	mConfigDescriptions.push_back({in_key, in_param});

	mDiagnosticsManager->log(mbase::PcDiagnostics::flags::LOGTYPE_SUCCESS, mbase::PcDiagnostics::flags::LOGIMPORTANCE_MID, mbase::string::from_format("Config key set: %s=%s", in_key.c_str(), in_param.c_str()));
	return flags::CONFIG_SUCCESS;
}

PcConfig::flags PcConfig::dump_to_string(mbase::string& out_config_string) noexcept
{
	MBASE_CONFIG_RETURN_UNINITIALIZED;
	for(config_descriptions::iterator It = mConfigDescriptions.begin(); It != mConfigDescriptions.end(); ++It)
	{
		out_config_string += It->mConfigKey + "=" + It->mConfigValue + MBASE_PLATFORM_NEWLINE;
	}
	return flags::CONFIG_SUCCESS;
}

MBASE_END