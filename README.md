# FantasySimulator
Experimenting with Reinforcement Learning in an RPG-like fantasy world

Required Installs:

libsfml-dev, libyaml-dev, g++, pkg-config

unzip libtorch in \<top level\>/third_party: https://pytorch.org/cppdocs/installing.html


# Building / Running
Build with cmake:

    mkdir build
    cd build
    cmake ..
    make
  
Can use the run_trial.sh script to run from top level:

    ./run_trial # basic run
    ./run_trial valgrind # run with valgrind
    ./run_trial gdb # open gdb debug session
