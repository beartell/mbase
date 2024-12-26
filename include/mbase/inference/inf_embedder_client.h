#ifndef MBASE_INF_EMBEDDER_CLIENT_H
#define MBASE_INF_EMBEDDER_CLIENT_H

#include <mbase/inference/inf_common.h>
#include <mbase/inference/inf_client.h>
#include <mbase/inference/inf_embedder.h>

MBASE_BEGIN

class MBASE_API InfClientEmbedder : public InfClientBase {
public:
    /* ===== INTERFACE METHODS BEGIN ===== */
    virtual GENERIC on_batch_processed(InfEmbedderProcessor* out_processor, const U32& out_proc_batch_length) = 0;
    virtual GENERIC on_write(InfEmbedderProcessor* out_processor, PTRF32 out_embeddings, const U32& out_cursor, bool out_is_finished) = 0;
    virtual GENERIC on_finish(InfEmbedderProcessor* out_processor, const size_type& out_total_processed_embeddings) = 0;
    /* ===== INTERFACE METHODS END ===== */
};

MBASE_END

#endif // !MBASE_INF_EMBEDDER_CLIENT_H