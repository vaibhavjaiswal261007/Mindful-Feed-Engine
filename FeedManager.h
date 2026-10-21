#pragma once
#include "Video.h"
#include <set>
#include <vector>
namespace mfe
{
    class FeedManager
    {
        std::vector<Video> candidates;
        std::size_t cursor = 0;
        bool allowRepeat = false;
        std::set<std::string> delivered;

    public:
        explicit FeedManager(std::vector<Video> ranked = {}, bool allowRepeat = false);
        bool next(Video &out);
        std::size_t remaining() const;
        const std::set<std::string> &deliveredIds() const
        {
            return delivered;
        }
        void reset();
    };
}
