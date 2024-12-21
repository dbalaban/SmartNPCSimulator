#ifndef SMART_ACTOR_HPP
#define SMART_ACTOR_HPP

#include "abstract_actor.hpp"
#include "character.hpp"
#include "gridworld.hpp"
#include "StateValueEstimator.hpp"
#include "FOMAP.hpp"

#include <vector>

namespace rl {

typedef torch::autograd::variable_list Parameters;

class SmartActor : public AbstractActor {
static const size_t ElementID = 5;
public:
  SmartActor();

  void update(double reward) override;

  size_t selectAction(const std::vector<ActionDesc>& actions) override;

private:
  GridWorld& world; // Global reference object
  StateValueEstimator v; // State value estimator
  FOMAP fomap; // Fully Observable Markovian Action Policy

  size_t randomSeed; // Random seed for the actor
  std::default_random_engine randomEngine; // Random engine for the actor

  torch::Tensor last_action_prob; // Probability of the last action
  torch::Tensor last_state_value; // Value of the last state
  const double discounting_factor; // Discounting factor for future rewards
  const double learning_rate_actor; // Learning rate for the actor
  const double learning_rate_critic; // Learning rate for the critic
  const double elibility_decay_actor; // Decay factor for the actor eligibility traces
  const double elibility_decay_critic; // Decay factor for the critic eligibility traces

  // stochastic gradient descent
  torch::optim::Adam optimizer_actor;
  torch::optim::RMSprop optimizer_critic;

  Parameters actor_eligibility_trace;
  Parameters critic_eligibility_trace;
};

} // namespace rl

#endif // SMART_ACTOR_HPP