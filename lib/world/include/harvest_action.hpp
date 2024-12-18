#ifndef HARVEST_ACTION_HPP
#define HARVEST_ACTION_HPP

#include "abstract_action.hpp"
#include "character.hpp"
#include "tile.hpp"

class HarvestAction : public AbstractAction {
public:
  static const size_t ActionID;

  static void execute(ElementBase* subject, ElementBase* object) {
    // require the subject to be a character
    Character* character = dynamic_cast<Character*>(subject);
    // require the object to be a tile
    Tile* tile = dynamic_cast<Tile*>(object);

    if (character && tile) {
      // harvest the tile
      character->addResources(tile->getResources());
      tile->getResources().kcal = 0;
    }
  }

private:
  // Register the action in the registry
  static bool registered;
};

inline const size_t HarvestAction::ActionID = 2;

inline bool HarvestAction::registered = []() {
  HarvestAction::registerAction(HarvestAction::ActionID, HarvestAction::execute);
  return true;
}();

#endif // HARVEST_ACTION_HPP