#! /bin/bash

source /Users/swkim/root-6.26.08/install/bin/thisroot.sh

export INSTALL_DIR_PATH=$PWD/install

export PATH=$PATH:$INSTALL_DIR_PATH/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$INSTALL_DIR_PATH/lib
export DYLD_LIBRARY_PATH=$INSTALL_DIR_PATH/lib
export PYTHONPATH=$PYTHONPATH:$INSTALL_DIR_PATH/lib
export YAMLPATH=/Users/Shared/cvmfs/sft.cern.ch/lcg/releases/yamlcpp/0.6.3-d05b2/arm64-mac12-clang131-opt/lib