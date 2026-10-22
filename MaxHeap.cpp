#include "MaxHeap.h"
#include <algorithm>
namespace mfe
{
    bool MaxHeap::higher(const Video &a, const Video &b)
    {
        if (a.rankingScore != b.rankingScore)
            return a.rankingScore > b.rankingScore;
        if (a.nutritionalScore != b.nutritionalScore)
            return a.nutritionalScore > b.nutritionalScore;
        return a.id < b.id;
    }
    MaxHeap::MaxHeap(const std::vector<Video> &v) : data_(v)
    {
        if (data_.size() > 1)
        {
            for (size_t i = data_.size() / 2; i > 0; --i)
                siftDown(i - 1);
        }
    }
    void MaxHeap::siftUp(size_t i)
    {
        while (i)
        {
            size_t p = (i - 1) / 2;
            if (!higher(data_[i], data_[p]))
                break;
            std::swap(data_[i], data_[p]);
            i = p;
        }
    }
    void MaxHeap::siftDown(size_t i)
    {
        for (;;)
        {
            size_t l = 2 * i + 1, r = l + 1, b = i;
            if (l < data_.size() && higher(data_[l], data_[b]))
                b = l;
            if (r < data_.size() && higher(data_[r], data_[b]))
                b = r;
            if (b == i)
                break;
            std::swap(data_[i], data_[b]);
            i = b;
        }
    }
    void MaxHeap::insert(const Video &v)
    {
        data_.push_back(v);
        siftUp(data_.size() - 1);
    }
    const Video *MaxHeap::top() const
    {
        return data_.empty() ? nullptr : &data_[0];
    }
    bool MaxHeap::pop(Video &o)
    {
        if (data_.empty())
            return false;
        o = std::move(data_[0]);
        if (data_.size() == 1)
        {
            data_.pop_back();
            return true;
        }
        data_[0] = std::move(data_.back());
        data_.pop_back();
        siftDown(0);
        return true;
    }
    std::vector<Video> MaxHeap::ordered() const
    {
        MaxHeap h = *this;
        std::vector<Video> o;
        Video v;
        while (h.pop(v))
            o.push_back(v);
        return o;
    }
}
