#ifndef GRIDWORLD_VIEW_HPP
#define GRIDWORLD_VIEW_HPP

#include <SFML/Graphics.hpp>
#include "gridworld.hpp"

class GridWorldView {
public:
    GridWorldView(GridWorld& model);
    void draw(sf::RenderWindow& window);

private:
    GridWorld& model;
};

#endif // GRIDWORLD_VIEW_HPP