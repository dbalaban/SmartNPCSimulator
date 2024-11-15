#ifndef GRIDWORLD_CONTROLLER_HPP
#define GRIDWORLD_CONTROLLER_HPP

#include <SFML/Graphics.hpp>
#include "gridworld.hpp"
#include "gridworld_view.hpp"

class GridWorldController {
public:
    GridWorldController(GridWorld& model, GridWorldView& view);
    void handleInput(sf::RenderWindow& window);
    void update();

private:
    GridWorld& model;
    GridWorldView& view;
};

#endif // GRIDWORLD_CONTROLLER_HPP