#ifndef MOVE_ACTION_HPP
#define MOVE_ACTION_HPP

#include "abstract_action.hpp"
#include "character.hpp"
#include "tile.hpp"
#include "gridworld.hpp"

class MoveAction : public AbstractAction {
public:
  static const size_t ActionID;

  // move the subject to the object
  static void execute(ElementBase* subject, ElementBase* object) {
    // require the subject to be a character
    Character* character = dynamic_cast<Character*>(subject);
    // require the object to be a tile
    Tile* newTile = dynamic_cast<Tile*>(object);

    if (character && newTile) {
      constTilePtr oldTile = character->getPosition();
      GridWorld& world = GridWorld::getInstance();
      size_t newTileID = newTile->getInstanceID();
      size_t oldTileID = oldTile->getInstanceID();
      if (newTileID != oldTileID) {
        double burnRate = character->getTraits().kcal_burn_rate;
        character->burnKcal(2*burnRate);
      } else {
        // move the character to the tile
        TilePtr newTilePtr = world.getTile(newTileID);
        character->setPosition(newTilePtr);
      }
    }
  }

private:
  // Register the action in the registry
  static bool registered;
};

inline const size_t MoveAction::ActionID = 1;

inline bool MoveAction::registered = []() {
  MoveAction::registerAction(MoveAction::ActionID, MoveAction::execute);
  return true;
}();

#endif // MOVE_ACTION_HPP