#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "fs.h"

typedef struct {
    char name[MAX_NAME];
    char content[MAX_CONTENT];
    int used;
} FileEntry;

static FileEntry files[MAX_FILES];

int fs_init() {
    for (int i = 0; i < MAX_FILES; ++i) files[i].used = 0;
    return 0;
}

int fs_find(const char *name) {
    for (int i = 0; i < MAX_FILES; ++i) {
        if (files[i].used && strcmp(files[i].name, name) == 0) return i;
    }
    return -1;
}

int fs_rmfile(const char *name) {
    int idx = fs_find(name);
    if (idx == -1) return -1; // no existe

    files[idx].used = 0;
    files[idx].name[0] = '\0';
    files[idx].content[0] = '\0';
    return 0; // eliminado con exito
}


int fs_mkfile(const char *name) {
    if (fs_find(name) != -1) {
        return -1; // ya existe
    }
    for (int i = 0; i < MAX_FILES; ++i) {
        if (!files[i].used) {
            files[i].used = 1;
            strncpy(files[i].name, name, MAX_NAME-1);
            files[i].content[0] = '\0';
            return i;
        }
    }
    return -1; // lleno
}

int fs_write(const char *name, const char *content) {
    int idx = fs_find(name);
    if (idx == -1) return -1;
    strncpy(files[idx].content, content, MAX_CONTENT-1);
    return 0;
}

int fs_read(const char *name, char *outbuf, int maxlen) {
    int idx = fs_find(name);
    if (idx == -1) return -1;
    strncpy(outbuf, files[idx].content, maxlen-1);
    return 0;
}

void fs_ls() {
    printf("Archivos en VFS (Sistema de Archivos Virtual):\n");
    for (int i = 0; i < MAX_FILES; ++i) {
        if (files[i].used) {
            printf(" - %s (len=%zu)\n", files[i].name, strlen(files[i].content));
        }
    }
}

int fs_save(const char *path) {
    FILE *f = fopen(path, "wb");
    if (!f) return -1;
    int count = 0;
    for (int i = 0; i < MAX_FILES; ++i) if (files[i].used) count++;
    fwrite(&count, sizeof(int), 1, f);
    for (int i = 0; i < MAX_FILES; ++i) {
        if (!files[i].used) continue;
        int name_len = (int)strlen(files[i].name);
        int content_len = (int)strlen(files[i].content);
        fwrite(&name_len, sizeof(int), 1, f);
        fwrite(files[i].name, 1, name_len, f);
        fwrite(&content_len, sizeof(int), 1, f);
        fwrite(files[i].content, 1, content_len, f);
    }
    fclose(f);
    return 0;
}

int fs_load(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) return -1;
    for (int i = 0; i < MAX_FILES; ++i) files[i].used = 0;
    int count = 0;
    if (fread(&count, sizeof(int), 1, f) != 1) { fclose(f); return -1; }
    for (int i = 0; i < count; ++i) {
        int name_len = 0;
        fread(&name_len, sizeof(int), 1, f);
        char name_buf[MAX_NAME];
        memset(name_buf,0,sizeof(name_buf));
        fread(name_buf, 1, name_len, f);
        int content_len = 0;
        fread(&content_len, sizeof(int), 1, f);
        char content_buf[MAX_CONTENT];
        memset(content_buf,0,sizeof(content_buf));
        fread(content_buf, 1, content_len, f);
        // create file entry
        fs_mkfile(name_buf);
        fs_write(name_buf, content_buf);
    }
    fclose(f);
    return 0;
}
