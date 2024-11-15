#include <SFML/Graphics.hpp>
#include "gridworld.hpp"
#include "gridworld_view.hpp"
#include "gridworld_controller.hpp"

int main() {
    // Create a GridWorld instance
    size_t width = 10;
    size_t height = 10;

    ResourceManager grain{Resources{100}, Resources{10}, Resources{100}};

    std::vector<ResourceManager> tile_prototypes;
    std::vector<double> weights;

    tile_prototypes.push_back(grain);
    weights.push_back(1.0);

    size_t randomSeed = 42;
    GridWorld gridWorld(width, height, tile_prototypes, weights, randomSeed);
    gridWorld.GenerateTileMap();

    // Create the view and controller
    GridWorldView view(gridWorld);
    GridWorldController controller(gridWorld, view);

    // Create the window
    sf::RenderWindow window(sf::VideoMode(800, 800), "GridWorld");

    // Main loop
    while (window.isOpen()) {
        controller.handleInput(window);
        controller.update();

        window.clear();
        view.draw(window);
        window.display();
    }

    return 0;
}