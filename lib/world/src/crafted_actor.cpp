#include "crafted_actor.hpp"
#include "gridworld.hpp"
#include "move_action.hpp"
#include "harvest_action.hpp"

CraftedActor::CraftedActor(size_t charID):
    characterID(charID),
    world(GridWorld::getInstance()) {
  character = world.getCharacter(characterID);
}

CraftedActor::~CraftedActor() {}

size_t CraftedActor::selectAction(const std::vector<ActionDesc>& actions) {
  // Get the tile the character is on
  constTilePtr tile = character.lock()->getPosition();
  const Resources& resources = tile->getResources();
  // if the current tile has less resources than the character's burn rate, move to a new tile
  if (resources.kcal < character.lock()->getTraits().kcal_burn_rate) {
    const size_t moveActionID = MoveAction::ActionID;
    double max_kcal = 0;
    size_t max_kcal_action = 0;
    bool canMove = false;
    for (size_t i = 0; i < actions.size(); i++) {
      if (actions[i].ActionID == moveActionID) {
        canMove = true;
        size_t newTileID = actions[i].object->getInstanceID();
        const TilePtr& newTile = world.getTile(newTileID);
        const Resources& newResources = newTile->getResources();
        if (newResources.kcal > max_kcal) {
          max_kcal = newResources.kcal;
          max_kcal_action = i;
        }
      }
    }
    if (canMove) {
      return max_kcal_action;
    }

  } else {
    // if the current tile has enough resources, stay and gather resources
    // find the index of the gather action
    const size_t gatherActionID = HarvestAction::ActionID;
    for (size_t i = 0; i < actions.size(); i++) {
      if (actions[i].ActionID == gatherActionID) {
        return i;
      }
    }
  }

  // if no valid action is found, return a random action
  return randomActor.selectAction(actions);
}

void CraftedActor::update(double reward) {}