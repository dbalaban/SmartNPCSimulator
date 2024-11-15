#ifndef HARVEST_ACTION_HPP
#define HARVEST_ACTION_HPP

#include "abstract_action.hpp"
#include "character.hpp"

class HarvestAction : public AbstractAction<HarvestAction> {
public:
  static const size_t ActionID = 2;

  static void takeAction(ElementBase* subject, ElementBase* object) {
    // require the subject to be an actor
    Character* character = dynamic_cast<Character*>(subject);
    // require the object to be a tile
    Tile* tile = dynamic_cast<Tile*>(object);

    if (character && tile) {
      // harvest the tile
      character->getTraits().kcal_on_hand += tile->getResources().kcal;
      tile->getResources().kcal = 0;
    }
  }
};

#endif // HARVEST_ACTION_HPP