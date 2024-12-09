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

GgufMetaConfigurator::param_application_error GgufMetaConfigurator::apply_mbase_parameter(const mbase::string& in_system_prompt)
{
	if(this->is_open())
	{
		mbase::string modelName;
		mbase::string modelArchitecture;
		U32 quantizationType;
		U32 blockCount;
		U32 headCount;
		U32 embeddingLength;
		F32 quantizationCoefficient = 32.0;
		llama_ftype enumQuantizationType = llama_ftype::LLAMA_FTYPE_ALL_F32;

		if (!this->get_key("general.architecture", modelArchitecture)) 
		{
			// Must be successful. Abort otherwise
			return param_application_error::ERR_ARCH_NOT_SET;
		} 

		if(!modelArchitecture.size())
		{
			return param_application_error::ERR_ARCH_NOT_SET;
		}

		if(!this->get_key("general.name", modelName))
		{
			if(!this->get_key("general.basename", modelName))
			{
				if(!this->get_key("general.base_model.0.name", modelName))
				{
					// Means, model does not have a name
					// Generate a pseudo name with the following format:
					// architecture-unknown-{uuid}
				}
			}
		}

		if(!modelName.size())
		{
			modelName = modelArchitecture + '-' + "unknown" + mbase::string::generate_uuid();
		}

		if(!this->get_key(modelArchitecture + ".block_count", blockCount))
		{
			return param_application_error::ERR_BLOCK_COUNT_NOT_FOUND;
		}

		if(!this->get_key(modelArchitecture + ".embedding_length", embeddingLength))
		{
			return param_application_error::ERR_EMBEDDING_LENGTH_NOT_FOUND;
		}

		if(!this->get_key(modelArchitecture + ".attention.head_count", headCount))
		{
			return param_application_error::ERR_ATTENTION_HEAD_COUNT_NOT_FOUND;
		}
		
		if(!this->get_key("general.file_type", quantizationType))
		{
			// if the quantization type is not found
			// F32 will be assumed
			quantizationType = 0;
		}
		enumQuantizationType = (llama_ftype)quantizationType;

		switch (enumQuantizationType)
		{
		case LLAMA_FTYPE_ALL_F32:
			quantizationCoefficient = 32.0f;
			break;
		case LLAMA_FTYPE_MOSTLY_F16:
			quantizationCoefficient = 16.0f;
			break;
		case LLAMA_FTYPE_MOSTLY_Q4_0:
			quantizationCoefficient = 4.0f;
			break;
		case LLAMA_FTYPE_MOSTLY_Q4_1:
			quantizationCoefficient = 4.0f;
			break;
		case LLAMA_FTYPE_MOSTLY_Q8_0:
			quantizationCoefficient = 8.0f;
			break;
		case LLAMA_FTYPE_MOSTLY_Q5_0:
			quantizationCoefficient = 5.0f;
			break;
		case LLAMA_FTYPE_MOSTLY_Q5_1:
			quantizationCoefficient = 5.0f;
			break;
		case LLAMA_FTYPE_MOSTLY_Q2_K:
			quantizationCoefficient = 2.0f;
			break;
		case LLAMA_FTYPE_MOSTLY_Q3_K_S:
			quantizationCoefficient = 3.0f;
			break;
		case LLAMA_FTYPE_MOSTLY_Q3_K_M:
			quantizationCoefficient = 3.0f;
			break;
		case LLAMA_FTYPE_MOSTLY_Q3_K_L:
			quantizationCoefficient = 3.0f;
			break;
		case LLAMA_FTYPE_MOSTLY_Q4_K_S:
			quantizationCoefficient = 4.0f;
			break;
		case LLAMA_FTYPE_MOSTLY_Q4_K_M:
			quantizationCoefficient = 4.0f;
			break;
		case LLAMA_FTYPE_MOSTLY_Q5_K_S:
			quantizationCoefficient = 5.0f;
			break;
		case LLAMA_FTYPE_MOSTLY_Q5_K_M:
			quantizationCoefficient = 5.0f;
			break;
		case LLAMA_FTYPE_MOSTLY_Q6_K:
			quantizationCoefficient = 6.0f;
			break;
		case LLAMA_FTYPE_MOSTLY_IQ2_XXS:
			quantizationCoefficient = 2.0f;
			break;
		case LLAMA_FTYPE_MOSTLY_IQ2_XS:
			quantizationCoefficient = 2.0f;
			break;
		case LLAMA_FTYPE_MOSTLY_Q2_K_S:
			quantizationCoefficient = 2.0f;
			break;
		case LLAMA_FTYPE_MOSTLY_IQ3_XS:
			quantizationCoefficient = 3.0f;
			break;
		case LLAMA_FTYPE_MOSTLY_IQ3_XXS:
			quantizationCoefficient = 3.0f;
			break;
		case LLAMA_FTYPE_MOSTLY_IQ1_S:
			quantizationCoefficient = 1.0f;
			break;
		case LLAMA_FTYPE_MOSTLY_IQ4_NL:
			quantizationCoefficient = 4.0f;
			break;
		case LLAMA_FTYPE_MOSTLY_IQ3_S:
			quantizationCoefficient = 3.0f;
			break;
		case LLAMA_FTYPE_MOSTLY_IQ3_M:
			quantizationCoefficient = 3.0f;
			break;
		case LLAMA_FTYPE_MOSTLY_IQ2_S:
			quantizationCoefficient = 2.0f;
			break;
		case LLAMA_FTYPE_MOSTLY_IQ2_M:
			quantizationCoefficient = 2.0f;
			break;
		case LLAMA_FTYPE_MOSTLY_IQ4_XS:
			quantizationCoefficient = 4.0f;
			break;
		case LLAMA_FTYPE_MOSTLY_IQ1_M:
			quantizationCoefficient = 1.0f;
			break;
		case LLAMA_FTYPE_MOSTLY_BF16:
			quantizationCoefficient = 16.0f;
			break;
		case LLAMA_FTYPE_MOSTLY_TQ1_0:
			quantizationCoefficient = 1.0f;
			break;
		case LLAMA_FTYPE_MOSTLY_TQ2_0:
			quantizationCoefficient = 2.0f;
			break;
		default:
			break;
		}
		quantizationCoefficient = quantizationCoefficient / 8.0f;
		mbase::io_file iof;
		iof.open_file(mGgufFile, mbase::io_file::access_mode::READ_ACCESS, mbase::io_file::disposition::OPEN);

		this->set_key<mbase::string>("mbase.model_name", modelName);
		this->set_key<mbase::string>("mbase.model_architecture", modelArchitecture);
		this->set_key<F32>("mbase.quantization_coefficient", quantizationCoefficient);
		this->set_key<U32>("mbase.block_count", blockCount);
		this->set_key<U32>("mbase.head_count", headCount);
		this->set_key<U32>("mbase.embedding_length", embeddingLength);
		this->set_key<U64>("mbase.model_size", iof.get_file_size());

		if(in_system_prompt.size())
		{
			this->set_key<mbase::string>("mbase.embedded_system_prompt", in_system_prompt);
		}
	}
	return param_application_error::SUCCESS;
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

			mbase::deep_char_stream dcs(oldGgufFile.get_file_size() - mOldMetaSize);
			oldGgufFile.read_data(dcs);
			oldGgufFile.close_file();

			newGgufFile.open_file(modifiedFileName, mbase::io_file::access_mode::RW_ACCESS, mbase::io_file::disposition::APPEND);
			newGgufFile.write_data(dcs.get_buffer(), dcs.buffer_length());
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