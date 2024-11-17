# Set the project name for the library
project(GuiLibrary)

# Add the include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src/world/include) # Include the WorldLibrary headers

# Add the source files
file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# Find the required packages
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# Create the static library
add_library(${PROJECT_NAME} STATIC ${SOURCES})

# Ensure WorldLibrary is built before GuiLibrary
add_dependencies(${PROJECT_NAME} WorldLibrary)

# Link the required libraries
target_link_libraries(${PROJECT_NAME} PUBLIC sfml-graphics sfml-window sfml-system WorldLibrary)

# Optionally, you can specify the output directory for the library
set_target_properties(${PROJECT_NAME} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib
)