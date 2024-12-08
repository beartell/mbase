#ifndef MBASE_INF_EMBEDDER_CLIENT_H
#define MBASE_INF_EMBEDDER_CLIENT_H

#include <mbase/inference/inf_common.h>
#include <mbase/inference/inf_client.h>
#include <mbase/inference/inf_embedder.h>

MBASE_BEGIN

// Not implemented yet
class MBASE_API InfClientEmbedder : public InfClientBase {
public:
    virtual GENERIC on_write(InfEmbedderProcessor* out_processor, const mbase::vector<inf_embedding_vector>& out_embeddings, bool out_is_finished) = 0;
    virtual GENERIC on_finish(InfEmbedderProcessor* out_processor, const size_type& out_total_processed_embeddings) = 0;
};

MBASE_END

#endif // !MBASE_INF_EMBEDDER_CLIENT_H