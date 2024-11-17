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
};

class Character : public Element<Character> {
public:
  static const size_t ElementID = 4;
  static const size_t FeatureSize = 8;

  Character(ActorPtr actor,
            CharacterTraits traits) : 
      Element<Character>(), 
      actor(std::move(actor)),
      traits(traits),
      reward(0) {}

  ~Character() = default;

  void update(double elapsedTime) override {
    burnKcal(traits.kcal_burn_rate * elapsedTime);
    if (traits.kcal_on_hand > 0 && traits.health < traits.max_health) {
      traits.health += traits.health_regen_rate * elapsedTime;
      reward += traits.health_regen_rate * elapsedTime;
      if (traits.health > traits.max_health) {
        traits.health = traits.max_health;
      }
    }

    actor->update(reward);
    reward = 0;
  }

  const double* getFeatures() const override {
    static double features[FeatureSize];
    features[0] = ElementID;
    features[1] = getInstanceID();
    features[2] = traits.health;
    features[3] = traits.health_regen_rate;
    features[4] = traits.max_health;
    features[5] = traits.kcal_on_hand;
    features[6] = traits.kcal_burn_rate;
    features[7] = position->getInstanceID();
    return features;
  }

  void addResources(const Resources& resources) {
    traits.kcal_on_hand += resources.kcal;
  }

  void burnKcal(double kcal) {
    if (traits.kcal_on_hand > kcal) {
      traits.kcal_on_hand -= kcal;
    } else {
      double remaining = kcal - traits.kcal_on_hand;
      traits.kcal_on_hand = 0;
      traits.health -= remaining;
      reward -= remaining;
    }
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

  const CharacterTraits& getTraits() const {
    return traits;
  }

  const double getReward() const {
    return reward;
  }

  void getAvailableActions(std::vector<ActionDesc>& actions);

protected:
  Tile* position;
  ActorPtr actor;
  CharacterTraits traits;
  double reward;
};

typedef std::unique_ptr<Character> CharacterPtr;

#endif // CHARACTER_HPP