#include "gridworld.hpp"
#include "character.hpp" // Include the header file for the Character class

#include "param_reader.hpp"

GridWorld::GridWorld()
    : Element<GridWorld>(), 
    width(data_management::ParamReader::getInstance().getParam<size_t>("GridWorld", "width", 10)),
    height(data_management::ParamReader::getInstance().getParam<size_t>("GridWorld", "height", 10)),
    randomSeed(data_management::ParamReader::getInstance().getParam<size_t>("GridWorld", "randomSeed", 0)),
    tileCount(0) {
  tiles.resize(width);
  for (size_t i = 0; i < width; i++) {
    tiles[i].resize(height);
  }
}

GridWorld::~GridWorld() {
  for (size_t i = 0; i < width; i++) {
    for (size_t j = 0; j < height; j++) {
      tiles[i][j].reset();
    }
  }
}

void GridWorld::addTilePrototypes(std::vector<ResourceManagerRef>& tile_prototypes,
                                  std::vector<double>& weights) {
  if (tile_prototypes.size() != weights.size()) {
    throw std::invalid_argument("tile_prototypes and weights must have the same size");
  }
  // append the tile prototypes and weights to the existing ones
  this->tile_prototypes.insert(this->tile_prototypes.end(),
                               tile_prototypes.begin(),
                               tile_prototypes.end());
  this->weights.insert(this->weights.end(), weights.begin(), weights.end());
  tile_prototypes.clear();
}

void GridWorld::update(double elapsedTime) {
  std::vector<ActionDesc> selectedActions;
  for (auto& character : characters) {
    std::vector<ActionDesc> actions;
    character.second->getAvailableActions(actions);
    size_t action_choice = character.second->getActor()->selectAction(actions);
    if (action_choice >= actions.size()) {
      std::cerr << "Invalid action choice: " << action_choice 
                << " by character " << character.first
                << " of " << actions.size() << " available actions, skipping character." << std::endl;
      continue;
    }
    ActionDesc action = actions[action_choice];
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
      TilePtr tile = it->second->getPosition();
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

TilePtr& GridWorld::getTile(Coord2D coord) {
  return tiles[coord.first][coord.second];
}

TilePtr& GridWorld::getTile(size_t tileID) {
  Coord2D coord = getTileCoord(tileID);
  return tiles[coord.first][coord.second];
}

const TilePtr& GridWorld::getTile(Coord2D coord) const {
  return tiles[coord.first][coord.second];
}

const TilePtr& GridWorld::getTile(size_t tileID) const {
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
  const TilePtr& tile = tiles[coord.first][coord.second];
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
      TilePtr tile(new Tile(resources));
      Coord2D coord = std::make_pair(i, j);
      tileCoordMap[tile->getInstanceID()] = coord;
      tiles[i][j] = tile;
      tileCount++;
    }
  }

  // establish adjacency
  for (size_t i = 0; i < width; i++) {
    for (size_t j = 0; j < height; j++) {
      TilePtr& tile = tiles[i][j];
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
  TilePtr& tile = getTile(coord);
  character->setPosition(tile);
  size_t characterID = character->getInstanceID();
  characters[characterID] = std::move(character);
  size_t tileID = tile->getInstanceID();
  tileCharacterMap[tileID].insert(characterID);
  characterTileMap[characterID] = tileID;
}

std::unique_ptr<double[]> GridWorld::getTileFeatures() const {
  size_t tile_feature_size = Tile::FeatureSize;
  std::unique_ptr<double[]> features(new double[tileCount * tile_feature_size]);
  for (size_t i = 0; i < tileCount; i++) {
    Coord2D coord = tileCoordMap.at(i);
    const TilePtr& tile = tiles[coord.first][coord.second];
    std::unique_ptr<double[]> tile_features = tile->getFeatures();
    std::copy(tile_features.get(), tile_features.get() + tile_feature_size, features.get() + i * tile_feature_size);
  }
  return features;
}

std::unique_ptr<double[]> GridWorld::getCharacterFeatures() const {
  size_t character_feature_size = Character::FeatureSize;
  std::unique_ptr<double[]> features(new double[characters.size() * character_feature_size]);
  for (size_t i = 0; i < characters.size(); i++) {
    Character* character = characters.at(i).get();
    std::unique_ptr<double[]> character_features = character->getFeatures();
    std::copy(character_features.get(), character_features.get() + character_feature_size, features.get() + i * character_feature_size);
  }
  return features;
}