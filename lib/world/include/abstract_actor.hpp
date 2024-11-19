#ifndef ABSTRACT_ACTOR_HPP
#define ABSTRACT_ACTOR_HPP

#include <vector>
#include <random>
#include <memory>

#include "element.hpp"
#include "abstract_action.hpp"
#include "tile.hpp"

class AbstractActor {
public:
  virtual ActionDesc selectAction(const std::vector<ActionDesc>& actions) = 0;
  virtual void update(double reward) = 0;
};

typedef std::unique_ptr<AbstractActor> ActorPtr;

class RandomActor : public AbstractActor {
public:
  RandomActor(size_t randomSeed) : randomEngine(randomSeed) {}

  ~RandomActor() {}

  ActionDesc selectAction(const std::vector<ActionDesc>& actions) override {
    std::uniform_int_distribution<size_t> distribution(0, actions.size() - 1);
    return actions[distribution(randomEngine)];
  }

  void update(double reward) {}

protected:
  std::default_random_engine randomEngine;
};

#endif // ABSTRACT_ACTOR_HPP