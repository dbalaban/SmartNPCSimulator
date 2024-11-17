# Set the project name for the library
project(WorldLibrary)

# Add the include directory
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Add the source files
file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# Create the static library
add_library(${PROJECT_NAME} STATIC ${SOURCES})

# Optionally, you can specify the output directory for the library
set_target_properties(${PROJECT_NAME} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib
)