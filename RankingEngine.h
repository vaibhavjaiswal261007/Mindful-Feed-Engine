#pragma once
#include "Video.h"
#include "MaxHeap.h"
#include <map>
namespace mfe
{
    struct RankingConfig
    {
        double nutritionWeight = .85, categoryWeight = .15;
        std::map<std::string, double> preferences;
    };
    class RankingEngine
    {
        RankingConfig config_;

    public:
        explicit RankingEngine(RankingConfig config = {});
        std::vector<Video> rank(const std::vector<Video> &videos) const;
    };
}
