===================
About Context Lines
===================

As we have mentioned in the :doc:`single-prompt example <../single_prompt_ex/about>`, 
we supply an array of context lines to the tokenizer in order to form the dialogue.

The order of the context lines supplied to the tokenizer will determine the order of our conversation with LLM.

The :code:`context_line` is defined in the header file :code:`mbase/inference/inf_context_line.h` as follows:

.. code-block:: cpp
    :caption: mbase/inference/inf_context_line.h

    enum class context_role {
        SYSTEM,
        ASSISTANT,
        USER,
        NONE
    };

    struct context_line {
        context_role mRole = context_role::NONE;
        mbase::string mMessage = "";
        U32 mMessageIndex;
    };

The message index makes no sense for our example's case. It is relevant if you make the client
manage the message history by itself. What matters here is the role and message fields of the struct.

----------------
Example Dialogue
----------------

Let's take an example of the given conversation:

.. code-block::
    :caption: example dialogue

    System: You are a helpful assistant.

    User: Can you code?
    Assistant: Yes I know how to write C++.

    User: Do you like MBASE?
    Assistant: Yep, pleasant experience.

    User: Are you an artifical intelligence?
    Assistant: Whatever you say...

This dialogue can be represented in context lines as follows:

.. code-block:: cpp

    mbase::vector<mbase::context_line> chatHistory = {
        {mbase::context_role::SYSTEM, "You are a helpful assistant."},
        {mbase::context_role::USER, "Can you code?"},
        {mbase::context_role::ASSISTANT, "Yes I know how to write C++."},
        {mbase::context_role::USER, "Do you like MBASE?"},
        {mbase::context_role::ASSISTANT, "Yep, pleasant experience."},
        {mbase::context_role::USER, "Are you an artifical intelligence?"},
        {mbase::context_role::ASSISTANT, "Whatever you say..."}
    };

After the context lines are supplied to the tokenizer and the token vector is generated, 
we will execute the token vector using :code:`execute_input` method.

---------
Basically
---------

* If you want the method of prompt resupplying, tokenize the entire chat history and execute the token vector. (First option)
* If you want the processor to keep the context, set the manual caching mode and tokenize and execute only the last prompt. (Second option)
