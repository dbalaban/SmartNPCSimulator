#ifndef GRIDWORLD_HPP
#define GRIDWORLD_HPP

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <random>

#include "element.hpp"
#include "tile.hpp"
#include "character.hpp"

typedef std::pair<size_t, size_t> Coord2D;

class GridWorld : public Element<GridWorld> {
public:
  static const size_t ElementID = 0;
  static const size_t FeatureSize = 5;

  GridWorld(size_t width, size_t height,
            std::vector<ResourceManager> tile_prototypes,
            std::vector<double> weights,
            size_t randomSeed = 0);

  ~GridWorld();

  double* getFeatures() const override {
    static double features[FeatureSize];
    features[0] = ElementID;
    features[1] = getInstanceID();
    features[2] = width;
    features[3] = height;
    features[4] = characters.size();
    return features;
  }

  double* getTileFeatures() const;

  double* getCharacterFeatures() const;

  Tile* getTile(Coord2D coord);

  Tile* getTile(size_t tileID);

  const int getWidth() const;

  const int getHeight() const;

  const size_t getTileID(Coord2D coord) const;

  const Coord2D getTileCoord(size_t tileID) const;

  const size_t getTileCount() const;

  const size_t getCharacterCount() const {
    return characters.size();
  }

  void GenerateTileMap();

  void AddCharacter(CharacterPtr Character, Coord2D coord);

  void update(double elapsedTime) override;

  const std::unordered_map<size_t, Coord2D>& getTileCoordMap() const {
    return tileCoordMap;
  }

  const std::unordered_map<size_t, std::unordered_set<size_t>>& getTileCharacterMap() const {
    return tileCharacterMap;
  }

  const CharacterPtr& getCharacter(size_t characterID) const {
    return characters.at(characterID);
  }

private:
  size_t width;
  size_t height;
  // grid of tiles
  std::vector<std::vector<Tile*>> tiles;
  // character ID to character pointer
  std::unordered_map<size_t, CharacterPtr> characters;
  // tile ID to tile coordinate
  std::unordered_map<size_t, Coord2D> tileCoordMap;
  // character ID to tile ID
  std::unordered_map<size_t, size_t> characterTileMap;
  // tile ID to set of character IDs inhabiting that tile
  std::unordered_map<size_t, std::unordered_set<size_t>> tileCharacterMap;

  size_t tileCount;
  std::vector<ResourceManager> tile_prototypes;
  std::vector<double> weights;

  const size_t randomSeed;
};

#endif // GRIDWORLD_HPP