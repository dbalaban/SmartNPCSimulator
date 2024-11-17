#ifndef SMART_ACTOR_HPP
#define SMART_ACTOR_HPP

#include "abstract_actor.hpp"
#include "character.hpp"
#include <vector>

class SmartActor : public AbstractActor {
static const size_t ElementID = 5;
public:
  SmartActor(Character* character) : character(character) {}

  void update(double elapsedTime) override;

  ActionDesc selectAction(const std::vector<ActionDesc>& actions) override;

private:
  Character* character; // Associated Character object
};

#endif // SMART_ACTOR_HPP