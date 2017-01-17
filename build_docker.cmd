docker run --rm -it --privileged --cap-add=ALL -v /lib/modules:/lib/modules -v %~dp0:/azure-fuse azurefuse:build
