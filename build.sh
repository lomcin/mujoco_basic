#!/bin/bash

BUILD_FOLDER=build

mkdir -p $BUILD_FOLDER && cd $BUILD_FOLDER && cmake .. && make all -j$(nproc --all)