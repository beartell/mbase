#include <mbase/inference/inf_gguf_metadata_configurator.h>
#include <mbase/filesystem.h>
#include <mbase/io_file.h>
#include <llama.h>

MBASE_BEGIN

GgufMetaConfigurator::GgufMetaConfigurator(const mbase::wstring in_filename) :
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
	mGgufContext = gguf_init_from_file(mbase::to_utf8(in_filename).c_str(), gip);
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
	this->clear_context();
}

bool GgufMetaConfigurator::is_open()
{
	return mGgufContext != NULL;
}

bool GgufMetaConfigurator::has_kv_key(const mbase::string& in_key)
{	
	if(mMetadataMap.find(in_key) != mMetadataMap.end())
	{
		return true;
	}
	return false;
}

typename GgufMetaConfigurator::size_type GgufMetaConfigurator::get_metadata_count()
{
	return mMetadataMap.size();
}

typename GgufMetaConfigurator::kv_map& GgufMetaConfigurator::get_kv_map()
{
	return mMetadataMap;
}

gguf_type GgufMetaConfigurator::get_kv_key_type(const mbase::string& in_key)
{
	if(this->has_kv_key(in_key))
	{
		return gguf_get_kv_type(mGgufContext, mMetadataMap[in_key]);
	}
	
	return gguf_type::GGUF_TYPE_BOOL;
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

GENERIC GgufMetaConfigurator::clear_context()
{
	if(this->is_open())
	{
		if(mIsModified)
		{
			// This place needs some refactoring
			//size_type metaDataSize = gguf_get_meta_size(mGgufContext);
			mbase::wstring modifiedFileName = mGgufFile + L".modif";
			
			gguf_write_to_file(mGgufContext, mbase::to_utf8(modifiedFileName).c_str(), true);

			mbase::io_file oldGgufFile;
			mbase::io_file newGgufFile;

			oldGgufFile.open_file(mGgufFile, mbase::io_file::access_mode::READ_ACCESS, mbase::io_file::disposition::OPEN);
			oldGgufFile.set_file_pointer(mOldMetaSize, mbase::io_base::move_method::MV_BEGIN);

			newGgufFile.open_file(modifiedFileName, mbase::io_file::access_mode::RW_ACCESS, mbase::io_file::disposition::APPEND);

			U64 bytesWritten = 0;
			U64 bytesToWrite = oldGgufFile.get_file_size() - mOldMetaSize;
			U64 bytesToReadEachIter = 8192;

			mbase::deep_char_stream oldFileData(bytesToReadEachIter);

			while(bytesWritten < bytesToWrite)
			{
				U64 bytesRead = oldGgufFile.read_data(oldFileData);				
				bytesWritten += newGgufFile.write_data(oldFileData.get_buffer(), bytesRead);

				oldFileData.set_cursor_front();
			}

			oldGgufFile.close_file();
			newGgufFile.close_file();
			#ifdef MBASE_PLATFORM_WINDOWS
			MoveFileExW(modifiedFileName.c_str(), mGgufFile.c_str(), MOVEFILE_REPLACE_EXISTING);
			#endif
			
			#ifdef MBASE_PLATFORM_UNIX
			rename(mbase::to_utf8(modifiedFileName).c_str(), mbase::to_utf8(mGgufFile).c_str());
			#endif
			
		}
		gguf_free(mGgufContext);
		mGgufContext = NULL;
		mGgufFile.clear();
		mIsModified = false;
		mMetadataMap.clear();
		mOldMetaSize = 0;
	}
}

MBASE_END