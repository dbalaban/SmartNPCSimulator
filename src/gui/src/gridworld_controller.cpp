#include "gridworld_controller.hpp"

void GridWorldController::handleInput(sf::RenderWindow& window) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
    }
}

void GridWorldController::update() {
    model.update(frameTime);
}