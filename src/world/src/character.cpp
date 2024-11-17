#include "character.hpp"
#include "move_action.hpp"
#include "harvest_action.hpp"

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