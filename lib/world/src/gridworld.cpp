#include "gridworld.hpp"
#include "character.hpp" // Include the header file for the Character class

GridWorld::GridWorld(size_t width, size_t height,
                     std::vector<ResourceManager> tile_proptypes,
                     std::vector<double> weights, size_t randomSeed)
    : Element<GridWorld>(), 
    width(width), 
    height(height), 
    tile_prototypes(tile_proptypes), 
    weights(weights), 
    randomSeed(randomSeed) {
  tileCount = 0;
  tiles.resize(width);
  for (size_t i = 0; i < width; i++) {
    tiles[i].resize(height);
  }
}

GridWorld::~GridWorld() {
  for (size_t i = 0; i < width; i++) {
    for (size_t j = 0; j < height; j++) {
      delete tiles[i][j];
    }
  }
}

void GridWorld::update(double elapsedTime) {
  std::vector<ActionDesc> selectedActions;
  for (auto& character : characters) {
    std::vector<ActionDesc> actions;
    character.second->getAvailableActions(actions);
    ActionDesc action = character.second->getActor()->selectAction(actions);
    selectedActions.push_back(action);
  }

  //TODO: check for conflicts and resolve

  // execute actions
  for (ActionDesc action : selectedActions) {
    AbstractAction::execute(action.ActionID, action.subject, action.object);
  }

  // update position of characters
  for (auto it = characters.begin(); it != characters.end(); ) {
    size_t characterID = it->first;
    size_t knownTileID = characterTileMap[characterID];
    size_t newTileID = it->second->getPosition()->getInstanceID();

    if (knownTileID != newTileID) {
      tileCharacterMap[knownTileID].erase(characterID);
      tileCharacterMap[newTileID].insert(characterID);
      characterTileMap[characterID] = newTileID;
    }

    it->second->update(elapsedTime);

    // if character's health is 0, remove character
    if (it->second->getTraits().health <= 0) {
      Tile* tile = it->second->getPosition();
      tileCharacterMap[tile->getInstanceID()].erase(characterID);
      characterTileMap.erase(characterID);
      it = characters.erase(it); // Erase and update the iterator
    } else {
      ++it; // Only increment the iterator if no deletion occurred
    }
  }

  for (size_t i = 0; i < width; i++) {
    for (size_t j = 0; j < height; j++) {
      tiles[i][j]->update(elapsedTime);
    }
  }
}

Tile* GridWorld::getTile(Coord2D coord) {
  return tiles[coord.first][coord.second];
}

Tile* GridWorld::getTile(size_t tileID) {
  Coord2D coord = getTileCoord(tileID);
  return tiles[coord.first][coord.second];
}

const int GridWorld::getWidth() const {
  return width;
}

const int GridWorld::getHeight() const {
  return height;
}

const size_t GridWorld::getTileID(Coord2D coord) const {
  Tile* tile = tiles[coord.first][coord.second];
  return tile->getInstanceID();
}

const Coord2D GridWorld::getTileCoord(size_t tileID) const {
  return tileCoordMap.at(tileID);
}

const size_t GridWorld::getTileCount() const {
  return tileCount;
}

void GridWorld::GenerateTileMap() {
  std::mt19937 gen(randomSeed);
  std::discrete_distribution<size_t> dist(weights.begin(), weights.end());

  for (size_t i = 0; i < width; i++) {
    for (size_t j = 0; j < height; j++) {
      size_t index = dist(gen);
      ResourceManager resources = tile_prototypes[index];
      Tile* tile = new Tile(resources);
      Coord2D coord = std::make_pair(i, j);
      tileCoordMap[tile->getInstanceID()] = coord;
      tiles[i][j] = tile;
      tileCount++;
    }
  }

  // establish adjacency
  for (size_t i = 0; i < width; i++) {
    for (size_t j = 0; j < height; j++) {
      Tile* tile = tiles[i][j];
      if (i > 0) {
        tile->addAdjacentTile(tiles[i - 1][j]);
      } else {
        tile->addAdjacentTile(tiles[width - 1][j]);
      }
      if (i < width - 1) {
        tile->addAdjacentTile(tiles[i + 1][j]);
      } else {
        tile->addAdjacentTile(tiles[0][j]);
      }
      if (j > 0) {
        tile->addAdjacentTile(tiles[i][j - 1]);
      } else {
        tile->addAdjacentTile(tiles[i][height - 1]);
      }
      if (j < height - 1) {
        tile->addAdjacentTile(tiles[i][j + 1]);
      } else {
        tile->addAdjacentTile(tiles[i][0]);
      }
    }
  }
}

void GridWorld::AddCharacter(CharacterPtr character, Coord2D coord) {
  Tile* tile = getTile(coord);
  character->setPosition(tile);
  size_t characterID = character->getInstanceID();
  characters[characterID] = std::move(character);
  size_t tileID = tile->getInstanceID();
  tileCharacterMap[tileID].insert(characterID);
  characterTileMap[characterID] = tileID;
}

const double** GridWorld::getTileFeatures() const {
  const double** features = new const double*[tileCount];
  for (size_t i = 0; i < tileCount; i++) {
    Coord2D coord = tileCoordMap.at(i);
    Tile* tile = tiles[coord.first][coord.second];
    features[i] = tile->getFeatures();
  }
  return features;
}

const double** GridWorld::getCharacterFeatures() const {
  const double** features = new const double*[characters.size()];
  size_t i = 0;
  for (auto& character : characters) {
    features[i] = character.second->getFeatures();
    i++;
  }
  return features;
}