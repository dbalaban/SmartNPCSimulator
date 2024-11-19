#include "gridworld_view.hpp"
#include <iostream>
#include <cmath>
#include <SFML/Graphics.hpp>

GridWorldView::GridWorldView(GridWorld& model) : model(model) {}

void GridWorldView::setTimeElapsed(float timeElapsed) {
  this->timeElapsed = timeElapsed;
}

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
      float ratio = static_cast<float>(resourceInfo.resources.kcal) / static_cast<float>(resourceInfo.maxResources.kcal);

      // Set the color of the tile based on the resources
      uint8_t r = static_cast<uint8_t>(255 * (1 - ratio) + 0.5);
      uint8_t g = static_cast<uint8_t>(255 * ratio + 0.5);
      uint8_t b = static_cast<uint8_t>(255 * (1 - 2 * std::fabs(ratio - 0.5)) + 0.5);

      tile.setFillColor(sf::Color(r, g, b));
      window.draw(tile);
    }
  }

  float dot_size = 2 * characterSize + characterSpacing;
  size_t max_dots_per_tile = static_cast<size_t>(tileHeight * tileWidth / (dot_size * dot_size));

  const std::unordered_map<size_t, Coord2D>& tileCoordMap = model.getTileCoordMap();
  const std::unordered_map<size_t, std::unordered_set<size_t>>& tileCharacterMap = model.getTileCharacterMap();

  sf::Font font;
  if (!font.loadFromFile("resources/Ubuntu-R.ttf")) {
    std::cerr << "Error loading font" << std::endl;
    return;
  }
  sf::Text timeText;
  timeText.setFont(font);
  timeText.setString("World Time: " + std::to_string(timeElapsed));
  timeText.setCharacterSize(24);
  timeText.setFillColor(sf::Color::White);
  timeText.setPosition(10, 10);
  window.draw(timeText);

  for (const auto& pair : tileCharacterMap) {
    size_t tileID = pair.first;
    Coord2D coord = tileCoordMap.at(tileID);
    size_t numCharacters = pair.second.size();
    if (numCharacters > max_dots_per_tile) {
      sf::CircleShape dot(characterSize);
      dot.setFillColor(sf::Color::Black);
      dot.setPosition(coord.first * tileWidth + tileWidth / 2 - characterSize / 2, 
                      coord.second * tileHeight + tileHeight / 2 - characterSize / 2);
      window.draw(dot);

      sf::Text text;
      text.setFont(font);
      text.setString(std::to_string(numCharacters));
      text.setCharacterSize(static_cast<unsigned int>(dot_size));
      text.setFillColor(sf::Color::Black);
      text.setPosition(coord.first * tileWidth + tileWidth / 2 - dot_size, 
                       coord.second * tileHeight + tileHeight / 2 - dot_size);
      window.draw(text);
    } else {
      size_t gridSize = static_cast<size_t>(std::ceil(std::sqrt(numCharacters)));
      float offsetX = (tileWidth - (gridSize - 1) * dot_size) / 2;
      float offsetY = (tileHeight - (gridSize - 1) * dot_size) / 2;

      for (size_t k = 0; k < numCharacters; k++) {
        size_t row = k / gridSize;
        size_t col = k % gridSize;
        sf::CircleShape dot(characterSize);
        dot.setFillColor(sf::Color::Black);
        dot.setPosition(coord.first * tileWidth + offsetX + col * dot_size, 
                        coord.second * tileHeight + offsetY + row * dot_size);
        window.draw(dot);

        // Draw the character's health bar
        // get the kth character in the unordered set
        size_t characterID = *std::next(pair.second.begin(), k);
        const CharacterPtr& character = model.getCharacter(characterID);
        const CharacterTraits& traits = character->getTraits();
        float healthRatio = traits.health / traits.max_health;

        sf::RectangleShape healthBarOutline(sf::Vector2f(dot_size+2, 6));
        healthBarOutline.setPosition(coord.first * tileWidth + offsetX + col * dot_size - 1, 
                                     coord.second * tileHeight + offsetY + row * dot_size - characterSize - 6);
        healthBarOutline.setFillColor(sf::Color::Black);
        window.draw(healthBarOutline);

        sf::RectangleShape healthBarGreen(sf::Vector2f(dot_size * healthRatio, 4));
        healthBarGreen.setPosition(coord.first * tileWidth + offsetX + col * dot_size, 
                                   coord.second * tileHeight + offsetY + row * dot_size - characterSize - 5);
        healthBarGreen.setFillColor(sf::Color::Green);
        window.draw(healthBarGreen);

        sf::RectangleShape healthBarRed(sf::Vector2f(dot_size * (1 - healthRatio), 4));
        healthBarRed.setPosition(coord.first * tileWidth + offsetX + col * dot_size + dot_size * healthRatio, 
                                 coord.second * tileHeight + offsetY + row * dot_size - characterSize - 5);
        healthBarRed.setFillColor(sf::Color::Red);
        window.draw(healthBarRed);
      }
    }
  }
}