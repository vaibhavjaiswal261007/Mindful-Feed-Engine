#pragma once
#include "FeedManager.h"
#include <string>
namespace mfe {
enum class SessionStatus { Delivered, BreakRequired, FeedEmpty, NotStarted };
struct DeliveryResult { SessionStatus status=SessionStatus::NotStarted; Video video; std::size_t remainingTokens=0; std::string message; bool breakRequired=false; /* true once the last token is spent */ };
class BreakController { std::string message_; public: explicit BreakController(std::string message="You've reached your viewing limit. Take a mindful break."); const std::string& message()const{return message_;} };
class SessionManager {
 FeedManager& feed_; std::size_t capacity_,remaining_; bool active_=false; BreakController breaks_;
public: SessionManager(FeedManager& feed,std::size_t tokens,BreakController breaks=BreakController());
 void start(); DeliveryResult next(); void end(); std::size_t remainingTokens()const{return remaining_;} bool active()const{return active_;}
};
}
