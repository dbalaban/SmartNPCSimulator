#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include <memory>

#include "abstract_actor.hpp"
#include "element.hpp"

struct CharacterTraits {
  double health;
  double health_regen_rate;
  double max_health;
  double kcal_on_hand;
  double kcal_burn_rate;

  CharacterTraits(double health,
                  double health_regen_rate,
                  double max_health,
                  double kcal_on_hand,
                  double kcal_burn_rate)
      : health(health),
        health_regen_rate(health_regen_rate),
        max_health(max_health),
        kcal_on_hand(kcal_on_hand),
        kcal_burn_rate(kcal_burn_rate) {}
  
  void update(double elapsedTime) {
    double kcal_burned = kcal_burn_rate * elapsedTime;
    if (kcal_on_hand > kcal_burned) {
      kcal_on_hand -= kcal_burned;
      if (health < max_health) {
        double health_regen = health_regen_rate * elapsedTime;
        health = std::min(health + health_regen, max_health);
      }
    } else {
      kcal_on_hand = 0;
      health -= kcal_burned - kcal_on_hand;
    }
  }
};

class Character : public Element<Character> {
public:
  static const size_t ElementID = 4;

  Character(std::unique_ptr<AbstractActor> actor,
            CharacterTraits traits) : 
      Element<Character>(), 
      actor(std::move(actor)),
      traits(traits) {}

  ~Character() = default;

  void update(double elapsedTime) override {
    traits.update(elapsedTime);
    actor->update(elapsedTime);
  }

  void setPosition(Tile* tile) {
    position = tile;
  }

  Tile* getPosition() {
    return position;
  }

  AbstractActor* getActor() {
    return actor.get();
  }

  CharacterTraits& getTraits() {
    return traits;
  }

  void getAvailableActions(std::vector<ActionDesc>& actions);

protected:
  Tile* position;
  std::unique_ptr<AbstractActor> actor;
  CharacterTraits traits;
};

#endif // CHARACTER_HPP