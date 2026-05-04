#ifndef VFS_H
#define VFS_H

#include "stdint.h"

#define VFS_APP_NONE 0
#define VFS_APP_MATRIX 1
#define VFS_APP_PLAYLIST 2
#define VFS_APP_SOUNDTEST 3
#define VFS_APP_TONE 4
#define VFS_APP_DODGER 5

void vfs_pwd(void);
const char* vfs_current_dir(void);
int vfs_cd(const char* path);
void vfs_ls(const char* path);
void vfs_tree(const char* path);
void vfs_cat(const char* path);
int vfs_resolve_app(const char* path);
int vfs_complete_path(const char* partial, uint32_t choice, char* out, uint32_t out_size, uint32_t* total, int dirs_only);
int vfs_complete_app_path(const char* partial, uint32_t choice, char* out, uint32_t out_size, uint32_t* total);

#endif
