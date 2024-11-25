#ifndef MBASE_INF_EMBEDDER_CLIENT_H
#define MBASE_INF_EMBEDDER_CLIENT_H

#include <mbase/inference/inf_common.h>
#include <mbase/inference/inf_client.h>
#include <mbase/inference/inf_embedder.h>

MBASE_BEGIN

// Not implemented yet
class MBASE_API InfClientEmbedder : public InfClientBase {
public:
    using embedding_input_vector = mbase::vector<mbase::string>;
    
    enum flags : U8 {
        INF_CLIENT_SUCCESS,
        INF_CLIENT_ERR_MISSING_INPUT
    };

    bool is_input_set() const noexcept;
    const embedding_input_vector& get_embedding_input_vector() const noexcept;
    flags add_input(const mbase::string& in_data);
    flags add_input(const mbase::wstring& in_data);
    flags add_input(CBYTEBUFFER in_data, const size_type& in_size);
    flags set_input_vector(const embedding_input_vector& in_vector);
    flags set_input_vector(embedding_input_vector&& in_vector);

    virtual GENERIC on_write(InfEmbedderProcessor* out_processor, const inf_embedding_vector& out_embeddings, const I32& out_embedding_index, bool out_is_finished);
    virtual GENERIC on_finish(InfEmbedderProcessor* out_processor, const size_type& out_total_processed_embeddings);

private:
    embedding_input_vector mInputVector;
};

MBASE_END

#endif // !MBASE_INF_EMBEDDER_CLIENT_H