#include <mbase/inference/inf_gguf_metadata_configurator.h>
#include <mbase/filesystem.h>
#include <mbase/io_file.h>

MBASE_BEGIN

GgufMetaConfigurator::GgufMetaConfigurator(const mbase::string in_filename) :
	mGgufContext(NULL),
	mIsModified(false),
	mGgufFile(in_filename),
	mOldMetaSize(0)
{
	if(!mGgufFile.size())
	{
		return;
	}
	gguf_init_params gip = { false, NULL };
	mGgufContext = gguf_init_from_file(in_filename.c_str(), gip);
	if(mGgufContext)
	{
		for(I32 i = 0; i < gguf_get_n_kv(mGgufContext); ++i)
		{
			mbase::string ggufKey = gguf_get_key(mGgufContext, i);
			mMetadataMap[ggufKey] = i;
		}
		mOldMetaSize = gguf_get_meta_size(mGgufContext);
	}
}

GgufMetaConfigurator::~GgufMetaConfigurator()
{
	this->_clear_context();
}

bool GgufMetaConfigurator::is_open()
{
	return mGgufContext != NULL;
}

typename GgufMetaConfigurator::size_type GgufMetaConfigurator::get_metadata_count()
{
	return mMetadataMap.size();
}

typename GgufMetaConfigurator::kv_map& GgufMetaConfigurator::get_kv_map()
{
	return mMetadataMap;
}

GENERIC GgufMetaConfigurator::remove_key(const mbase::string& in_key)
{
	if(this->is_open())
	{
		if(mMetadataMap.find(in_key) != mMetadataMap.end())
		{
			mMetadataMap.erase(in_key);
		}
		gguf_remove_key(mGgufContext, in_key.c_str());
		return;
	}
}

GENERIC GgufMetaConfigurator::_clear_context()
{
	if(mGgufContext)
	{
		if(mIsModified)
		{
			size_type metaDataSize = gguf_get_meta_size(mGgufContext);
			mbase::string modifiedFileName = mGgufFile + ".modif";
			
			gguf_write_to_file(mGgufContext, modifiedFileName.c_str(), true);

			mbase::io_file oldGgufFile;
			mbase::io_file newGgufFile;

			oldGgufFile.open_file(mGgufFile, mbase::io_file::access_mode::READ_ACCESS, mbase::io_file::disposition::OPEN);
			oldGgufFile.set_file_pointer(mOldMetaSize, mbase::io_base::move_method::MV_BEGIN);

			mbase::deep_char_stream dcs(oldGgufFile.get_file_size() - mOldMetaSize);
			oldGgufFile.read_data(dcs);
			oldGgufFile.close_file();

			newGgufFile.open_file(modifiedFileName, mbase::io_file::access_mode::RW_ACCESS, mbase::io_file::disposition::APPEND);
			newGgufFile.write_data(dcs.get_buffer(), dcs.buffer_length());
			newGgufFile.close_file();
			
			MoveFileExA(modifiedFileName.c_str(), mGgufFile.c_str(), MOVEFILE_REPLACE_EXISTING);
		}
		gguf_free(mGgufContext);
		mGgufContext = NULL;
		mGgufFile = "";
		mIsModified = false;
		mMetadataMap.clear();
		mOldMetaSize = 0;
	}
}

MBASE_END