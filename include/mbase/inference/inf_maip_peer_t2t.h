#ifndef MBASE_INF_MAIP_PEER_T2T_H
#define MBASE_INF_MAIP_PEER_T2T_H

#include <mbase/inference/inf_maip_peer_base.h>
#include <mbase/inference/inf_t2t_client.h>

MBASE_BEGIN

class InfMaipTextToTextProcessor;
class InfMaipPeerTextToText;

class MBASE_API InfMaipTextToTextProcessor : public mbase::InfTextToTextProcessor {
public:
    InfMaipTextToTextProcessor(InfMaipPeerTextToText* in_peer);
    GENERIC on_initialize_fail(last_fail_code out_code) override;
	GENERIC on_initialize() override;
	GENERIC on_destroy() override;
private:
    InfMaipPeerTextToText* mClientNominee;
};

class MBASE_API InfMaipPeerTextToText : public mbase::InfMaipPeerBase, public mbase::InfClientTextToText {
public:
    InfMaipPeerTextToText(std::shared_ptr<mbase::PcNetPeerClient> in_peer, const mbase::string& in_maip_user);
    ~InfMaipPeerTextToText();
    GENERIC on_init_fail(InfProcessorBase* out_processor, InfTextToTextProcessor::last_fail_code out_fail_code);
    GENERIC on_register(InfProcessorBase* out_processor) override;
	GENERIC on_unregister(InfProcessorBase* out_processor) override; // This function is a one-liner which will only call remove_processor_by_address and then delete the processor
    GENERIC on_write(InfTextToTextProcessor* out_processor, const inf_text_token_vector& out_token, bool out_is_finish) override;
	GENERIC on_finish(InfTextToTextProcessor* out_processor, size_type out_total_token_size, InfTextToTextProcessor::finish_state out_finish_state) override;
private:
    inf_token_description mLastToken;
};

MBASE_END

#endif // MBASE_INF_MAIP_PEER_T2T_H