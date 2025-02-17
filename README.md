# MBASE

<div align="center">
  <a href="" />
    <img alt="MBASE" height="325px" src="https://github.com/user-attachments/assets/89d4b07a-1540-442f-9d86-5e105424ed70">
  </a>
</div>
<br>
MBASE inference library is a high-level non-blocking LLM inference library written on top
of the [llama.cpp](https://github.com/ggml-org/llama.cpp) library to provide the necessary tools and APIs to allow developers
to integrate popular LLMs into their application with minimal performance loss and development time.

The inference SDK will allow developers the utilize the LLMs and create their own solutions,
tooling mechanisms etc.

When the phrase "local LLM inference" is thrown around,
it usually means that hosting an Openai API compatible HTTP server and using the completions API locally.
The MBASE inference library is expected to change this notion by providing you the LLM inference capability 
through its low-level objects and procedures so that 
you can integrate and embedd LLM into your high-performance application such as games, server applications and many more.

Also you still have the option of hosting Openai server using the [mbase_openai_server](doc_link) program or you can
code a similar one yourself!

There also is a benchmarking tool developed for testing the performance of the LLM but more specifically,
the impact of the inference operation on your main program loop. For further details refer to [benchmark](doc_link) documentation.

## Features

- Non-blocking TextToText LLM inference SDK.
- Non-blocking Embedder model inference SDK.
- GGUF file meta-data manipulation SDK.
- Openai server program supporting both TextToText and Embedder endpoints with system prompt caching support which implies significant performance boost.
- Hosting multiple models in a single Openai server program.
- Using [llama.cpp](https://github.com/ggml-org/llama.cpp) as an inference backend so that models that are supported by the [llama.cpp](https://github.com/ggml-org/llama.cpp) library are supported by default.
- Benchmark application for measuring the impact of LLM inference on your application.
- Plus anything [llama.cpp](https://github.com/ggml-org/llama.cpp) supports.

## Implementation Matrix

| Type       | SDK Support | Openai API Support | Engine                                             |
| ---------- | ----------- | ------------------ | -------------------------------------------------- |
| TextToText | ✅          | ✅                 | [llama.cpp](https://github.com/ggml-org/llama.cpp) |
| Embedder   | ✅          | ✅                 | [llama.cpp](https://github.com/ggml-org/llama.cpp) |

## Supported Platform

- Mac OS
- Linux
- Windows

## Documentation

Detailed MBASE SDK documentation can be found here.

## Download and Setting up

Download page: quickstart/setup/download

SDK setup and compiling from source: quickstart/setup/setting_up

## Useful Programs

### Openai Server

Detailed documentation: programs/openai-server/about

An Openai API compatible HTTP/HTTPS server for serving LLMs.
This program provides chat completion API for TextToText models and embeddings API For embedder models.

**Usage:**

```bash
mbase_openai_server *[option [value]]
mbase_openai_server --hostname "127.0.0.1" -jsdesc description.json
mbase_openai_server --hostname "127.0.0.1" --port 8080 -jsdesc description.json
mbase_openai_server --hostname "127.0.0.1" --port 8080 -jsdesc description.json
mbase_openai_server --hostname "127.0.0.1" --port 8080 --ssl-pub public_key_file --ssl-key private_key_file -jsdesc description.json
```

### Benchmark T2T

Detailed documentation: programs/benchmark-t2t/about

It is a program written to measure the performance
of the given T2T LLM and its impact on your main application logic.

**Usage:**

```bash
mbase_benchmark_t2t model_path *[option [value]]
mbase_benchmark_t2t model.gguf -uc 1 -fps 500 -jout .
mbase_benchmark_t2t model.gguf -uc 1 -fps 500 -jout . -mdout .
```

### Embedding

Detailed documentation: programs/embedding/about

An example program for generating the embeddings of the given prompt or prompts.

**Usage:**

```bash
mbase_embedding_simple model_path *[option [value]]
mbase_embedding_simple model.gguf -gl 80 -p 'What is life?'
mbase_embedding_simple model.gguf -gl 80 -pf prompt1.txt -pf prompt2.txt
```

### Retrieval

Detailed documentation: programs/embedding/about

An example program for generating the embeddings of the given prompt or prompts.

**Usage:**

```bash
mbase_retrieval model_path *[option [value]]
mbase_retrieval model.gguf -q 'What is MBASE' -pf file1.txt -pf file2.txt -gl 80
```

### Simple Conversation

Detailed documentation: programs/simple-conversation/about

It is a simple executable program where you are having
a dialogue with the LLM you provide. It is useful for examining the answer of the LLM since the system prompt 
and sampler values can be altered.

**Usage:**

```bash
mbase_simple_conversation model_path *[option [value]]
mbase_simple_conversation model.gguf
mbase_simple_conversation model.gguf -gl 80
mbase_simple_conversation model.gguf -gl 80 -sys 'You are a helpful assistant.'
```

### Typo Fixer

Detailed documentation: programs/typo-fixer/about

This is an applied example use case of the MBASE library.
The program is reading a user-supplied text file and fixing the typos.

```bash
mbase_typo_fixer model_path *[option [value]]
mbase_typo_fixer model.gguf -gl 80 -s typo.txt -o fixed.txt
```

## SDK Usage Examples

- **Single-Prompt Example**: Simple prompt and answer example. At the end of the example, the user will supply a prompt in the terminal and LLM will give the response.
- **Dialogue-Example**: More complex dialogue based prompt and answer example. At the end of the example, the user will be able to have a dialogue with LLM using terminal.
- **Embedding Example**: Vector embedding generator which is generally used by RAG programs and more. At the end of the example, the user will supply an input and vector embeddings will be generated by using embedder LLM model.

## Finding the SDK

Detailed documentation: SDK_STRUCTURE_DOC

If you have installed the MBASE SDK, you can find the library using CMake find_package function with components specification.

In order to find the library using cmake, write the following:

```cmake
find_package(mbase.libs REQUIRED COMPONENTS inference)
```

This will find the inference SDK. In order to link both include directories and link libraries, write the following:

```cmake
target_compile_features(<your_target> PUBLIC cxx_std_17)
target_link_libraries(<your_target> PRIVATE mbase-inference)
target_include_directories(<your_target> PUBLIC mbase-inference)
```

## GGUF, Displaying General Metadata

Detailed documentation: ABOUT GGUF FILES

```cpp
#include <mbase/inference/inf_gguf_metadata_configurator.h>
#include <iostream>

int main()
{
    mbase::GgufMetaConfigurator metaConfigurator(L"<path_to_model>");

    if(!metaConfigurator.is_open())
    {
        std::cout << "Unable to open gguf file." << std::endl;
        return 1;
    }

    mbase::string modelArchitecture;
    mbase::string modelName;
    mbase::string modelAuthor;
    mbase::string modelVersion;
    mbase::string modelOrganization;
    mbase::string modelSizeLabel;
    mbase::string modelLicense;
    mbase::string modelLicenseName;
    mbase::string modelLicenseLink;
    mbase::string modelUuid;
    uint32_t modelFileType;

    metaConfigurator.get_key("general.architecture", modelArchitecture);
    metaConfigurator.get_key("general.name", modelName);
    metaConfigurator.get_key("general.author", modelAuthor);
    metaConfigurator.get_key("general.version", modelVersion);
    metaConfigurator.get_key("general.organization", modelOrganization);
    metaConfigurator.get_key("general.size_label", modelSizeLabel);
    metaConfigurator.get_key("general.license", modelLicense);
    metaConfigurator.get_key("general.license.name", modelLicenseName);
    metaConfigurator.get_key("general.license.link", modelLicenseLink);
    metaConfigurator.get_key("general.uuid", modelUuid);
    metaConfigurator.get_key("general.file_type", modelFileType);

    std::cout << "Architecture: " << modelArchitecture << std::endl;
    std::cout << "Name: " << modelName << std::endl;
    std::cout << "Author: " << modelAuthor << std::endl;
    std::cout << "Version: " << modelVersion << std::endl;
    std::cout << "Organization: " << modelOrganization << std::endl;
    std::cout << "Size label: " << modelSizeLabel << std::endl;
    std::cout << "License: " << modelLicense << std::endl;
    std::cout << "License name: " << modelLicenseName << std::endl;
    std::cout << "License link: " << modelLicenseLink << std::endl;
    std::cout << "Model UUID: " << modelUuid << std::endl;
    std::cout << "File type: " << modelFileType << std::endl;

    return 0;
}
```
