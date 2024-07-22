#ifndef MBASE_INF_MODEL_H
#define MBASE_INF_MODEL_H

#include <mbase/common.h>
#include <mbase/string.h>
#include <mbase/vector.h>
#include <llama.h>

MBASE_BEGIN

class InfModel;

class InfModel {
public:
	using inf_token = llama_token;

	enum class flags : U8 {
		INF_MODEL_SUCCESS,
		INF_MODEL_ERR_CANT_LOAD_MODEL,
		INF_MODEL_ERR_MISSING_MODEL,
		INF_MODEL_ERR_NO_SENTENCE,
		INF_MODEL_ERR_MISSING_PROCESSOR
	};

	InfModel();
	~InfModel();

	bool is_initialized() const;
	llama_model* get_raw_model();
	flags get_vocabulary_type(mbase::string& out_type);
	flags get_rope_type(mbase::string& out_type);
	flags get_vocab_count(I32& out_count);
	flags get_model_param_count(U32& out_count);
	flags get_size(U32& out_size);
	flags get_bof_sentence(mbase::string& out_sentence);
	flags get_eof_sentence(mbase::string& out_sentence);
	flags get_classification_sentence(mbase::string& out_sentence);
	flags get_sentence_seperator(mbase::string& out_sentence);
	flags get_next_line(mbase::string& out_sentence);
	flags is_token_eof_generation(inf_token in_token);
	flags is_token_control(inf_token in_token);
	flags get_metadata_count(U32& out_count);
	
	flags load_model(const mbase::string& in_model);
	flags unload_model();

private:
	bool mIsLoaded;
	llama_model* mLoadedModel;
};

MBASE_END

#endif // !MBASE_INF_MODEL_H
