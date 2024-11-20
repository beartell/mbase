#include <mbase/inference/inf_common.h>
#include <cmath>

MBASE_BEGIN

GENERIC inf_common_batch_add(
    llama_batch& in_batch,
    inf_text_token in_id,
    I32 in_pos,
    const mbase::vector<llama_seq_id>& in_seq_ids,
    bool in_logits
)
{
    // No need for assertion because if this function is called, inputs are guaranteed to be well aligned and valid
    in_batch.token[in_batch.n_tokens] = in_id;
    in_batch.pos[in_batch.n_tokens] = in_pos;
    in_batch.n_seq_id[in_batch.n_tokens] = in_seq_ids.size();
    for(SIZE_T i = 0; i < in_seq_ids.size(); ++i)
    {
        in_batch.seq_id[in_batch.n_tokens][i] = in_seq_ids[i];
    }

    in_batch.logits[in_batch.n_tokens] = in_logits;
    
    in_batch.n_tokens++;
}

GENERIC inf_common_embd_normalize(
    const PTRF32 in_inp, 
    PTRF32 out_normalized, 
    const SIZE_T& in_n
)
{
    F64 tmpSum = 0.0;
    for(SIZE_T i = 0; i < in_n; i++)
    {
        tmpSum += in_inp[i] * in_inp[i];
    }
    tmpSum = std::sqrt(tmpSum);

    const F32 tmpNorm = tmpSum > 0.0 ? 1.0 / tmpSum : 0.0f;

    for(SIZE_T i = 0; i < in_n; i++)
    {
        out_normalized[i] = in_inp[i] * tmpNorm;
    }
}

MBASE_END
