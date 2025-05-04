#include "processor.h"
#include "global_state.h"

MBASE_BEGIN

OpenaiTextToTextProcessor::OpenaiTextToTextProcessor(const I32& in_processor_index)
{
    mProcessorIndex = in_processor_index;
}

OpenaiTextToTextProcessor::~OpenaiTextToTextProcessor()
{
    this->release_inference_client_stacked();
}

GENERIC OpenaiTextToTextProcessor::on_initializing()
{
    
}

GENERIC OpenaiTextToTextProcessor::on_initialize_fail(last_fail_code out_code)
{
    std::cout << "ERR: TextToText processor " << mProcessorIndex <<" failed to initialize." << std::endl;
    std::cout << "INFO: Make sure you have enough memory for such operation." << std::endl;
    exit(1);
}

GENERIC OpenaiTextToTextProcessor::on_initialize()
{
    printf("TextToText processor %d is initialized\n", mProcessorIndex);
    OpenaiModel* tmpModel = static_cast<OpenaiModel*>(this->get_processed_model());
    tmpModel->_incr_processor_count();
    this->set_inference_client(&mTextClient);
}

GENERIC OpenaiTextToTextProcessor::on_destroy()
{

}

OpenaiEmbedderProcessor::OpenaiEmbedderProcessor(const I32& in_processor_index)
{
    mProcessorIndex = in_processor_index;
}

OpenaiEmbedderProcessor::~OpenaiEmbedderProcessor()
{
    this->release_inference_client_stacked();
}

GENERIC OpenaiEmbedderProcessor::on_initializing()
{

}

GENERIC OpenaiEmbedderProcessor::on_initialize_fail(last_fail_code out_code)
{
    std::cout << "ERR: Embedder processor " << mProcessorIndex <<" failed to initialize." << std::endl;
    std::cout << "INFO: Make sure you have enough memory for such operation." << std::endl;
    exit(1);
}

GENERIC OpenaiEmbedderProcessor::on_initialize()
{
    printf("Embedder processor %d is initialized\n", mProcessorIndex);
    OpenaiModel* tmpModel = static_cast<OpenaiModel*>(this->get_processed_model());
    tmpModel->_incr_processor_count();
    this->set_inference_client(&mEmbedderClient);
}

GENERIC OpenaiEmbedderProcessor::on_destroy()
{

}


MBASE_END