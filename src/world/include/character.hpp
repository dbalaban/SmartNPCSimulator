#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include "abstract_actor.hpp"
#include "element.hpp"

struct CharacterTraits {
  double health;
  double health_regen_rate;
  double max_health;
  double kcal_on_hand;
  double kcal_burn_rate;

  CharacterTraits(double health, double kcal_on_hand, double kcal_burn_rate)
      : health(health), kcal_on_hand(kcal_on_hand), kcal_burn_rate(kcal_burn_rate) {}
  
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

  Character(AbstractActor* actor, CharacterTraits traits) : Element<Character>(), actor(actor), traits(traits) {}

  void update(double elapsedTime) override {
    traits.update(elapsedTime);
  }

  void setPosition(Tile* tile) {
    position = tile;
  }

  Tile* getPosition() {
    return position;
  }

  AbstractActor* getActor() {
    return actor;
  }

  CharacterTraits& getTraits() {
    return traits;
  }

protected:
  Tile* position;
  AbstractActor* actor;
  CharacterTraits traits;
};

#endif // CHARACTER_HPP