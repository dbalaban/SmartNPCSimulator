#include "gridworld_view.hpp"
#include <iostream>

GridWorldView::GridWorldView(GridWorld& model) : model(model) {}

void GridWorldView::draw(sf::RenderWindow& window) {
  size_t width = model.getWidth();
  size_t height = model.getHeight();
  float tileWidth = window.getSize().x / width;
  float tileHeight = window.getSize().y / height;

  for (size_t i = 0; i < width; i++) {
    for (size_t j = 0; j < height; j++) {
      sf::RectangleShape tile(sf::Vector2f(tileWidth, tileHeight));
      tile.setPosition(i * tileWidth, j * tileHeight);

      // Get the resources of the tile
      const ResourceManager& resourceInfo = model.getTile({i, j})->getResourceManager();
      float ratio = static_cast<float>(resourceInfo.resources.kcal / resourceInfo.maxResources.kcal);

      // float ratio = static_cast<float>(i + j) / static_cast<float>((width - 1) + (height - 1));

      // Set the color of the tile based on the resources
      // [0,255,0] if the resources are full
      // [127,127,255] if the resources are half full
      // [255,0,0] if the resources are empty
      uint8_t r = 255 * (1 - ratio) + 0.5;
      uint8_t g = 255 * (ratio) + 0.5;
      uint8_t b = 255 * (1 - 2*fabs(ratio - 0.5)) + 0.5;

      tile.setFillColor(sf::Color(r, g, b));
      window.draw(tile);
    }
  }
}