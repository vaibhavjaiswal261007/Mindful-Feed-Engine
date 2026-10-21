#include "FeedManager.h"
#include <set>
namespace mfe
{
    FeedManager::FeedManager(std::vector<Video> v, bool r) : candidates(std::move(v)), allowRepeat(r) {}
    bool FeedManager::next(Video &o)
    {
        while (cursor < candidates.size())
        {
            Video v = candidates[cursor++];
            if (v.filtered || v.id.empty())
                continue;
            if (!allowRepeat && !delivered.insert(v.id).second)
                continue;
            if (allowRepeat)
                delivered.insert(v.id);
            o = std::move(v);
            return true;
        }
        return false;
    }
    std::size_t FeedManager::remaining() const
    {
        return cursor >= candidates.size() ? 0 : candidates.size() - cursor;
    }
    void FeedManager::reset()
    {
        cursor = 0;
        delivered.clear();
    }
}
