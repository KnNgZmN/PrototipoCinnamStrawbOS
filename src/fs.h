#ifndef FS_H
#define FS_H

#define MAX_FILES 64
#define MAX_NAME 64
#define MAX_CONTENT 2048

int fs_find(const char *name);
int fs_init();
int fs_mkfile(const char *name);
int fs_write(const char *name, const char *content);
int fs_read(const char *name, char *outbuf, int maxlen);
void fs_ls();
int fs_save(const char *path);
int fs_load(const char *path);
int fs_rmfile(const char *name);


#endif
