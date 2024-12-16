#include "character.hpp"
#include "move_action.hpp"
#include "harvest_action.hpp"

#include "data_writer.hpp"

void Character::getAvailableActions(std::vector<ActionDesc>& actions) {
  actions.clear();
  Tile* tile = getPosition();
  // stay in place action, do nothing
  ActionDesc stayInPlaceAction = {ElementID, getInstanceID(), MoveAction::ActionID, tile->getElementID(), tile->getInstanceID(), this, tile};
  actions.push_back(stayInPlaceAction);

  // move to adjacent tile actions
  for (Tile* adjacentTile : tile->getAdjacentTiles()) {
    ActionDesc moveAction = {ElementID, getInstanceID(), MoveAction::ActionID, adjacentTile->getElementID(), adjacentTile->getInstanceID(), this, adjacentTile};
    actions.push_back(moveAction);
  }

  // harvest action
  ActionDesc harvestAction = {ElementID, getInstanceID(), HarvestAction::ActionID, tile->getElementID(), tile->getInstanceID(), this, tile};
  actions.push_back(harvestAction);
}

void Character::update(double elapsedTime) {
  burnKcal(traits.kcal_burn_rate * elapsedTime);
  if (traits.kcal_on_hand > 0 && traits.health < traits.max_health) {
    traits.health += traits.health_regen_rate * elapsedTime;
    reward += traits.health_regen_rate * elapsedTime;
    if (traits.health > traits.max_health) {
      traits.health = traits.max_health;
    }
  }

  std::string name = "Character " + std::to_string(getInstanceID());
  data_management::DataWriter& writer = data_management::DataWriter::getInstance();
  std::string hLab = name + " Health";
  std::string kLab = name + " Kcal";
  writer.writeData<double>(hLab.c_str(), data_management::DataType::DOUBLE, traits.health);
  writer.writeData<double>(kLab.c_str(), data_management::DataType::DOUBLE, traits.kcal_on_hand);

  actor->update(reward);
  reward = 0;
}

std::unique_ptr<double[]> Character::getFeatures() const {
  std::unique_ptr<double[]> features(new double[FeatureSize]);
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

void Character::burnKcal(double kcal) {
  data_management::DataWriter& writer = data_management::DataWriter::getInstance();
  std::string name = "Character " + std::to_string(getInstanceID());
  std::string lab = name + " Kcal Burned";
  writer.writeData(lab.c_str(), data_management::DataType::DOUBLE, kcal);
  if (traits.kcal_on_hand > kcal) {
    traits.kcal_on_hand -= kcal;
  } else {
    double remaining = kcal - traits.kcal_on_hand;
    traits.kcal_on_hand = 0;
    traits.health -= remaining;
    reward -= remaining;
  }
}