# Set the project name for the executable
project(GridWorldApp)

# Add the include directory from the lib/world and lib/gui projects
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib/world/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib/gui/include)

# Add the source files for the executable
file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link the GuiLibrary and SFML libraries to the executable
target_link_libraries(${PROJECT_NAME} PRIVATE GuiLibrary sfml-graphics sfml-window sfml-system)