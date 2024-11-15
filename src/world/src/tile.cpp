#include "tile.hpp"

void Tile::update(double elapsedTime) {
  resources.update(elapsedTime);
}

const size_t Tile::getInstanceID() const {
  return instanceID;
}