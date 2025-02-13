========
Programs
========

This chapter contains some useful programs that show the capabilities of the
MBASE library. Their source code directory is given in their program identification section, so dont hesitate
to use their implementation as a reference point for your specific implementations use case.


Here are the programs that are implemented:

:doc:`Benchmark T2T<benchmark-t2t/about>`: It is a program written to measure the performance
of the given T2T LLM and its impact on your main application logic.

:doc:`Embedding <embedding/about>`: An example program for generating the embeddings of the given prompt or prompts.

:doc:`Openai Server <openai-server/about>`: An Openai API compatible HTTP/HTTPS server for serving LLMs.
This program provides chat completion API for TextToText models and embeddings API For embedder models.

:doc:`Retrieval <retrieval/about>`: An example for calculating the distance between the
given query and multiple text files/documents and applying retrieval operation.

:doc:`Simple Conversation <simple-conversation/about>`: It is a simple executable program where you are having
a dialogue with the LLM you provide. It is useful for examining the answer of the LLM since the system prompt 
and sampler values can be altered.

:doc:`Typo Fixer <typo-fixer/about>`: This is an applied example use case of the MBASE library.
The program is reading a user-supplied text file and fixing the typos.


.. toctree::
    :hidden:

    Benchmark TextToText <benchmark-t2t/about>
    Embedding <embedding/about>
    Openai Server <openai-server/about>
    Retrieval <retrieval/about>
    Simple Conversation <simple-conversation/about>
    Typo Fixer <typo-fixer/about>