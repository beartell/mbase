#include "processor.h"

MBASE_BEGIN

OpenaiTextToTextProcessor::OpenaiTextToTextProcessor()
{

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
    std::cout << "ERR: Processor failed to initialize." << std::endl;
    std::cout << "INFO: Make sure you have enough memory for such operation." << std::endl;
    exit(1);
}

GENERIC OpenaiTextToTextProcessor::on_initialize()
{
    this->set_inference_client(&mTextClient);
}

GENERIC OpenaiTextToTextProcessor::on_destroy()
{

}

MBASE_END