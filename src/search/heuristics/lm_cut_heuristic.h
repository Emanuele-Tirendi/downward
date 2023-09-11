#ifndef HEURISTICS_LM_CUT_HEURISTIC_H
#define HEURISTICS_LM_CUT_HEURISTIC_H

#include "../heuristic.h"

#include <memory>

namespace plugins {
class Options;
}

namespace lm_cut_heuristic {
class LandmarkCutLandmarks;

class LandmarkCutHeuristic : public Heuristic {
    std::unique_ptr<LandmarkCutLandmarks> landmark_generator;

    virtual int compute_heuristic(const State &ancestor_state) override;
public:
    explicit LandmarkCutHeuristic(const plugins::Options &opts);
    explicit LandmarkCutHeuristic(std::basic_string<char> unparsed_config,
                                  utils::LogProxy log,
                                  bool cache_evaluator_values,
                                  std::shared_ptr<AbstractTask> task);
    explicit LandmarkCutHeuristic(std::shared_ptr<AbstractTask> task,
                                  utils::Verbosity verbosity,
                                  std::shared_ptr<AbstractTask> transform,
                                  bool cache_evaluator_values);
    virtual ~LandmarkCutHeuristic() override;
};
}

#endif
