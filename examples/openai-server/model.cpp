#include "model.h"
#include "processor.h"

MBASE_BEGIN

OpenaiModel::OpenaiModel()
{
    mCreationDate = static_cast<U64>(time(NULL));
}

U64 OpenaiModel::get_creation_date_in_epoch()
{
    return mCreationDate;
}

OpenaiModel::init_proc_err OpenaiModel::initialize_t2t_processors(
        const U32& in_processor_count,
        const U32& in_thread_count,
        const U32& in_batch_thread_count,
        const U32& in_context_length,
        const U32& in_batch_length,
        const mbase::wstring& in_sys_prompt_file
)
{
    if(!in_processor_count)
    {
        return OpenaiModel::init_proc_err::ERR_INVALID_PROCESSOR_COUNT;
    }

    if(!in_thread_count)
    {
        return OpenaiModel::init_proc_err::ERR_INVALID_THREAD_COUNT;
    }

    if(!in_batch_thread_count)
    {
        return OpenaiModel::init_proc_err::ERR_INVALID_BATCH_THREAD_COUNT;
    }

    if(!in_context_length)
    {
        return OpenaiModel::init_proc_err::ERR_INVALID_CONTEXT_LENGTH;
    }

    if(!in_batch_length)
    {
        return OpenaiModel::init_proc_err::ERR_INVALID_BATCH_LENGTH;
    }

    for(U32 i = 0; i < in_processor_count; i++)
    {
        OpenaiTextToTextProcessor* newProcessor = new OpenaiTextToTextProcessor;
        
        if(in_sys_prompt_file.size())
        {
            newProcessor->set_manual_caching(true, mbase::InfProcessorTextToText::cache_mode::KV_LOCK_MODE);
        }

        this->register_context_process(
            newProcessor,
            in_context_length,
            in_batch_length,
            in_thread_count,
            in_batch_thread_count,
            true,
            {}
        );
        mAvailableT2tProcessors.push_back(newProcessor);
    }

    return OpenaiModel::init_proc_err::PROC_SUCCESS;
}

bool OpenaiModel::acquire_processor(OpenaiTextToTextProcessor*& out_processor)
{
    mbase::lock_guard lockGuard(mProcDistributionSync);

    mbase::vector<OpenaiTextToTextProcessor*>::iterator It = mAvailableT2tProcessors.begin();
    if(It == mAvailableT2tProcessors.end())
    {
        return false;
    }

    out_processor = *It;
    mAvailableT2tProcessors.erase(It);

    return true;
}

GENERIC OpenaiModel::release_processor(OpenaiTextToTextProcessor* in_processor)
{
    mbase::lock_guard lockGuard(mProcDistributionSync);
    
    mAvailableT2tProcessors.push_back(in_processor);
}

GENERIC OpenaiModel::on_initialize_fail(init_fail_code out_fail_code)
{

}

GENERIC OpenaiModel::on_initialize()
{

}

GENERIC OpenaiModel::on_destroy()
{

}

MBASE_END