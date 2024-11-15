#ifndef MOVE_ACTION_HPP
#define MOVE_ACTION_HPP

#include "abstract_action.hpp"
#include "abstract_actor.hpp"
#include "tile.hpp"
#include "character.hpp"

class MoveAction : public AbstractAction<MoveAction> {
static const size_t ActionID = 1;

public:
  // move the subject to the object
  static void takeAction(ElementBase* subject, ElementBase* object) {
    // require the subject to be an actor
    Character* character = dynamic_cast<Character*>(subject);
    // require the object to be a tile
    Tile* tile = dynamic_cast<Tile*>(object);

    if (character && tile) {
      // move the actor to the tile
      character->setPosition(tile);
    }
  }
};

#endif // MOVE_ACTION_HPP