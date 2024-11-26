#include <SFML/Graphics.hpp>
#include "gridworld.hpp"
#include "gridworld_view.hpp"
#include "gridworld_controller.hpp"
#include "abstract_actor.hpp"
#include "FOMAP.hpp"
#include "StateValueEstimator.hpp"
#include "smart_actor.hpp"

int main() {
    // Create a GridWorld instance
    size_t width = 10;
    size_t height = 10;

    ResourceManager grain{Resources{200}, Resources{10}, Resources{200}};

    std::vector<ResourceManager> tile_prototypes;
    std::vector<double> weights;

    tile_prototypes.push_back(grain);
    weights.push_back(1.0);

    size_t randomSeed = 42;
    GridWorld gridWorld(width, height, tile_prototypes, weights, randomSeed);
    gridWorld.GenerateTileMap();

    // random action policy
    // auto actor = std::make_unique<RandomActor>(randomSeed);

    // smart action policy
    FOMAP fomap(1026, 1026);
    StateValueEstimator v(1026);
    auto actor = std::make_unique<SmartActor>(&gridWorld, &v, &fomap, randomSeed);

    // Add a character with action policy
    CharacterTraits traits(48000, 100, 48000, 0, 1600/24);
    auto character = std::make_unique<Character>(std::move(actor), traits);
    Coord2D coord = std::make_pair(5, 5);
    gridWorld.AddCharacter(std::move(character), coord);

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