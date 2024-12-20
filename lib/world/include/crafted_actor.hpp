#ifndef CRAFTED_ACTOR_HPP
#define CRAFTED_ACTOR_HPP

#include "abstract_actor.hpp"
#include "gridworld.hpp"
#include "character.hpp"

class CraftedActor : public AbstractActor {
public:
  CraftedActor(size_t charID);
  virtual ~CraftedActor();

  virtual size_t selectAction(const std::vector<ActionDesc>& actions) override;
  virtual void update(double reward) override;

private:
  const size_t characterID;
  const GridWorld& world;
  wCharacterPtr character;
  RandomActor randomActor;
};

#endif // CRAFTED_ACTOR_HPP