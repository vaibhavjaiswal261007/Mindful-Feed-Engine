#include "SessionManager.h"
namespace mfe {
BreakController::BreakController(std::string m):message_(std::move(m)){}
SessionManager::SessionManager(FeedManager&f,std::size_t t,BreakController b):feed_(f),capacity_(t),remaining_(t),breaks_(std::move(b)){}
void SessionManager::start(){feed_.reset();active_=true;remaining_=capacity_;} // new session = fresh feed + fresh tokens
DeliveryResult SessionManager::next(){DeliveryResult r;r.remainingTokens=remaining_;if(!active_){r.status=SessionStatus::NotStarted;r.message="Session has not started.";return r;}if(remaining_==0){r.status=SessionStatus::BreakRequired;r.breakRequired=true;r.message=breaks_.message();return r;}if(!feed_.next(r.video)){r.status=SessionStatus::FeedEmpty;r.message="No eligible videos remain.";return r;}--remaining_;r.status=SessionStatus::Delivered;r.remainingTokens=remaining_;if(remaining_==0){r.breakRequired=true;r.message=breaks_.message();}return r;}
void SessionManager::end(){active_=false;}
}
