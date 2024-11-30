#ifndef GRIDWORLD_CONTROLLER_HPP
#define GRIDWORLD_CONTROLLER_HPP

#include <SFML/Graphics.hpp>
#include "gridworld_view.hpp"

class GridWorldController {
public:
    GridWorldController(GridWorldView& view, double frameTime = 1.0);
    void handleInput(sf::RenderWindow& window);
    void update();

private:
    GridWorldView& view;
    // hours per frame
    const double frameTime;
    const double frameRate = 10.0;
    double timeAccumulator = 0.0;
    sf::Clock clock;
};

#endif // GRIDWORLD_CONTROLLER_HPP