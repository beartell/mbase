===============
Acquiring Model
===============

.. tip::

    If you already have a valid embedder model with GGUF format, you can skip this chapter.

In order to generate embeddings, you first need to download an open-source
embedder model from huggingface or some place anywhere.

In our example, we will use the nomic-ai's `nomic-ai/nomic-embed-text-v1-GGUF <https://huggingface.co/nomic-ai/nomic-embed-text-v1-GGUF>`_
because it is popular and useful for demonstrative examples. However, what embedder model you use
won't make a difference for our example, just make sure you have one in your system.

MBASE Inference engine only supports "gguf" file format so that safetensors or any format that is not gguf, must
be converted into gguf format.

.. tip::

    Non GGUF models can be converted into GGUF format.
    To see how to do it and in order to have in-depth knowledge of GGUF format, 
    refer to: :ref:`gguf-convert`

------------------------------------
Downloading a Model from Huggingface
------------------------------------

The gguf file of nomic-embed-text-v1 is already being hosted on nomic-ai's huggingface repository.

Repo link: https://huggingface.co/nomic-ai/nomic-embed-text-v1-GGUF

^^^^^^^^^^^^
From Website
^^^^^^^^^^^^

Step 1: Click the "Files and versions" tab:

Step 2: Then download the Q4 quantized version of the model:

^^^^^^^^^^
Using wget
^^^^^^^^^^

Here is a wget command for people who love downloading using terminal:

.. code-block:: bash

    wget https://huggingface.co/nomic-ai/nomic-embed-text-v1-GGUF/resolve/main/nomic-embed-text-v1.Q8_0.gguf
