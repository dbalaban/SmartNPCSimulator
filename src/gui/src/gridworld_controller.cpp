#include "gridworld_controller.hpp"

GridWorldController::GridWorldController(GridWorld& model, GridWorldView& view)
    : model(model), view(view) {}

void GridWorldController::handleInput(sf::RenderWindow& window) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
    }
}

void GridWorldController::update() {
    // Update the model if needed
}