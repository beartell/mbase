=============================
Inference Workflow in General
=============================

.. important::

    This page is incomplete. Make sure to follow the examples such as :doc:`../quickstart/single_prompt_ex/about`, :doc:`../quickstart/dialogue_ex/about` and :doc:`../quickstart/embeddings_ex/about`.

The inference workflow is same and simple for all types of models including the ones that are not implemented yet.

Here is the flow of operations user should employ to achieve non-blocking inference:

- Initialize the model object properly. See :doc:`model-in-detail`, and :doc:`../quickstart/single_prompt_ex/initializing_model`.
- Initialize the processor object properly. See :doc:`processor-in-detail`, and :doc:`../quickstart/single_prompt_ex/initializing_processor`.
- Initialize the client object properly. See :doc:`client-in-detail` and :doc:`../quickstart/single_prompt_ex/initializing_client`.
- Invoke their respective methods by employing the :doc:`parallel-state-machine`