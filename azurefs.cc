#include "azurefs.hh"

ExampleFS* ExampleFS::_instance = NULL;

#define RETURN_ERRNO(x) (x) == 0 ? 0 : -errno

ExampleFS* ExampleFS::Instance() {
  if(_instance == NULL) {
    _instance = new ExampleFS();
  }
  return _instance;
}

ExampleFS::ExampleFS() {

}

ExampleFS::~ExampleFS() {

}

void ExampleFS::AbsPath(char dest[PATH_MAX], const char *path) {
  strncpy(dest, path, PATH_MAX);
  //printf("translated path: %s to %s\n", path, dest);
}

void ExampleFS::setAzureOptions(const char *connectionString) {
  printf("connecting to: %s\n", connectionString);
  _connectionString = connectionString;
  _storage_account = azure::storage::cloud_storage_account::parse(_connectionString);
  _blob_client = _storage_account.create_cloud_blob_client();
}

int ExampleFS::Getattr(const char *path, struct stat *statbuf) {
  printf("getattr(%s)\n", path);
  memset(statbuf, 0, sizeof(struct stat));
  if (strcmp(path, "/") == 0) {
    // Root director
    statbuf->st_mode = S_IFDIR | 0755;
    statbuf->st_nlink = 2;
  } else if (strrchr(path, '/') == path) {
    // Only one slash at the beginning: this is a container (root-level directory)
    statbuf->st_mode = S_IFDIR | 0755;
    statbuf->st_nlink = 2;
  } else {
    // More than one slash: this is a blob
    // TODO: handle folders and subfolders...
    char containerName[64], blobName[1024];
    const char *slash = strchr(path+1, '/');
    int slashPos = slash-path-1;

    strcpy(blobName, slash);
    strncpy(containerName, path+1, slashPos);
    containerName[slashPos] = '\0';

    printf("check container=%s blob=%s\n", containerName, blobName);

    azure::storage::cloud_blob_container container = _blob_client.get_container_reference(containerName);
    azure::storage::cloud_blob blob = container.get_blob_reference(blobName);
    blob.download_attributes();
    
    statbuf->st_mode = S_IFREG | 0755;
    statbuf->st_nlink = 1;
    statbuf->st_size = blob.properties().size();
  }
  return 0;
}

int ExampleFS::Readlink(const char *path, char *link, size_t size) {
  printf("readlink(path=%s, link=%s, size=%d)\n", path, link, (int)size);
  char fullPath[PATH_MAX];
  AbsPath(fullPath, path);
  return RETURN_ERRNO(readlink(fullPath, link, size));
}

int ExampleFS::Mknod(const char *path, mode_t mode, dev_t dev) {
  printf("mknod(path=%s, mode=%d)\n", path, mode);
  char fullPath[PATH_MAX];
  AbsPath(fullPath, path);
	
  //handles creating FIFOs, regular files, etc...
  return RETURN_ERRNO(mknod(fullPath, mode, dev));
}

int ExampleFS::Mkdir(const char *path, mode_t mode) {
  printf("**mkdir(path=%s, mode=%d)\n", path, (int)mode);
  char fullPath[PATH_MAX];
  AbsPath(fullPath, path);
  return RETURN_ERRNO(mkdir(fullPath, mode));
}

int ExampleFS::Unlink(const char *path) {
  printf("unlink(path=%s\n)", path);
  char fullPath[PATH_MAX];
  AbsPath(fullPath, path);
  return RETURN_ERRNO(unlink(fullPath));
}

int ExampleFS::Rmdir(const char *path) {
  printf("rmkdir(path=%s\n)", path);
  char fullPath[PATH_MAX];
  AbsPath(fullPath, path);
  return RETURN_ERRNO(rmdir(fullPath));
}

int ExampleFS::Symlink(const char *path, const char *link) {
  printf("symlink(path=%s, link=%s)\n", path, link);
  char fullPath[PATH_MAX];
  AbsPath(fullPath, path);
  return RETURN_ERRNO(symlink(fullPath, link));
}

int ExampleFS::Rename(const char *path, const char *newpath) {
  printf("rename(path=%s, newPath=%s)\n", path, newpath);
  char fullPath[PATH_MAX];
  AbsPath(fullPath, path);
  return RETURN_ERRNO(rename(fullPath, newpath));
}

int ExampleFS::Link(const char *path, const char *newpath) {
  printf("link(path=%s, newPath=%s)\n", path, newpath);
  char fullPath[PATH_MAX];
  char fullNewPath[PATH_MAX];
  AbsPath(fullPath, path);
  AbsPath(fullNewPath, newpath);
  return RETURN_ERRNO(link(fullPath, fullNewPath));
}

int ExampleFS::Chmod(const char *path, mode_t mode) {
  printf("chmod(path=%s, mode=%d)\n", path, mode);
  char fullPath[PATH_MAX];
  AbsPath(fullPath, path);
  return RETURN_ERRNO(chmod(fullPath, mode));
}

int ExampleFS::Chown(const char *path, uid_t uid, gid_t gid) {
  printf("chown(path=%s, uid=%d, gid=%d)\n", path, (int)uid, (int)gid);
  char fullPath[PATH_MAX];
  AbsPath(fullPath, path);
  return RETURN_ERRNO(chown(fullPath, uid, gid));
}

int ExampleFS::Truncate(const char *path, off_t newSize) {
  printf("truncate(path=%s, newSize=%d\n", path, (int)newSize);
  char fullPath[PATH_MAX];
  AbsPath(fullPath, path);
  return RETURN_ERRNO(truncate(fullPath, newSize));
}

int ExampleFS::Utime(const char *path, struct utimbuf *ubuf) {
  printf("utime(path=%s)\n", path);
  char fullPath[PATH_MAX];
  AbsPath(fullPath, path);
  return RETURN_ERRNO(utime(fullPath, ubuf));
}

int ExampleFS::Open(const char *path, struct fuse_file_info *fileInfo) {
  printf("open(path=%s)\n", path);
  // Check that blob exists
  return 0;
}

int ExampleFS::Read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo) {
  printf("read(path=%s, size=%d, offset=%d)\n", path, (int)size, (int)offset);

  char containerName[64], blobName[1024];
  const char *slash = strchr(path+1, '/');
  int slashPos = slash-path-1;

  strcpy(blobName, slash);
  strncpy(containerName, path+1, slashPos);
  containerName[slashPos] = '\0';

  printf("download from container=%s blob=%s\n", containerName, blobName);

  azure::storage::cloud_blob_container container = _blob_client.get_container_reference(containerName);
  azure::storage::cloud_blob blob = container.get_blob_reference(blobName);
  blob.download_attributes();

  if (offset >= blob.properties().size()) {
    return 0;
  }
  
  concurrency::streams::rawptr_buffer<unsigned char> buffer((unsigned char *)buf, size);
  concurrency::streams::ostream stream = buffer.create_ostream();
  blob.download_range_to_stream(stream, offset, size);
  size = stream.tell();
  
  stream.close().wait();
  buffer.close().wait();

  printf("read %d bytes\n", (int)size);

  return size;
}

int ExampleFS::Write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo) {
  printf("write(path=%s, size=%d, offset=%d)\n", path, (int)size, (int)offset);
  return RETURN_ERRNO(pwrite(fileInfo->fh, buf, size, offset));
}

int ExampleFS::Statfs(const char *path, struct statvfs *statInfo) {
  printf("statfs(path=%s)\n", path);
  char fullPath[PATH_MAX];
  AbsPath(fullPath, path);
  return RETURN_ERRNO(statvfs(fullPath, statInfo));
}

int ExampleFS::Flush(const char *path, struct fuse_file_info *fileInfo) {
  printf("flush(path=%s)\n", path);
  return 0;
}

int ExampleFS::Release(const char *path, struct fuse_file_info *fileInfo) {
  printf("release(path=%s)\n", path);
  return 0;
}

int ExampleFS::Fsync(const char *path, int datasync, struct fuse_file_info *fi) {
  printf("fsync(path=%s, datasync=%d\n", path, datasync);
  if(datasync) {
    //sync data only
    return RETURN_ERRNO(fdatasync(fi->fh));
  } else {
    //sync data + file metadata
    return RETURN_ERRNO(fsync(fi->fh));
  }
}

int ExampleFS::Setxattr(const char *path, const char *name, const char *value, size_t size, int flags) {
  printf("setxattr(path=%s, name=%s, value=%s, size=%d, flags=%d\n",
	 path, name, value, (int)size, flags);
  char fullPath[PATH_MAX];
  AbsPath(fullPath, path);
  return RETURN_ERRNO(lsetxattr(fullPath, name, value, size, flags));
}

int ExampleFS::Getxattr(const char *path, const char *name, char *value, size_t size) {
  printf("getxattr(path=%s, name=%s, size=%d\n", path, name, (int)size);
  char fullPath[PATH_MAX];
  AbsPath(fullPath, path);
  return RETURN_ERRNO(getxattr(fullPath, name, value, size));
}

int ExampleFS::Listxattr(const char *path, char *list, size_t size) {
  printf("listxattr(path=%s, size=%d)\n", path, (int)size);
  char fullPath[PATH_MAX];
  AbsPath(fullPath, path);
  return RETURN_ERRNO(llistxattr(fullPath, list, size));
}

int ExampleFS::Removexattr(const char *path, const char *name) {
  printf("removexattry(path=%s, name=%s)\n", path, name);
  char fullPath[PATH_MAX];
  AbsPath(fullPath, path);
  return RETURN_ERRNO(lremovexattr(fullPath, name));
}

int ExampleFS::Opendir(const char *path, struct fuse_file_info *fileInfo) {
  printf("opendir(path=%s)\n", path);
  return 0;
}

int ExampleFS::Readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fileInfo) {
  printf("readdir(path=%s, offset=%d)\n", path, (int)offset);
  if (strcmp(path, "/") == 0) {
    // Root directory, return list of containers
    for (azure::storage::cloud_blob_container container : _blob_client.list_containers()) {
      if (filler(buf, container.name().c_str(), NULL, 0) !=0) {
	return -ENOMEM;
      }
    }
  } else {
    // List blobs in a container
    azure::storage::cloud_blob_container container = _blob_client.get_container_reference(path+1);
    for (azure::storage::list_blob_item blob : container.list_blobs()) {
      if (filler(buf, blob.as_blob().name().c_str(), NULL, 0) !=0) {
	return -ENOMEM;
      }
    }
  }    
  return 0;
}

int ExampleFS::Releasedir(const char *path, struct fuse_file_info *fileInfo) {
  printf("releasedir(path=%s)\n", path);
  return 0;
}

int ExampleFS::Fsyncdir(const char *path, int datasync, struct fuse_file_info *fileInfo) {
  return 0;
}

int ExampleFS::Init(struct fuse_conn_info *conn) {
  return 0;
}

int ExampleFS::Truncate(const char *path, off_t offset, struct fuse_file_info *fileInfo) {
  printf("truncate(path=%s, offset=%d)\n", path, (int)offset);
  char fullPath[PATH_MAX];
  AbsPath(fullPath, path);
  return RETURN_ERRNO(ftruncate(fileInfo->fh, offset));
}


