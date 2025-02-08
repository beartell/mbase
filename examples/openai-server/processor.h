#ifndef MBASE_OPENAI_PROCESSOR_H
#define MBASE_OPENAI_PROCESSOR_H

#include <mbase/common.h>
#include <mbase/inference/inf_common.h>
#include <mbase/inference/inf_t2t_processor.h>
#include <mbase/inference/inf_embedder.h>
#include "client.h"

MBASE_BEGIN

class OpenaiTextToTextProcessor : public mbase::InfProcessorTextToText {
public:
    OpenaiTextToTextProcessor(const I32& in_processor_index);
    ~OpenaiTextToTextProcessor();

    GENERIC on_initializing() override;
	GENERIC on_initialize_fail(last_fail_code out_code) override;
	GENERIC on_initialize() override;
	GENERIC on_destroy() override;

private:
    I32 mProcessorIndex = -1;
    OpenaiTextToTextClient mTextClient;
};

class OpenaiEmbedderProcessor : public mbase::InfEmbedderProcessor {
public:
    OpenaiEmbedderProcessor(const I32& in_processor_index);
    ~OpenaiEmbedderProcessor();

    GENERIC on_initializing() override;
	GENERIC on_initialize_fail(last_fail_code out_code) override;
	GENERIC on_initialize() override;
	GENERIC on_destroy() override;

private:
    I32 mProcessorIndex = -1;
    OpenaiEmbedderClient mEmbedderClient;
};

MBASE_END

#endif // MBASE_OPENAI_PROCESSOR_H