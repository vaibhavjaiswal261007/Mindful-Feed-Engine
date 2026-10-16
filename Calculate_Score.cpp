#include "Calculate_Score.h"

// Base Post
Post::Post(string id, string creator, string caption, string audioTag, int likes, int views)
    : id(id), creator(creator), caption(caption), audioTag(audioTag), likes(likes), views(views) {}

// Educational Post
EducationalPost::EducationalPost(string id, string creator, string caption, string audioTag, int likes, int views)
    : Post(id, creator, caption, audioTag, likes, views) {}

double EducationalPost::calculateScore() {
    // Educational content gets a boost
    return (likes * 0.5 + views * 0.2) * 2.5;
}

string EducationalPost::getType() { return "Educational"; }

// News Post
NewsPost::NewsPost(string id, string creator, string caption, string audioTag, int likes, int views)
    : Post(id, creator, caption, audioTag, likes, views) {}

double NewsPost::calculateScore() {
    return likes * 0.3 + views * 0.2;
}

string NewsPost::getType() { return "News"; }

// Entertainment Post
EntertainmentPost::EntertainmentPost(string id, string creator, string caption, string audioTag, int likes, int views)
    : Post(id, creator, caption, audioTag, likes, views) {}

double EntertainmentPost::calculateScore() {
    // No boost - this is the category most brainrot content falls into
    return likes * 0.1 + views * 0.1;
}

string EntertainmentPost::getType() { return "Entertainment"; }

// Games Post
GamesPost::GamesPost(string id, string creator, string caption, string audioTag, int likes, int views)
    : Post(id, creator, caption, audioTag, likes, views) {}

double GamesPost::calculateScore() {
    return likes * 0.15 + views * 0.15;
}

string GamesPost::getType() { return "Games"; }
