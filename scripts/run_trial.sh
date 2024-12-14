#!/bin/bash

# Define the executable and its arguments
EXECUTABLE="./bin/GridWorldApp"
ARGS="config/Data.yaml config/FOMAP.yaml config/SmartActor.yaml config/GridWorld.yaml config/StateValueEstimator.yaml"

# Check if the first argument is "valgrind"
if [ "$1" == "valgrind" ]; then
    # Run the application with Valgrind to check for memory corruption and write the output to valgrind_output.txt
    valgrind --track-origins=yes --log-file=valgrind_output.txt --read-var-info=yes --show-reachable=yes --undef-value-errors=yes $EXECUTABLE $ARGS
elif [ "$1" == "gdb" ]; then
    # Run the application with GDB and set watchpoints
    gdb -ex "break ParamReader::addConfigFiles" \
        -ex "run $ARGS" \
        -ex "set \$paramReader = &ParamReader::getInstance()" \
        -ex "set \$configAddress = &\$paramReader->config" \
        -ex "watch *\$configAddress" \
        -ex "continue" \
        --args $EXECUTABLE $ARGS
else
    # Run the application normally
    $EXECUTABLE $ARGS
fi