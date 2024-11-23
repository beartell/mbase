#ifndef MBASE_MAIP_MODEL_DESCRIPTION_H
#define MBASE_MAIP_MODEL_DESCRIPTION_H

#include <mbase/common.h>
#include <mbase/string.h>
#include <mbase/vector.h>
#include <mbase/inference/inf_common.h>

MBASE_BEGIN

class MBASE_API InfMaipModelDescription {
public:

    InfMaipModelDescription();
    ~InfMaipModelDescription();

    const mbase::string& get_original_name();
    const mbase::string& get_custom_name();
    const mbase::string& get_description();
    const mbase::string& get_system_prompt();
    const mbase::string& get_model_file();
    const mbase::vector<mbase::string>& get_tags();
    const bool& get_embedding();
    const bool& get_forced_system_prompt();
    inf_model_category get_category_value();
    mbase::string get_category_string();
    const U32& get_maximum_context_length();

    GENERIC set_original_name(const mbase::string& in_name);
    GENERIC set_custom_name(const mbase::string& in_name);
    GENERIC set_description(const mbase::string& in_description);
    GENERIC set_system_prompt(const mbase::string& in_system_prompt);
    GENERIC set_model_file(const mbase::string& in_model_file);
    GENERIC set_tags(const mbase::vector<mbase::string>& in_tags);
    GENERIC set_embedding(const bool& in_value);
    GENERIC set_force_system_prompt(const bool& in_value);
    GENERIC set_category(const inf_model_category& in_category);
    GENERIC set_maximum_context_length(const U32& in_maximum_context);

    GENERIC load_from_state_file(const mbase::string& in_object_name, const mbase::wstring& in_state_path);
    GENERIC update_state_file(const mbase::wstring& in_state_path);

private:
    mbase::string mModelOriginalName; // (MUST)
    mbase::string mModelCustomName; // (MUST) This will also be the name of the state file. If it is not given, it will be the original name
    mbase::string mDescription;
    mbase::string mSystemPrompt;
    mbase::string mModelFile;
    mbase::vector<mbase::string> mTags;
    bool mIsEmbeddingModel;
    bool mForceSystemPrompt;
    inf_model_category mCategory;
    U32 mMaximumAllowedContext; // MUST (MUST)
};

MBASE_END

#endif // MBASE_MAIP_MODEL_DESCRIPTION_H