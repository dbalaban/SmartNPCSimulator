#include "gridworld.hpp"
#include "gridworld_view.hpp"
#include "gridworld_controller.hpp"
#include "abstract_actor.hpp"
#include "FOMAP.hpp"
#include "StateValueEstimator.hpp"
#include "smart_actor.hpp"
#include "tile.hpp"
#include "param_reader.hpp"
#include "data_writer.hpp"

int main(int argc, char** argv) {
    // takes list of config files as arguments
    ClassConfigFiles configFiles;
    for (int i = 1; i < argc; i++) {
        // get the config file name
        std::string file_path = argv[i];
        int extension_index = file_path.find_last_of('.');
        if (extension_index == std::string::npos) {
            std::cerr << "Invalid config file " << file_path << std::endl;
            continue;
        }
        int path_end = file_path.find_last_of('/');
        if (extension_index < path_end) {
            std::cerr << "Invalid config file " << file_path << std::endl;
            continue;
        }
        std::string class_name = file_path.substr(path_end+1, extension_index - path_end - 1);
        std::cout << "Found class " << class_name << " with config file " << file_path << std::endl;
        configFiles.push_back(ClassConfigFile(class_name, argv[i]));
    }
    data_management::ParamReader& reader = data_management::ParamReader::getInstance();
    reader.addConfigFiles(configFiles);

    data_management::DataWriter& writer = data_management::DataWriter::getInstance();
    std::string data_file = reader.getParam<std::string>("Data", "filename", "trial.data");
    std::string write_dir = reader.getParam<std::string>("Data", "directory", "data/raw/");
    std::string write_path = write_dir + data_file;
    writer.openFile(write_path);

    ResourceManager grain{Resources{200}, Resources{10}, Resources{200}};

    std::vector<ResourceManagerRef> tile_prototypes;
    std::vector<double> weights;

    tile_prototypes.push_back(grain);
    weights.push_back(1.0);
    GridWorld& gridWorld = GridWorld::getInstance();
    gridWorld.addTilePrototypes(tile_prototypes, weights);
    gridWorld.GenerateTileMap();

    // random action policy
    // auto actor = std::make_unique<RandomActor>(randomSeed);

    // smart action policy
    auto actor = std::make_unique<rl::SmartActor>();

    // Add a character with action policy
    CharacterTraits traits(48000, 100, 48000, 0, 1600/24);
    auto character = std::make_unique<Character>(std::move(actor), traits);
    Coord2D coord = std::make_pair(5, 5);
    gridWorld.AddCharacter(std::move(character), coord);

    // Create the view and controller
    GridWorldView view;
    GridWorldController controller(view);

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