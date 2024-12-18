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
typedef std::reference_wrapper<ResourceManager> ResourceManagerRef;

class GridWorld : public Element<GridWorld> {
public:
  static const size_t ElementID = 0;
  static const size_t FeatureSize = 5;

  void addTilePrototypes(std::vector<ResourceManagerRef>& tile_prototypes,
                         std::vector<double>& weights);

  static GridWorld& getInstance() {
    static GridWorld instance;
    return instance;
  }

  std::unique_ptr<double[]> getFeatures() const override {
    std::unique_ptr<double[]> features(new double[FeatureSize]);
    features[0] = ElementID;
    features[1] = getInstanceID();
    features[2] = width;
    features[3] = height;
    features[4] = characters.size();
    return features;
  }

  std::unique_ptr<double[]> getTileFeatures() const;

  std::unique_ptr<double[]> getCharacterFeatures() const;

  TilePtr& getTile(Coord2D coord);

  TilePtr& getTile(size_t tileID);

  const TilePtr& getTile(Coord2D coord) const;

  const TilePtr& getTile(size_t tileID) const;

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

  const std::unordered_map<size_t, size_t>& getCharacterTileMap() const {
    return characterTileMap;
  }

private:
  GridWorld();
  ~GridWorld();

  GridWorld(const GridWorld&) = delete;
  GridWorld& operator=(const GridWorld&) = delete;

  const size_t width;
  const size_t height;
  // grid of tiles
  std::vector<std::vector<TilePtr>> tiles;
  // character ID to character pointer
  std::unordered_map<size_t, CharacterPtr> characters;
  // tile ID to tile coordinate
  std::unordered_map<size_t, Coord2D> tileCoordMap;
  // character ID to tile ID
  std::unordered_map<size_t, size_t> characterTileMap;
  // tile ID to set of character IDs inhabiting that tile
  std::unordered_map<size_t, std::unordered_set<size_t>> tileCharacterMap;

  size_t tileCount;
  std::vector<ResourceManagerRef> tile_prototypes;
  std::vector<double> weights;

  const size_t randomSeed;
};

#endif // GRIDWORLD_HPP