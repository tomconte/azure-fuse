#!/bin/bash

docker run --rm -it --privileged --cap-add=ALL -v /lib/modules:/lib/modules -v $PWD:/azure-fuse azurefuse:build
