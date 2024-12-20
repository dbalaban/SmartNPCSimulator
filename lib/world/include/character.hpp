#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include <memory>

#include "abstract_actor.hpp"
#include "element.hpp"
#include "tile.hpp"

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

class Character;

typedef std::shared_ptr<Character> CharacterPtr;
typedef std::shared_ptr<const Character> constCharacterPtr;
typedef std::weak_ptr<const Character> wCharacterPtr;

class Character : public Element<Character> {
public:
  static const size_t ElementID = 4;
  static const size_t FeatureSize = 8;

  Character(CharacterTraits traits) : 
      Element<Character>(),
      traits(traits),
      reward(0),
      isActionSet(false) {}

  ~Character() = default;

  void setActionPolicy(ActorPtr& actor_);

  void update(double elapsedTime) override;

  std::unique_ptr<double[]> getFeatures() const override;

  void addResources(const Resources& resources) {
    traits.kcal_on_hand += resources.kcal;
  }

  void burnKcal(double kcal);

  void setPosition(const TilePtr tile) {
    position = tile;
  }

  constTilePtr getPosition() const {
    return position.lock();
  }

  ActorPtr& getActor() {
    if (!isActionSet) {
      std::cerr << "Warning: Actor not set for character " << getInstanceID() << std::endl;
    }
    return actor;
  }

  const CharacterTraits& getTraits() const {
    return traits;
  }

  const double getReward() const {
    return reward;
  }

  void getAvailableActions(std::vector<ActionDesc>& actions);

  bool isActionPolicySet() {
    return isActionSet;
  }

protected:
  wTilePtr position;
  ActorPtr actor;
  CharacterTraits traits;
  double reward;
  bool isActionSet;
};

#endif // CHARACTER_HPP