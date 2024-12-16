#include <mbase/inference/inf_embedder.h>
#include <mbase/inference/inf_embedder_client.h>
#include <mbase/inference/inf_t2t_model.h>
#include <iostream>

#define MBASE_BENCHMARK_VERSION "v1.4.0"

using namespace mbase;

class EmbedderModel;
class EmbedderProcessor;
class EmbedderClient;

class EmbedderModel : public InfModelTextToText {
public:
    GENERIC on_initialize_fail(init_fail_code out_fail_code) override{}
    GENERIC on_initialize() override{}
    GENERIC on_destroy() override{}
private:
};

class EmbedderProcessor : public InfEmbedderProcessor {
public:

private:
};

int main()
{
    return 0;
}