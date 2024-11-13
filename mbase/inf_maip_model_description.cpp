#include <mbase/inference/inf_maip_model_description.h>
#include <mbase/pc/pc_state.h>

MBASE_BEGIN

InfMaipModelDescription::InfMaipModelDescription() :
    mModelOriginalName(""),
    mModelCustomName(""),
    mDescription(""),
    mSystemPrompt(""),
    mModelFile(""),
    mTags(),
    mIsEmbeddingModel(false),
    mForceSystemPrompt(false),
    mCategory(CATEGORY::TEXT_TO_TEXT),
    mMaximumAllowedContext(0)
{
}

InfMaipModelDescription::~InfMaipModelDescription()
{
}

const mbase::string& InfMaipModelDescription::get_original_name()
{
    return mModelOriginalName;
}

const mbase::string& InfMaipModelDescription::get_custom_name()
{  
    return mModelCustomName;
}

const mbase::string& InfMaipModelDescription::get_description()
{
    return mDescription;
}

const mbase::string& InfMaipModelDescription::get_system_prompt()
{
    return mSystemPrompt;
}

const mbase::string& InfMaipModelDescription::get_model_file()
{
    return mModelFile;
}

const mbase::vector<mbase::string>& InfMaipModelDescription::get_tags()
{
    return mTags;
}

const bool& InfMaipModelDescription::get_embedding()
{
    return mIsEmbeddingModel;
}

const bool& InfMaipModelDescription::get_forced_system_prompt()
{
    return mForceSystemPrompt;
}

const InfMaipModelDescription::CATEGORY& InfMaipModelDescription::get_category_value()
{
    return mCategory;
}

const U32& InfMaipModelDescription::get_maximum_context_length()
{
    return mMaximumAllowedContext;
}

GENERIC InfMaipModelDescription::set_original_name(const mbase::string& in_name)
{
    mModelOriginalName = in_name;
}

GENERIC InfMaipModelDescription::set_custom_name(const mbase::string& in_name)
{
    mModelCustomName = in_name;
}

GENERIC InfMaipModelDescription::set_description(const mbase::string& in_description)
{
    mDescription = in_description;
}

GENERIC InfMaipModelDescription::set_system_prompt(const mbase::string& in_system_prompt)
{
    mSystemPrompt = in_system_prompt;
}

GENERIC InfMaipModelDescription::set_model_file(const mbase::string& in_model_file)
{
    mModelFile = in_model_file;
}

GENERIC InfMaipModelDescription::set_tags(const mbase::vector<mbase::string>& in_tags)
{
    mTags = in_tags;
}

GENERIC InfMaipModelDescription::set_embedding(const bool& in_value)
{
    mIsEmbeddingModel = in_value;
}

GENERIC InfMaipModelDescription::set_force_system_prompt(const bool& in_value)
{
    mForceSystemPrompt = in_value;
}

GENERIC InfMaipModelDescription::set_category(const CATEGORY& in_category)
{
    mCategory = in_category;
}

GENERIC InfMaipModelDescription::set_maximum_context_length(const U32& in_maximum_context)
{
    mMaximumAllowedContext = in_maximum_context;
}

GENERIC InfMaipModelDescription::load_from_state_file(const mbase::string& in_object_name, const mbase::wstring& in_state_path)
{
    PcState modelDescriptionState;
    modelDescriptionState.initialize(in_object_name, in_state_path);

    modelDescriptionState.get_state("original_name", mModelOriginalName);
    modelDescriptionState.get_state("custom_name", mModelCustomName);
    modelDescriptionState.get_state("description", mDescription);
    modelDescriptionState.get_state("system_prompt", mSystemPrompt);
    modelDescriptionState.get_state("model_file", mModelFile);
    modelDescriptionState.get_state("tags", mTags);

    U8 categoryValue = 0;

    modelDescriptionState.get_state("category", categoryValue);
    modelDescriptionState.get_state("max_context_length", mMaximumAllowedContext);

    mCategory = (CATEGORY)categoryValue;
}

GENERIC InfMaipModelDescription::update_state_file(const mbase::wstring& in_state_path)
{
    if(!mModelOriginalName.size())
    {
        return;
    }

    if(!mModelCustomName.size())
    {
        mModelCustomName = mModelOriginalName;
    }

    PcState modelDescriptionState;
    modelDescriptionState.initialize_overwrite(mModelCustomName, in_state_path);

    modelDescriptionState.set_state("original_name", mModelOriginalName);
    modelDescriptionState.set_state("custom_name", mModelCustomName);
    modelDescriptionState.set_state("description", mDescription);
    modelDescriptionState.set_state("system_prompt", mSystemPrompt);
    modelDescriptionState.set_state("model_file", mModelFile);
    modelDescriptionState.set_state("tags", mTags);

    U8 categoryValue = (U8)mCategory;

    modelDescriptionState.set_state("category", categoryValue);
    modelDescriptionState.set_state("max_context_length", mMaximumAllowedContext);

    modelDescriptionState.update();
}

MBASE_END