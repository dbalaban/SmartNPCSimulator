#ifndef GRIDWORLD_VIEW_HPP
#define GRIDWORLD_VIEW_HPP

#include <SFML/Graphics.hpp>

class GridWorldView {
public:
    GridWorldView();
    void draw(sf::RenderWindow& window);
    void setTimeElapsed(float timeElapsed);

private:
    // size of character dots
    const float characterSize = 10.0f;
    const float characterSpacing = 2.0f;
    const float max_time_elapsed;
    float timeElapsed = 0.0f;
};

#endif // GRIDWORLD_VIEW_HPP