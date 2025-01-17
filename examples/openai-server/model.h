#ifndef MBASE_OPENAI_MODEL_H
#define MBASE_OPENAI_MODEL_H

#include <mbase/common.h>
#include <mbase/inference/inf_common.h>
#include <mbase/inference/inf_t2t_model.h>
#include <mbase/vector.h>
#include <mbase/synchronization.h>

MBASE_BEGIN

class OpenaiTextToTextProcessor;

class OpenaiModel : public mbase::InfModelTextToText {
public:

    OpenaiModel();

    enum init_proc_err {
        PROC_SUCCESS,
        ERR_INVALID_PROCESSOR_COUNT,
        ERR_INVALID_THREAD_COUNT,
        ERR_INVALID_BATCH_THREAD_COUNT,
        ERR_INVALID_CONTEXT_LENGTH,
        ERR_INVALID_BATCH_LENGTH,
    };
    U64 get_creation_date_in_epoch();
    init_proc_err initialize_t2t_processors(
        const U32& in_processor_count,
        const U32& in_thread_count,
        const U32& in_batch_thread_count,
        const U32& in_context_length,
        const U32& in_batch_length,
        const mbase::wstring& in_sys_prompt_file = wstring()
    );
    bool acquire_processor(OpenaiTextToTextProcessor*& out_processor);
    GENERIC release_processor(OpenaiTextToTextProcessor* in_processor);

    GENERIC on_initialize_fail(init_fail_code out_fail_code) override;
	GENERIC on_initialize() override;
	GENERIC on_destroy() override;

private:
    mbase::mutex mProcDistributionSync;
    mbase::vector<OpenaiTextToTextProcessor*> mAvailableT2tProcessors; // will leak memory but its okay.
    U32 mAccessLimit;
    U64 mCreationDate;
};

MBASE_END

#endif // MBASE_OPENAI_MODEL_H