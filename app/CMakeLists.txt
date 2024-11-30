# Set the project name for the executable
project(GridWorldApp)

# Add the include directory from the lib/world and lib/gui projects
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib/world/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib/gui/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib/rl/include)

# Create the executable
add_executable(${PROJECT_NAME} gridworld_test.cpp)

# Link the GuiLibrary and SFML libraries to the executable
target_link_libraries(${PROJECT_NAME} PUBLIC
${YAML_LIBRARIES}
${TORCH_LIBRARIES}
WorldLibrary
GuiLibrary
RLLibrary 
sfml-graphics 
sfml-window 
sfml-system
)