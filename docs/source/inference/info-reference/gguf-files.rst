.. include:: ../links.rst

================
About GGUF Files
================

.. important::

    This chapter talks briefly about GGUF the amount that is necessary
    enough to understand and use the inference SDK. For super-detailed explanation,
    refer to the original GGUF specification that is documented under the `ggml <ggml_>`_ project.

    `GGUF Specification <https://github.com/ggerganov/ggml/blob/master/docs/gguf.md>`_

    `llama.cpp wiki <https://en.wikipedia.org/wiki/Llama.cpp#GGUF_file_format>`_

GGUF which stands for "GGML Universal File"[#r1]_, is a binary file format 
which is developed by the `Georgi Gerganov <https://github.com/ggerganov>`_ and the community.
It is designed to store both metadata information about the model such
as context length, vocabulary, architecture etc. and its tensors
in a single file with fast loading and saving of models, and for ease of reading. [#r2]_

It is the only format the underlying `llama.cpp <llama.cpp_>`_ inference backend supports.

Here is the file format image taken from the `GGUF specification <https://github.com/ggerganov/ggml/blob/master/docs/gguf.md#file-structure>`_: 

.. image:: images/gguf_file_content.png

As you can see that the model metadatas is written at the beginning of the file and after the metadatas,
the model tensors are laid out.

--------------------
Quantization Support
--------------------

GGUF file supports multiple quantization formats ranging from 2-bit to 8-bit as well as common float formats such as
f16, f32, bf16. [#r3]_

.. tip::

    The quantizations gguf file supports can be observed by looking at at the `llama.h file <https://github.com/ggerganov/llama.cpp/blob/master/include/llama.h>`_ file which
    has a :code:`llama_ftype` enum declared for that. Or you can look at the `Huggingface GGUF documentation <https://huggingface.co/docs/hub/gguf#quantization-types>`_.

-------------------
Important Metadatas 
-------------------

There are some important metadatas that is worth mentioning.

.. important::

    Calling the keys below important is a subjective opinion since that some keys
    may exhibit more importance based-off of the scenario they are being interpreted.

    Since the MBASE inference library targets developers who want to integrate LLMs into their
    programs but don't have a in-depth knowledge of ML, AI, data-science etc. We listed the keys that are necessary
    for the mere users of LLMs.

    If you want to see the full list of metadatas and their descriptions, refer to
    `GGUF specification <https://github.com/ggerganov/ggml/blob/master/docs/gguf.md#standardized-key-value-pairs>`_

^^^^^^^^^^^^^^^^
General Metadata
^^^^^^^^^^^^^^^^

Here is a list of some important general metadata key/values :

* :code:`general.architecture : string`: Describes what architecture the model implements.
* :code:`general.name : string`: A human-readable name of the model. It is expected to be unique within the community that the model is defined in.
* :code:`general.author : string`: Author of the model.
* :code:`general.version : string`: Version of the model.
* :code:`general.organization : string`: Organization of the model.
* :code:`general.size_label : string`: The size label of the model. For example "3B", "70B" etc.
* :code:`general.license : string`: Licence of the model.
* :code:`general.license.name : string`: Humand readable licence name.
* :code:`general.license.link : string`: URL to the licence.
* :code:`general.uuid : string`: Model file UUID.
* :code:`general.file_type : uint32`: An integer value describing the type of the majority of the tensors in the file.

and many more.

^^^^^^^^^^^^^^^^^^^^^^
Architectural Metadata
^^^^^^^^^^^^^^^^^^^^^^

Model's architectural metadatas are prefixed by the model's architecture name.
For example, if the :code:`general.architecture` is :code:`llama`, architectural keys
are written as: :code:`llama.context_length`, :code:`llama.embedding_length` etc.

So from now, assume the :code:`[llm]` is the architecture name, you need to substitute it with the value of
:code:`general.architecture` to get architectural metadata. 

Here is a list of some important architectural metadata key/values:

* :code:`[llm].context_length : uint32`: The context length that the model was trained on. The inference engine still allows you to do inference on context sizes greater than this value but the result may be poor in quality.
* :code:`[llm].embedding_length : uint32`: Embedding layer size. In user terms, length of the embedding vector.
* :code:`[llm].block_count : uint32`: The number of blocks of attention+feed-forward layers. It may be useful for developer to calculate how much memory he/she needs to create a context in length N.

and many more.

.. _gguf-convert:

------------------
Converting to GGUF
------------------

In this section, we will cover how to download a non GGUF model from the `Huggingface <https://huggingface.co/>`_ website
and we will convert that model into gguf format with proper naming. 

The model conversion process is generally as follows:

1. Acquire the model from its `Huggingface <https://huggingface.co/>`_ repository.
2. Use the conversion script :code:`convert_hf_to_gguf.py` that is provided by the `llama.cpp <llama.cpp_>`_ library.
3. Quantize the model using :code:`llama-quantize` that is provided by the `llama.cpp <llama.cpp_>`_ library.
4. You are good to go!

Our case study model will be: `microsoft/Phi-3-mini-128k-instruct <https://huggingface.co/microsoft/Phi-3-mini-128k-instruct>`_

^^^^^^^^^^^^^^^^^^^
Acquiring the Model
^^^^^^^^^^^^^^^^^^^

.. important::

    Since `Huggingface <https://huggingface.co/>`_ store models as git repositories and those model
    files are great in size, you need to have the :code:`git-lfs` installed on your system before proceeding further.

    The :code:`git-lfs` installation steps can be found here https://github.com/git-lfs/git-lfs/blob/main/INSTALLING.md

    Another important thing is that since we will use the model conversion script that is provided
    by the `llama.cpp <llama.cpp_>`_ library, you should have the clone of `llama.cpp <llama.cpp_>`_ repository
    because the conversion scripts and requirements.txt reside under that repository.

Since models are stored in git repositories in `Huggingface <https://huggingface.co/>`_, you first need to clone the repository.
Now, in your working directory, clone the `microsoft/Phi-3-mini-128k-instruct <https://huggingface.co/microsoft/Phi-3-mini-128k-instruct>`_ repository:

.. code-block:: bash

    git clone https://huggingface.co/microsoft/Phi-3-mini-128k-instruct

Now, go into that directory and make sure that the model is downloaded successfully.
After the download is finished, we will apply the conversion script.

^^^^^^^^^^^^^^^^^^^^^^^^^^^
Using the Conversion Script
^^^^^^^^^^^^^^^^^^^^^^^^^^^

First, clone the `llama.cpp <llama.cpp_>`_ repository:

.. code-block:: bash

    git clone https://github.com/ggerganov/llama.cpp.git

Then, you need to install the necessary python packages:

.. code-block:: bash

    pip install -r llama.cpp/requirements.txt

After the packages are installed, we can call the conversion script. Here is the usage output of the conversion script:

.. code-block:: bash

    usage: convert_hf_to_gguf.py [-h] [--vocab-only] [--outfile OUTFILE] [--outtype {f32,f16,bf16,q8_0,tq1_0,tq2_0,auto}] [--bigendian] 
    [--use-temp-file] [--no-lazy] [--model-name MODEL_NAME] [--verbose] 
    [--split-max-tensors SPLIT_MAX_TENSORS] [--split-max-size SPLIT_MAX_SIZE] [--dry-run] 
    [--no-tensor-first-split] [--metadata METADATA] [--print-supported-models] 
    [model]

Based-off of the usage information, we will call the conversion script to convert to GGUF format with f16 quantization method:

.. code-block:: bash

    python llama.cpp/convert_hf_to_gguf.py --outfile Phi-3-mini-128k-instruct-f16.gguf --outtype f16 ./Phi-3-mini-128k-instruct

After the conversion is complete, you can quantize the :code:`Phi-3-mini-128k-instruct-f16.gguf` to Q4 using the :code:`llama-quantize` executable.

Here is the usage output of the quantization program:

.. code-block:: bash

    usage: llama-quantize [--help] [--allow-requantize] [--leave-output-tensor] [--pure] [--imatrix] [--include-weights] 
    [--exclude-weights] [--output-tensor-type] [--token-embedding-type] 
    [--override-kv] model-f32.gguf [model-quant.gguf] type [nthreads]

Based-off of the usage information, we will call the quantization program as follows:

.. code-block:: bash

    llama-quantize Phi-3-mini-128k-instruct-f16.gguf Phi-3-mini-128k-instruct-Q4_0.gguf Q4_0 8

This program will quantize your f16 quantization model into Q4_0 quantization model and the number '8' at the end of the line
tells the program how many threads to use for quantization.

The quantization program states that the following quantization methods are possible:

.. code-block:: bash

    Allowed quantization types:
    2  or  Q4_0    :  4.34G, +0.4685 ppl @ Llama-3-8B
    3  or  Q4_1    :  4.78G, +0.4511 ppl @ Llama-3-8B
    8  or  Q5_0    :  5.21G, +0.1316 ppl @ Llama-3-8B
    9  or  Q5_1    :  5.65G, +0.1062 ppl @ Llama-3-8B
    19  or  IQ2_XXS :  2.06 bpw quantization
    20  or  IQ2_XS  :  2.31 bpw quantization
    28  or  IQ2_S   :  2.5  bpw quantization
    29  or  IQ2_M   :  2.7  bpw quantization
    24  or  IQ1_S   :  1.56 bpw quantization
    31  or  IQ1_M   :  1.75 bpw quantization
    36  or  TQ1_0   :  1.69 bpw ternarization
    37  or  TQ2_0   :  2.06 bpw ternarization
    10  or  Q2_K    :  2.96G, +3.5199 ppl @ Llama-3-8B
    21  or  Q2_K_S  :  2.96G, +3.1836 ppl @ Llama-3-8B
    23  or  IQ3_XXS :  3.06 bpw quantization
    26  or  IQ3_S   :  3.44 bpw quantization
    27  or  IQ3_M   :  3.66 bpw quantization mix
    12  or  Q3_K    : alias for Q3_K_M
    22  or  IQ3_XS  :  3.3 bpw quantization
    11  or  Q3_K_S  :  3.41G, +1.6321 ppl @ Llama-3-8B
    12  or  Q3_K_M  :  3.74G, +0.6569 ppl @ Llama-3-8B
    13  or  Q3_K_L  :  4.03G, +0.5562 ppl @ Llama-3-8B
    25  or  IQ4_NL  :  4.50 bpw non-linear quantization
    30  or  IQ4_XS  :  4.25 bpw non-linear quantization
    15  or  Q4_K    : alias for Q4_K_M
    14  or  Q4_K_S  :  4.37G, +0.2689 ppl @ Llama-3-8B
    15  or  Q4_K_M  :  4.58G, +0.1754 ppl @ Llama-3-8B
    17  or  Q5_K    : alias for Q5_K_M
    16  or  Q5_K_S  :  5.21G, +0.1049 ppl @ Llama-3-8B
    17  or  Q5_K_M  :  5.33G, +0.0569 ppl @ Llama-3-8B
    18  or  Q6_K    :  6.14G, +0.0217 ppl @ Llama-3-8B
    7  or  Q8_0    :  7.96G, +0.0026 ppl @ Llama-3-8B
    1  or  F16     : 14.00G, +0.0020 ppl @ Mistral-7B
    32  or  BF16    : 14.00G, -0.0050 ppl @ Mistral-7B
    0  or  F32     : 26.00G              @ 7B

^^^^^^^^^^^^^^^^^^^^^^
Remarks on GGUF Naming
^^^^^^^^^^^^^^^^^^^^^^

.. important::

    In order to get super-detailed explanation about
    GGUF naming, See: `GGUF Specification <https://github.com/ggml-org/ggml/blob/master/docs/gguf.md#gguf-naming-convention>`_

Since the conversion script doesn't automatically generate a name for the output gguf file, you should 
correctly name your output file by specifying the option :code:`--outfile`.

The `GGUF Specification <https://github.com/ggml-org/ggml/blob/master/docs/gguf.md#gguf-naming-convention>`_ designates
a clean and structured output gguf file naming. Even though the correct output naming is not mandatory,
it is a good practice to name your output model correctly for other people to understand some parameters about the model.

GGUF follow a naming convention of :code:`<BaseName><SizeLabel><FineTune><Version><Encoding><Type><Shard>.gguf` where each
component is delimited by a :code:`-` if present.

The components are:

1. **BaseName**: A descriptive name for the model base type or architecture.

2. **SizeLabel**: Parameter weigth class such as Q, T, B, M, K. 

3. **FineTune**: A descriptive name for the model fine tuning goal(e.g. Chat, Instruct, etc...)

4. **Version**: Denotes the model version number, formatted as v<Major>.<Minor>. This is an optional field.

5. **Encoding**: Indicates the weights encoding scheme that was applied to the model.

6. **Type**: Indicates the kind of gguf file and the intended purpose for it.

Here is a list of gguf file examples:

* :code:`Hermes-2-Pro-Llama-3-8B-F16.gguf`:

    * Model Name: Hermes 2 Pro Llama 3
    * Expert Count: 0
    * Parameter Count: 8B
    * Version Number: v1.0
    * Weight Encoding Scheme: F16
    * Shard: N/A

* :code:`Phi-3-mini-128k-instruct-Q4_0.gguf`:

    * Model Name: Phi 3 mini 128k
    * Export Count: 0
    * Parameter Count: 3B
    * FineTune: Instruct
    * Version Number: v1.0
    * Weight Encoding Scheme: Q4_0
    * Shard: N/A

* :code:`Mixtral-8x7B-v0.1-KQ2.gguf`:

    * Model Name: Mixtral
    * Expert Count: 8
    * Parameter Count: 7B
    * Version Number: v0.1
    * Weight Encoding Scheme: KQ2

---------
SDK Usage
---------

The MBASE inference library has a builtin functionality for displaying/reading/writing metadatas in the GGUF files.
The gguf file manipulation is handled through a single class named `GgufMetaConfigurator` which reside
under the header file :code:`mbase/inference/inf_gguf_metadata_configurator.h`. In the next three sub-sections, 
we will read/write keys/values in the gguf file.

^^^^^^^^^^^^^^^^^
Reading Key/Value
^^^^^^^^^^^^^^^^^

Below are the methods that is associated with key reading operation:

.. code-block:: cpp
    :caption: mbase/inference/inf_gguf_metadata_configurator.h

    class MBASE_API GgufMetaConfigurator : public mbase::non_copymovable {
    public:
        using kv_map = mbase::unordered_map<mbase::string, I32>;        
        ...

        template<typename T>
        bool get_key(const mbase::string& in_key, T& out_value);
        bool has_kv_key(const mbase::string& in_key);
        size_type get_metadata_count();
        kv_map& get_kv_map();
        gguf_type get_kv_key_type(const mbase::string& in_key);

        ...
        
    private:
        ...
    };

Keep in mind that the :code:`kv_map` alias stores a map of keys and their gguf indexes not their values.
In order to get the value of the key, you need to call the :code:`get_key` method with proper parameters.

.. _gguf-display-general-meta:

"""""""""""""""""""""""""""
Displaying General Metadata
"""""""""""""""""""""""""""

Here is an example code that displays the general metadata of the model:

.. code-block:: cpp
    :caption: main.cpp

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

"""""""""""""""""""""""""""""""""
Displaying Architectural Metadata
"""""""""""""""""""""""""""""""""

Here is an example code that displays the architectural metadata of the model:

.. code-block:: cpp
    :caption: main.cpp

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
        metaConfigurator.get_key("general.architecture", modelArchitecture);

        uint32_t contextLength = 0;
        uint32_t embeddingLength = 0;
        uint32_t blockCount = 0;

        metaConfigurator.get_key(modelArchitecture + ".context_length", contextLength);
        metaConfigurator.get_key(modelArchitecture + ".embedding_length", embeddingLength);
        metaConfigurator.get_key(modelArchitecture + ".block_count", blockCount);

        std::cout << "Architecture: " << modelArchitecture << std::endl;
        std::cout << "Context length: " << contextLength << std::endl;
        std::cout << "Embedding length: " << embeddingLength << std::endl;
        std::cout << "Block count: " << blockCount << std::endl;

        return 0;
    }

""""""""""""""""""""""
Displaying All Strings
""""""""""""""""""""""

In order to see and display all key values, you first need to acquire the :code:`kv_map` and iterate over the keys.
Then, build a fat switch case statement for all :code:`gguf_type` types.

Here is an example that prints all strings in gguf file:

.. code-block:: cpp
    :caption: main.cpp

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

        mbase::GgufMetaConfigurator::kv_map& kvMap = metaConfigurator.get_kv_map();
        for(mbase::GgufMetaConfigurator::kv_map::iterator It = kvMap.begin(); It != kvMap.end(); ++It)
        {
            const mbase::string& keyString = It->first;
            if(metaConfigurator.get_kv_key_type(keyString) == gguf_type::GGUF_TYPE_STRING)
            {
                mbase::string valueString;
                metaConfigurator.get_key(keyString, valueString);
                std::cout << keyString << " = " << valueString << std::endl;;
            }
        }

        return 0;
    }
    

^^^^^^^^^^^^^^^^^
Writing Key/Value
^^^^^^^^^^^^^^^^^

Below are the methods that is associated with key reading operation:

.. code-block:: cpp
    :caption: mbase/inference/inf_gguf_metadata_configurator.h

    class MBASE_API GgufMetaConfigurator : public mbase::non_copymovable {
    public:
        ...

        template<typename T>
        GENERIC set_key(const mbase::string& in_key, const T& in_value);
        GENERIC remove_key(const mbase::string& in_key);
        GENERIC clear_context();
        
        ...
    private:
        ...
    };

When you write or remove keys using their corresponding methods, it doesn't write automatically to
the GGUF file but instead it marks those keys as new keys and sets the object into
modified state internally.

Then, by the time you call the :code:`clear_context`  new keys will be written to the file.
Call to :code:`clear_context` may block for a long period of time depending on the 
size of the gguf file. 

Clearing the context will invalidate the object however, this behavior will be changed
in the future.

.. important::

    :code:`clear_context` is also being called on destructor, however it won't block if there are
    no modified keys or the modified keys are already written into the gguf file.

Here is how you do it:

.. code-block:: cpp
    :caption: main.cpp

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

        metaConfigurator.set_key("test.key1", mbase::string("Hello!"));
        metaConfigurator.set_key("test.key2", mbase::string("World!"));
        metaConfigurator.set_key("embedded.system.prompt", mbase::string("You are a wonderful person."));

        metaConfigurator.clear_context(); // This will block for a long period of time

        return 0;
    }

---------------
Header Synopsis
---------------

.. code-block:: cpp
    :caption: mbase/inference/inf_gguf_metadata_configurator.h

    class MBASE_API GgufMetaConfigurator : public mbase::non_copymovable {
    public:
        using kv_map = mbase::unordered_map<mbase::string, I32>;
        using size_type = SIZE_T;

        enum class param_application_error : U8 {
            SUCCESS,
            ERR_ARCH_NOT_SET,
            ERR_BLOCK_COUNT_NOT_FOUND,
            ERR_EMBEDDING_LENGTH_NOT_FOUND,
            ERR_ATTENTION_HEAD_COUNT_NOT_FOUND
        };

        GgufMetaConfigurator(const mbase::wstring in_filename);
        ~GgufMetaConfigurator();

        template<typename T>
        bool get_key(const mbase::string& in_key, T& out_value);
        bool is_open();
        bool has_kv_key(const mbase::string& in_key);
        size_type get_metadata_count();
        kv_map& get_kv_map();
        gguf_type get_kv_key_type(const mbase::string& in_key);
        template<typename T>
        GENERIC set_key(const mbase::string& in_key, const T& in_value);
        GENERIC remove_key(const mbase::string& in_key);
        GENERIC clear_context();

    private:
        gguf_context* mGgufContext;
        kv_map mMetadataMap;
        bool mIsModified;
        mbase::wstring mGgufFile;
        size_type mOldMetaSize;
    };

.. rubric:: Reference

.. [#r1] https://github.com/ggerganov/llama.cpp/blob/master/gguf-py/README.md
.. [#r2] https://github.com/ggerganov/ggml/blob/master/docs/gguf.md#gguf
.. [#r3] https://en.wikipedia.org/wiki/Llama.cpp#Design