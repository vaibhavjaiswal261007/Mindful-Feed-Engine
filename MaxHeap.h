#pragma once
#include "Video.h"
#include <vector>
namespace mfe
{
    class MaxHeap
    {
        std::vector<Video> data_;
        static bool higher(const Video &, const Video &);
        void siftUp(std::size_t);
        void siftDown(std::size_t);

    public:
        MaxHeap() = default;
        explicit MaxHeap(const std::vector<Video> &values);
        void insert(const Video &);
        bool empty() const
        {
            return data_.empty();
        }
        std::size_t size() const
        {
            return data_.size();
        }
        const Video *top() const;
        bool pop(Video &out);
        std::vector<Video> ordered() const;
    };
}
