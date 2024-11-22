#include <mbase/inference/inf_maip_peer_base.h>
#include <mbase/inference/inf_processor.h>

MBASE_BEGIN

InfMaipPeerBase::InfMaipPeerBase(std::shared_ptr<mbase::PcNetPeerClient> in_peer, const mbase::string& in_maip_user, const mbase::string& in_peer_category):
    mPeer(in_peer),
    mMaipUsername(in_maip_user),
    mPeerCategory(in_peer_category)
{

}

InfMaipPeerBase::~InfMaipPeerBase()
{
    
}

std::shared_ptr<mbase::PcNetPeerClient> InfMaipPeerBase::get_maip_peer() const
{
    return mPeer;
}

const mbase::string& InfMaipPeerBase::get_maip_username() const
{
    return mMaipUsername;
}

const mbase::string& InfMaipPeerBase::get_peer_category() const
{
    return mPeerCategory;
}

InfProcessorBase* InfMaipPeerBase::get_processor_by_id(const U64& in_id)
{
    return mIndexedProcMap[in_id];
}

mbase::vector<U64> InfMaipPeerBase::get_processor_ids() const
{
    mbase::vector<U64> procVector;
    for(indexed_processor_map::const_iterator It = mIndexedProcMap.cbegin(); It != mIndexedProcMap.cend(); ++It)
    {
        procVector.push_back(It->first);
    }
    return procVector;
}

GENERIC InfMaipPeerBase::add_processor(InfProcessorBase* in_address, U64& out_context_id)
{
    mRegisteredProcMap.insert({in_address, mContextCounter});
    mIndexedProcMap.insert({mContextCounter, in_address});

    out_context_id = mContextCounter;
    mContextCounter++;
}

GENERIC InfMaipPeerBase::set_network_peer(std::shared_ptr<mbase::PcNetPeerClient> in_peer)
{
    mPeer = in_peer;
}

GENERIC InfMaipPeerBase::set_maip_username(const mbase::string& in_user)
{
    mMaipUsername = in_user;
}

GENERIC InfMaipPeerBase::remove_processor_by_address(InfProcessorBase* in_address)
{
    registered_processor_map::iterator It = mRegisteredProcMap.find(in_address);
    if(It == mRegisteredProcMap.end())
    { 
        return; 
    }

    mIndexedProcMap.erase(It->second);
    mRegisteredProcMap.erase(It);
}

GENERIC InfMaipPeerBase::remove_processor_by_id(const U64& in_id)
{
    indexed_processor_map::iterator It = mIndexedProcMap.find(in_id);
    if(It == mIndexedProcMap.end())
    { 
        return; 
    }

    mRegisteredProcMap.erase(It->second);
    mIndexedProcMap.erase(It);
}

GENERIC InfMaipPeerBase::mandatory_processor_cleanup()
{
    for(registered_processor_map::iterator It = mRegisteredProcMap.begin(); It != mRegisteredProcMap.end(); ++It)
    {
        InfProcessorBase* procBase = It->first;
        procBase->release_inference_client_stacked();
        delete It->first;
    }
}

MBASE_END