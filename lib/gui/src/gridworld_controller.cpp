#include "gridworld_controller.hpp"

#include "gridworld.hpp"
#include "data_writer.hpp"

GridWorldController::GridWorldController(GridWorldView& view, double frameTime) :
    view(view),
    frameTime(frameTime) {
  data_management::DataWriter::getInstance().writeData("Time Elapsed", data_management::DataType::DOUBLE, 0.0);
}

void GridWorldController::handleInput(sf::RenderWindow& window) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
    }
}

void GridWorldController::update() {
    float frameDuration = 1.0f / frameRate;
    if (clock.getElapsedTime().asSeconds() >= frameDuration) {
        GridWorld& model = GridWorld::getInstance();
        model.update(frameTime);
        timeAccumulator += frameTime;
        data_management::DataWriter::getInstance().endLine();
        data_management::DataWriter::getInstance().writeData("Time Elapsed", data_management::DataType::DOUBLE, timeAccumulator);
        view.setTimeElapsed(timeAccumulator);
        clock.restart();
    }
}