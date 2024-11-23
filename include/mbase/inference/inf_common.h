#ifndef MBASE_INF_COMMON_H
#define MBASE_INF_COMMON_H

#include <mbase/common.h>
#include <mbase/vector.h>
#include <mbase/framework/object_watcher.h>

// If llama.cpp is defined macro should be placed here
#include <llama.h>

MBASE_BEGIN

using inf_text_token = llama_token;
using inf_text_token_vector = mbase::vector<inf_text_token>;
using inf_embedding_vector = mbase::vector<F32>;

template<typename TargetObject>
struct inf_processor_watcher {
    mbase::list<inf_processor_watcher<TargetObject>>::iterator mItSelf;
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

// The rest is common functionality inspired from llama.cpp common library for examples
// The set of functions will be populated as new needs are found

GENERIC inf_common_batch_add(
    llama_batch& in_batch,
    inf_text_token in_id,
    I32 in_pos,
    const mbase::vector<llama_seq_id>& in_seq_ids,
    bool in_logits
);

GENERIC inf_common_embd_normalize(
    const PTRF32 in_inp, 
    PTRF32 out_normalized, 
    const SIZE_T& in_n
);


MBASE_END

#endif // MBASE_INF_COMMON_H