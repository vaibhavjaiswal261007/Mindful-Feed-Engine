#ifndef POST_H
#define POST_H

#include <string>
using namespace std;

// Base class - every post type inherits from this
class Post {
public:
    string id;
    string creator;
    string caption;
    string audioTag;
    int likes;
    int views;

    Post(string id, string creator, string caption, string audioTag, int likes, int views);

    // Every subclass must define its own scoring rule
    virtual double calculateScore() = 0;
    virtual string getType() = 0;

    virtual ~Post() {}
};

class EducationalPost : public Post {
public:
    EducationalPost(string id, string creator, string caption, string audioTag, int likes, int views);
    double calculateScore() override;
    string getType() override;
};

class NewsPost : public Post {
public:
    NewsPost(string id, string creator, string caption, string audioTag, int likes, int views);
    double calculateScore() override;
    string getType() override;
};

class EntertainmentPost : public Post {
public:
    EntertainmentPost(string id, string creator, string caption, string audioTag, int likes, int views);
    double calculateScore() override;
    string getType() override;
};

class GamesPost : public Post {
public:
    GamesPost(string id, string creator, string caption, string audioTag, int likes, int views);
    double calculateScore() override;
    string getType() override;
};

#endif
