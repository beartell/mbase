#ifndef MBASE_INF_COMMON_H
#define MBASE_INF_COMMON_H

#include <mbase/common.h>
#include <mbase/vector.h>
#include <mbase/unordered_map.h>
#include <mbase/framework/object_watcher.h>

// If llama.cpp is defined macro should be placed here
#include <llama.h>

#define MBASE_INFERENCE_SYS_STRING "MBASE Inference"
#define MBASE_INFERENCE_LIB_NAME "inference"
#define MBASE_INFERENCE_SYS_VERSION "1.0.0alpha"

MBASE_BEGIN

using inf_text_token = llama_token;
using inf_text_token_vector = mbase::vector<inf_text_token>;
using inf_embedding_vector = mbase::vector<F32>;

template<typename TargetObject>
struct inf_processor_watcher {
    typename mbase::list<inf_processor_watcher<TargetObject>>::iterator mItSelf;
    TargetObject* mSubject = NULL;
    U32 mContextLength = 0;
};

struct inf_token_description {
    mbase::string mTokenString = "";
    bool mIsSpecial = false;
};

enum class inf_model_category {
    TEXT_TO_TEXT,
    EMBEDDING,
    UNDEFINED
};

struct MBASE_API inf_lora_adapter {
    friend bool operator!=(const inf_lora_adapter& in_lhs, const inf_lora_adapter& in_rhs)
    {
        return in_lhs.mAdapterName != in_rhs.mAdapterName;
    }

    friend bool operator==(const inf_lora_adapter& in_lhs, const inf_lora_adapter& in_rhs)
    {
        return in_lhs.mAdapterName == in_rhs.mAdapterName;
    }

	mbase::string mAdapterName = "";
	mbase::wstring mLoraPath = L"";
	llama_adapter_lora* mAdapterHandle = NULL;
};

using lora_adapter_map = mbase::unordered_map<mbase::string, inf_lora_adapter>;

// The rest is common functionality inspired from llama.cpp common library for examples
// The set of functions will be populated as new needs are found

MBASE_API GENERIC inf_common_batch_add(
    llama_batch& in_batch,
    inf_text_token in_id,
    I32 in_pos,
    const mbase::vector<llama_seq_id>& in_seq_ids,
    bool in_logits
);

MBASE_API GENERIC inf_common_embd_normalize(
    const PTRF32 in_inp, 
    PTRF32 out_normalized, 
    const SIZE_T& in_n
);

MBASE_API F32 inf_common_cosine_similarity(
    const PTRF32 in_data1,
    const PTRF32 in_data2,
    const I32& in_length
);

MBASE_API mbase::string inf_get_sys_name_total();

MBASE_END

#endif // MBASE_INF_COMMON_H
