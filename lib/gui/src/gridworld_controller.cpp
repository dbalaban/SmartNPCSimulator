#include "gridworld_controller.hpp"

#include "gridworld.hpp"
#include "data_writer.hpp"

GridWorldController::GridWorldController(GridWorldView& view, double frameTime) :
    view(view),
    frameTime(frameTime) {}

void GridWorldController::handleInput(sf::RenderWindow& window) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
    }
}

bool GridWorldController::update() {
    float frameDuration = 1.0f / frameRate;
    if (clock.getElapsedTime().asSeconds() >= frameDuration) {
        GridWorld& model = GridWorld::getInstance();
        data_management::DataWriter& writer = data_management::DataWriter::getInstance();
        model.update(frameTime);
        timeAccumulator += frameTime;
        writer.endLine();
        if (!model.hasLivingCharacters()) {
            return false;
        }
        writer.writeData("Time Elapsed", data_management::DataType::DOUBLE, timeAccumulator);
        view.setTimeElapsed(timeAccumulator);
        clock.restart();
    }
}