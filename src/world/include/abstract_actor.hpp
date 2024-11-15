#ifndef ABSTRACT_ACTOR_HPP
#define ABSTRACT_ACTOR_HPP

#include <vector>
#include <random>

#include "element.hpp"
#include "abstract_action.hpp"
#include "tile.hpp"

class AbstractActor : public Element<AbstractActor> {
public:
  virtual ActionDesc selectAction(const std::vector<ActionDesc>& actions) = 0;
};

class RandomActor : public AbstractActor {
public:
  static const size_t ElementID = 3;

  RandomActor(size_t randomSeed) : randomEngine(randomSeed) {}

  ActionDesc selectAction(const std::vector<ActionDesc>& actions) override {
    std::uniform_int_distribution<size_t> distribution(0, actions.size() - 1);
    return actions[distribution(randomEngine)];
  }

protected:
  std::default_random_engine randomEngine;
};

#endif // ABSTRACT_ACTOR_HPP