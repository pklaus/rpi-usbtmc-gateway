#!/bin/bash

prev_wd="$(pwd)"

# go to script dir...
cd "$( dirname "${BASH_SOURCE[0]}" )"

cd kernel-module
make
cd ..

cd servers
make
cd ..

cd userland
make
cd ..

cd "$prev_wd"
