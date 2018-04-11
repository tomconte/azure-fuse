# Azure-FUSE

**Important Note**: this project remained at the prototype stage. Please check out the [blobfuse](https://github.com/Azure/azure-storage-fuse) project for a FUSE driver developed by the Azure Storage engineering team.

This is a very early prototype of a FUSE driver for Azure Blob Storage. It uses the following frameworks:

- Microsoft Azure Storage Client Library for C++: http://azure.github.io/azure-storage-cpp/
- C++ REST SDK: http://microsoft.github.io/cpprestsdk/index.html
- libfuse: http://libfuse.github.io/doxygen/index.html

## How to build

The easiest way is to use the Docker image which has all the necessary pre-requisites. Just run:

```
docker build -t azurefuse:build  .
```

in the `build` directory to create the Docker image. Then use the `build_docker.sh` to start the container (don't forget to check the arguments to `-v` to point to your local source directory). Once inside the container:

```
cd /azure-fuse && make
```

To mount the filesystem:

```
./mount.sh <connection_string> <mount_point>
```

Replace `mount_point` by the mount point, e.g. `/azure` by default, and `connection_string` by your full Azure Storage connection string. It should be in this format:

```
DefaultEndpointsProtocol=https;AccountName=xxxxxx;AccountKey=aaabbbcccddd
```
