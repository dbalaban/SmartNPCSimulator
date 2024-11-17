#ifndef MOVE_ACTION_HPP
#define MOVE_ACTION_HPP

#include "abstract_action.hpp"
#include "character.hpp"
#include "tile.hpp"

class MoveAction : public AbstractAction {
public:
  static const size_t ActionID;

  // move the subject to the object
  static void execute(ElementBase* subject, ElementBase* object) {
    // require the subject to be a character
    Character* character = dynamic_cast<Character*>(subject);
    // require the object to be a tile
    Tile* tile = dynamic_cast<Tile*>(object);

    if (character && tile) {
      // move the character to the tile
      character->setPosition(tile);
      if (character->getPosition() != tile) {
        character->getTraits().kcal_on_hand -= 2*character->getTraits().kcal_burn_rate;
      }
    }
  }

private:
  // Register the action in the registry
  static bool registered;
};

const size_t MoveAction::ActionID = 1;

bool MoveAction::registered = []() {
  MoveAction::registerAction(MoveAction::ActionID, MoveAction::execute);
  return true;
}();

#endif // MOVE_ACTION_HPP