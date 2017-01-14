#!/bin/bash

# Usage: azurefs <connection_string> <mount_point>

export LD_LIBRARY_PATH=/usr/local/lib

mkdir -p /azure

./bin/azurefs $1 $2 -f -s
