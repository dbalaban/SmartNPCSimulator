#include "gridworld.hpp"

GridWorld::GridWorld(size_t width, size_t height,
                     std::vector<ResourceManager> tile_proptypes,
                     std::vector<double> weights, size_t randomSeed)
    : width(width), height(height), tile_prototypes(tile_proptypes), weights(weights), randomSeed(randomSeed) {
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
  return tileMap.at(tileID);
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
      tileMap[tile->getInstanceID()] = coord;
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