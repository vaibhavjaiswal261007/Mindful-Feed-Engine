#include "RankingEngine.h"
#include <set>
namespace mfe
{
    RankingEngine::RankingEngine(RankingConfig c) : config_(std::move(c)) {}
    std::vector<Video> RankingEngine::rank(const std::vector<Video> &vs) const
    {
        MaxHeap h;
        std::set<std::string> ids;
        for (auto v : vs)
        {
            if (v.filtered || v.id.empty() || !ids.insert(v.id).second)
                continue;
            double pref = 0;
            auto it = config_.preferences.find(v.category);
            if (it != config_.preferences.end())
                pref = it->second;
            v.rankingComponents = {{"nutritional_score", v.nutritionalScore, config_.nutritionWeight, v.nutritionalScore * config_.nutritionWeight, "Nutritional Score weighted by configured ranking weight."}, {"category_preference", pref, config_.categoryWeight, pref * config_.categoryWeight, "Manual category preference; defaults to zero."}};
            v.rankingScore = v.rankingComponents[0].contribution + v.rankingComponents[1].contribution;
            h.insert(v);
        }
        return h.ordered();
    }
}
