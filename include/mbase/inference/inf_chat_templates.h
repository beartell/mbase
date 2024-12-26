#ifndef MBASE_CHAT_TEMPLATES
#define MBASE_CHAT_TEMPLATES

#include <mbase/common.h>
#include <mbase/string.h>

MBASE_BEGIN

struct InfTemplateQwen {
	mbase::string systemStart = "<|im_start|>system\n";
	mbase::string assistantStart = "<|im_start|>assistant\n";
	mbase::string userStart = "<|im_start|>user\n";

	mbase::string systemEnd = "<|im_end|>\n";
	mbase::string assistantEnd = "<|im_end|>\n";
	mbase::string userEnd = "<|im_end|>\n";
};

struct InfTemplatePhi3 {
	mbase::string systemStart = "<|system|>\n";
	mbase::string assistantStart = "<|assistant|>\n";
	mbase::string userStart = "<|user|>\n";

	mbase::string systemEnd = "<|end|>\n";
	mbase::string assistantEnd = "<|end|>\n";
	mbase::string userEnd = "<|end|>\n";
};

struct InfTemplateOrion {
	mbase::string systemStart = "System: </s>\n";
	mbase::string assistantStart = "Assistant: </s>\n";
	mbase::string userStart = "</s>Human:\n";

	mbase::string systemEnd = "</s>\n";
	mbase::string assistantEnd = "</s>\n";
	mbase::string userEnd = "</s>\n";
};

struct InfTemplateOpenchat {
	mbase::string systemStart = "<s>GPT4 Correct System: ";
	mbase::string assistantStart = "GPT4 Correct Assistant: ";
	mbase::string userStart = "GPT4 Correct User: ";

	mbase::string systemEnd = "<|end_of_turn|>";
	mbase::string assistantEnd = "<|end_of_turn|>";
	mbase::string userEnd = "<|end_of_turn|>";
};

struct InfTemplateMonarch {
	mbase::string systemStart = "<s>system\n";
	mbase::string assistantStart = "<s>assistant\n";
	mbase::string userStart = "<s>user\n";

	mbase::string systemEnd = "</s>\n";
	mbase::string assistantEnd = "</s>\n";
	mbase::string userEnd = "</s>\n";
};

struct InfTemplateLlama {
	mbase::string systemStart = "<|start_header_id|>system<|end_header_id|>\n";
	mbase::string assistantStart = "<|start_header_id|>assistant<|end_header_id|>\n";
	mbase::string userStart = "<|start_header_id|>user<|end_header_id|>\n";

	mbase::string systemEnd = "<|eot_id|>\n";
	mbase::string assistantEnd = "<|eot_id|>\n";
	mbase::string userEnd = "<|eot_id|>\n";
};

struct InfTemplateDeepseek {
	mbase::string systemStart = "";
	mbase::string assistantStart = "### Response:\n";
	mbase::string userStart = "### Instruction: \n";

	mbase::string systemEnd = "";
	mbase::string assistantEnd = "<|EOT|>\n";
	mbase::string userEnd = "";
};

struct InfTemplateCommandR {
	mbase::string systemStart = "<|START_OF_TURN_TOKEN|><|SYSTEM_TOKEN|>";
	mbase::string assistantStart = "<|START_OF_TURN_TOKEN|><|CHATBOT_TOKEN|>";
	mbase::string userStart = "<|START_OF_TURN_TOKEN|><|USER_TOKEN|>";

	mbase::string systemEnd = "<|END_OF_TURN_TOKEN|>";
	mbase::string assistantEnd = "<|END_OF_TURN_TOKEN|>";
	mbase::string userEnd = "<|END_OF_TURN_TOKEN|>";
};

struct InfTemplateVicuna {
	mbase::string systemStart = "SYSTEM:";
	mbase::string assistantStart = "ASSISTANT:";
	mbase::string userStart = "USER:";

	mbase::string systemEnd = "";
	mbase::string assistantEnd = "</s>\n";
	mbase::string userEnd = "";
};

struct InfTemplateZephyr {
	mbase::string systemStart = "<|system|>\n";
	mbase::string assistantStart = "<|assistant|>\n";
	mbase::string userStart = "<|user|>\n";

	mbase::string systemEnd = "<|endoftext|>\n";
	mbase::string assistantEnd = "<|endoftext|>\n";
	mbase::string userEnd = "<|endoftext|>\n";
};

struct InfTemplateGemma2 {
	mbase::string systemStart = "<start_of_turn>system\n";
	mbase::string assistantStart = "<start_of_turn>model\n";
	mbase::string userStart = "<start_of_turn>user\n";

	mbase::string systemEnd = "<end_of_turn>\n";
	mbase::string assistantEnd = "<end_of_turn>\n";
	mbase::string userEnd = "<end_of_turn>\n";
};

MBASE_INLINE GENERIC tokenizer_align_instruct_template(const mbase::string& in_template,
	mbase::string& out_system_start,
	mbase::string& out_assistant_start,
	mbase::string& out_user_start,
	mbase::string& out_system_end,
	mbase::string& out_assistant_end,
	mbase::string& out_user_end
);

MBASE_INLINE GENERIC tokenizer_align_instruct_template(const mbase::string& in_template,
	mbase::string& out_system_start,
	mbase::string& out_assistant_start,
	mbase::string& out_user_start,
	mbase::string& out_system_end,
	mbase::string& out_assistant_end,
	mbase::string& out_user_end) {
	// Fuck DRY
	// Joke, fix here later
	if (in_template == "command-r") 
	{
		InfTemplateCommandR chatTemplate;
		out_system_start = chatTemplate.systemStart;
		out_assistant_start = chatTemplate.assistantStart;
		out_user_start = chatTemplate.userStart;
		
		out_system_end = chatTemplate.systemEnd;
		out_assistant_end = chatTemplate.assistantEnd;
		out_user_end = chatTemplate.userEnd;
	}
	else if(in_template == "deepseek")
	{
		InfTemplateDeepseek chatTemplate;
		out_system_start = chatTemplate.systemStart;
		out_assistant_start = chatTemplate.assistantStart;
		out_user_start = chatTemplate.userStart;

		out_system_end = chatTemplate.systemEnd;
		out_assistant_end = chatTemplate.assistantEnd;
		out_user_end = chatTemplate.userEnd;
	}
	else if(in_template == "llama")
	{
		InfTemplateLlama chatTemplate;
		out_system_start = chatTemplate.systemStart;
		out_assistant_start = chatTemplate.assistantStart;
		out_user_start = chatTemplate.userStart;

		out_system_end = chatTemplate.systemEnd;
		out_assistant_end = chatTemplate.assistantEnd;
		out_user_end = chatTemplate.userEnd;
	}
	else if(in_template == "monarch")
	{
		InfTemplateMonarch chatTemplate;
		out_system_start = chatTemplate.systemStart;
		out_assistant_start = chatTemplate.assistantStart;
		out_user_start = chatTemplate.userStart;

		out_system_end = chatTemplate.systemEnd;
		out_assistant_end = chatTemplate.assistantEnd;
		out_user_end = chatTemplate.userEnd;
	}
	else if(in_template == "openchat")
	{
		InfTemplateOpenchat chatTemplate;
		out_system_start = chatTemplate.systemStart;
		out_assistant_start = chatTemplate.assistantStart;
		out_user_start = chatTemplate.userStart;

		out_system_end = chatTemplate.systemEnd;
		out_assistant_end = chatTemplate.assistantEnd;
		out_user_end = chatTemplate.userEnd;
	}
	else if(in_template == "orion")
	{
		InfTemplateOrion chatTemplate;
		out_system_start = chatTemplate.systemStart;
		out_assistant_start = chatTemplate.assistantStart;
		out_user_start = chatTemplate.userStart;

		out_system_end = chatTemplate.systemEnd;
		out_assistant_end = chatTemplate.assistantEnd;
		out_user_end = chatTemplate.userEnd;
	}
	else if(in_template == "phi3")
	{
		InfTemplatePhi3 chatTemplate;
		out_system_start = chatTemplate.systemStart;
		out_assistant_start = chatTemplate.assistantStart;
		out_user_start = chatTemplate.userStart;

		out_system_end = chatTemplate.systemEnd;
		out_assistant_end = chatTemplate.assistantEnd;
		out_user_end = chatTemplate.userEnd;
	}
	else if(in_template == "qwen2")
	{
		InfTemplateQwen chatTemplate;
		out_system_start = chatTemplate.systemStart;
		out_assistant_start = chatTemplate.assistantStart;
		out_user_start = chatTemplate.userStart;

		out_system_end = chatTemplate.systemEnd;
		out_assistant_end = chatTemplate.assistantEnd;
		out_user_end = chatTemplate.userEnd;
	}
	else if(in_template == "vicuna")
	{
		InfTemplateVicuna chatTemplate;
		out_system_start = chatTemplate.systemStart;
		out_assistant_start = chatTemplate.assistantStart;
		out_user_start = chatTemplate.userStart;

		out_system_end = chatTemplate.systemEnd;
		out_assistant_end = chatTemplate.assistantEnd;
		out_user_end = chatTemplate.userEnd;
	}
	else if(in_template == "zephyr")
	{
		InfTemplateZephyr chatTemplate;
		out_system_start = chatTemplate.systemStart;
		out_assistant_start = chatTemplate.assistantStart;
		out_user_start = chatTemplate.userStart;

		out_system_end = chatTemplate.systemEnd;
		out_assistant_end = chatTemplate.assistantEnd;
		out_user_end = chatTemplate.userEnd;
	}
	else if(in_template == "gemma2" || in_template == "gemma")
	{
		InfTemplateGemma2 chatTemplate;
		out_system_start = chatTemplate.systemStart;
		out_assistant_start = chatTemplate.assistantStart;
		out_user_start = chatTemplate.userStart;

		out_system_end = chatTemplate.systemEnd;
		out_assistant_end = chatTemplate.assistantEnd;
		out_user_end = chatTemplate.userEnd;
	}
}

MBASE_END

#endif // !MBASE_CHAT_TEMPLATES
