#!/bin/bash

prev_wd="$(pwd)"

# go to script dir...
cd "$( dirname "${BASH_SOURCE[0]}" )"

cd kernel-module
make
cd ..

cd cat2
make
cd ..

cd "$prev_wd"
