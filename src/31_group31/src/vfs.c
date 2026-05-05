#include "vfs.h"

extern void printf(const char* format, ...);

#define VFS_PATH_MAX 96
#define VFS_TYPE_DIR 1
#define VFS_TYPE_FILE 2
#define VFS_COMPLETE_ALL 0
#define VFS_COMPLETE_DIRS 1
#define VFS_COMPLETE_APPS 2

typedef struct {
    const char* path;
    const char* name;
    const char* parent;
    int type;
    int app;
    const char* content;
} VfsNode;

static char current_dir[VFS_PATH_MAX] = "/";

static const VfsNode nodes[] = {
    {"/", "/", "", VFS_TYPE_DIR, VFS_APP_NONE, ""},
    {"/bin", "bin", "/", VFS_TYPE_DIR, VFS_APP_NONE, ""},
    {"/docs", "docs", "/", VFS_TYPE_DIR, VFS_APP_NONE, ""},
    {"/assignments", "assignments", "/", VFS_TYPE_DIR, VFS_APP_NONE, ""},
    {"/assignments/5", "5", "/assignments", VFS_TYPE_DIR, VFS_APP_NONE, ""},
    {"/assignments/6", "6", "/assignments", VFS_TYPE_DIR, VFS_APP_NONE, ""},

    {"/bin/soundtest", "soundtest", "/bin", VFS_TYPE_FILE, VFS_APP_SOUNDTEST,
        "Runs a short PC speaker diagnostic melody.\n"},
    {"/bin/tone", "tone", "/bin", VFS_TYPE_FILE, VFS_APP_TONE,
        "Runs a pulsed PC speaker tone until q is pressed.\n"},

    {"/assignments/5/playlist.mp3", "playlist.mp3", "/assignments/5", VFS_TYPE_FILE, VFS_APP_PLAYLIST,
        "Assignment 5: PC speaker music playlist using PIT channel 2.\n"},
    {"/assignments/5/notes.txt", "notes.txt", "/assignments/5", VFS_TYPE_FILE, VFS_APP_NONE,
        "Assignment 5 implemented PC speaker audio with PIT timing and note delays.\n"},

    {"/assignments/6/matrix.exe", "matrix.exe", "/assignments/6", VFS_TYPE_FILE, VFS_APP_MATRIX,
        "Assignment 6: real-time Matrix rain using VGA memory, PIT timing, PRNG, and IRQ exit.\n"},
    {"/assignments/6/dodger.exe", "dodger.exe", "/assignments/6", VFS_TYPE_FILE, VFS_APP_DODGER,
        "Assignment 6: Signal Dodger mini-game with warning enemies, boss waves, and PC speaker music.\n"},
    {"/assignments/6/readme.txt", "readme.txt", "/assignments/6", VFS_TYPE_FILE, VFS_APP_NONE,
        "KebabOS Assignment 6 is presented as an interactive demo suite.\n"
        "Matrix feature: real-time rain using direct VGA writes and asynchronous IRQ exit.\n"
        "Matrix controls: ESC exit, c color, f/s speed, p pause, r reset.\n"
        "Dodger controls: a/d move, p pause, m music, ESC exit.\n"
        "Dodger hazards: ! marks an incoming elite enemy. Every 20 levels a boss attacks from the top.\n"},

    {"/docs/about.txt", "about.txt", "/docs", VFS_TYPE_FILE, VFS_APP_NONE,
        "KebabOS is Group 31's 32-bit x86 teaching OS.\n"
        "It includes a shell, read-only virtual catalog, PC speaker audio, Matrix rain, and Signal Dodger.\n"},
    {"/docs/commands.txt", "commands.txt", "/docs", VFS_TYPE_FILE, VFS_APP_NONE,
        "Commands: help, clear, pwd, ls, cd, cat, demo, tree, run.\n"
        "Run demo to print the assignment verification checklist.\n"
        "Try: cd /assignments/6, ls, cat readme.txt, run matrix.exe\n"
        "Try the mini-game too: run /assignments/6/dodger.exe\n"
        "Absolute paths also work: run /assignments/5/playlist.mp3\n"
        "System tools live in /bin: run /bin/soundtest or run /bin/tone\n"},
};

static int vfs_strcmp(const char* a, const char* b) {
    while (*a && (*a == *b)) {
        a++;
        b++;
    }
    return *(const unsigned char*)a - *(const unsigned char*)b;
}

static int vfs_starts_with(const char* text, const char* prefix) {
    while (*prefix) {
        if (*text != *prefix) return 0;
        text++;
        prefix++;
    }
    return 1;
}

static uint32_t vfs_strlen(const char* s) {
    uint32_t len = 0;
    while (s[len]) len++;
    return len;
}

static void vfs_copy(char* dst, const char* src, uint32_t out_size) {
    uint32_t i = 0;
    if (out_size == 0) return;
    while (src[i] && i < out_size - 1) {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';
}

static void vfs_append_char(char* dst, char c, uint32_t out_size) {
    uint32_t len = vfs_strlen(dst);
    if (len + 1 >= out_size) return;
    dst[len] = c;
    dst[len + 1] = '\0';
}

static void vfs_append_part(char* dst, const char* src, uint32_t len, uint32_t out_size) {
    uint32_t pos = vfs_strlen(dst);
    uint32_t i = 0;
    while (i < len && src[i] && pos + 1 < out_size) {
        dst[pos++] = src[i++];
    }
    dst[pos] = '\0';
}

static void vfs_parent(char* path) {
    uint32_t len = vfs_strlen(path);
    if (len <= 1) {
        path[0] = '/';
        path[1] = '\0';
        return;
    }

    while (len > 1 && path[len - 1] == '/') len--;
    while (len > 1 && path[len - 1] != '/') len--;
    if (len <= 1) {
        path[0] = '/';
        path[1] = '\0';
    } else {
        path[len - 1] = '\0';
    }
}

static void vfs_normalize(const char* input, char* out, uint32_t out_size) {
    uint32_t i = 0;

    if (!input || input[0] == '\0') {
        vfs_copy(out, current_dir, out_size);
        return;
    }

    if (input[0] == '/') {
        vfs_copy(out, "/", out_size);
        i = 1;
    } else {
        vfs_copy(out, current_dir, out_size);
    }

    while (input[i]) {
        while (input[i] == '/') i++;
        if (!input[i]) break;

        uint32_t start = i;
        while (input[i] && input[i] != '/') i++;
        uint32_t len = i - start;

        if (len == 1 && input[start] == '.') {
            continue;
        }
        if (len == 2 && input[start] == '.' && input[start + 1] == '.') {
            vfs_parent(out);
            continue;
        }

        if (vfs_strcmp(out, "/") != 0) vfs_append_char(out, '/', out_size);
        vfs_append_part(out, &input[start], len, out_size);
    }
}

static const VfsNode* vfs_find(const char* path) {
    uint32_t count = sizeof(nodes) / sizeof(nodes[0]);
    for (uint32_t i = 0; i < count; i++) {
        if (vfs_strcmp(nodes[i].path, path) == 0) return &nodes[i];
    }
    return 0;
}

static int vfs_is_child_of(const VfsNode* node, const char* parent) {
    return vfs_strcmp(node->parent, parent) == 0;
}

void vfs_pwd(void) {
    printf("%s\n", current_dir);
}

const char* vfs_current_dir(void) {
    return current_dir;
}

int vfs_cd(const char* path) {
    char resolved[VFS_PATH_MAX];
    vfs_normalize(path, resolved, VFS_PATH_MAX);

    const VfsNode* node = vfs_find(resolved);
    if (!node) {
        printf("cd: no such directory: %s\n", path);
        return 0;
    }
    if (node->type != VFS_TYPE_DIR) {
        printf("cd: not a directory: %s\n", path);
        return 0;
    }

    vfs_copy(current_dir, resolved, VFS_PATH_MAX);
    return 1;
}

void vfs_ls(const char* path) {
    char resolved[VFS_PATH_MAX];
    vfs_normalize(path, resolved, VFS_PATH_MAX);

    const VfsNode* node = vfs_find(resolved);
    if (!node) {
        printf("ls: cannot access '%s'\n", path && path[0] ? path : resolved);
        return;
    }

    if (node->type == VFS_TYPE_FILE) {
        printf("%s\n", node->name);
        return;
    }

    uint32_t count = sizeof(nodes) / sizeof(nodes[0]);
    for (uint32_t i = 0; i < count; i++) {
        if (vfs_is_child_of(&nodes[i], resolved)) {
            if (nodes[i].type == VFS_TYPE_DIR) printf("%s/\n", nodes[i].name);
            else printf("%s\n", nodes[i].name);
        }
    }
}

static void vfs_tree_from(const char* path, int depth) {
    uint32_t count = sizeof(nodes) / sizeof(nodes[0]);

    for (uint32_t i = 0; i < count; i++) {
        if (!vfs_is_child_of(&nodes[i], path)) continue;

        for (int j = 0; j < depth; j++) printf("  ");
        if (nodes[i].type == VFS_TYPE_DIR) printf("%s/\n", nodes[i].name);
        else printf("%s\n", nodes[i].name);

        if (nodes[i].type == VFS_TYPE_DIR) {
            vfs_tree_from(nodes[i].path, depth + 1);
        }
    }
}

void vfs_tree(const char* path) {
    char resolved[VFS_PATH_MAX];
    vfs_normalize(path, resolved, VFS_PATH_MAX);

    const VfsNode* node = vfs_find(resolved);
    if (!node) {
        printf("tree: cannot access '%s'\n", path && path[0] ? path : resolved);
        return;
    }

    if (node->type == VFS_TYPE_FILE) {
        printf("%s\n", node->name);
        return;
    }

    printf("%s\n", resolved);
    vfs_tree_from(resolved, 1);
}

void vfs_cat(const char* path) {
    char resolved[VFS_PATH_MAX];
    vfs_normalize(path, resolved, VFS_PATH_MAX);

    const VfsNode* node = vfs_find(resolved);
    if (!node) {
        printf("cat: no such file: %s\n", path);
        return;
    }
    if (node->type != VFS_TYPE_FILE) {
        printf("cat: %s is a directory\n", path);
        return;
    }
    printf("%s", node->content);
}

int vfs_resolve_app(const char* path) {
    char resolved[VFS_PATH_MAX];
    vfs_normalize(path, resolved, VFS_PATH_MAX);

    const VfsNode* node = vfs_find(resolved);
    if (!node || node->type != VFS_TYPE_FILE) return VFS_APP_NONE;
    return node->app;
}

static void split_completion_path(const char* partial, char* dir_part, char* prefix, char* display_dir) {
    int slash = -1;
    uint32_t len = vfs_strlen(partial);

    for (uint32_t i = 0; i < len; i++) {
        if (partial[i] == '/') slash = (int)i;
    }

    if (slash < 0) {
        dir_part[0] = '\0';
        display_dir[0] = '\0';
        vfs_copy(prefix, partial, VFS_PATH_MAX);
        return;
    }

    uint32_t pos = (uint32_t)slash;
    for (uint32_t i = 0; i <= pos && i < VFS_PATH_MAX - 1; i++) {
        display_dir[i] = partial[i];
    }
    display_dir[pos + 1] = '\0';

    if (pos == 0) {
        vfs_copy(dir_part, "/", VFS_PATH_MAX);
    } else {
        for (uint32_t i = 0; i < pos && i < VFS_PATH_MAX - 1; i++) {
            dir_part[i] = partial[i];
            dir_part[i + 1] = '\0';
        }
    }
    vfs_copy(prefix, &partial[pos + 1], VFS_PATH_MAX);
}

static int vfs_completion_match(const VfsNode* node, int mode) {
    if (mode == VFS_COMPLETE_DIRS) return node->type == VFS_TYPE_DIR;
    if (mode == VFS_COMPLETE_APPS) return node->type == VFS_TYPE_DIR || node->app != VFS_APP_NONE;
    return 1;
}

static int vfs_complete_node_path(const char* partial, uint32_t choice, char* out, uint32_t out_size, uint32_t* total, int mode) {
    char dir_part[VFS_PATH_MAX];
    char display_dir[VFS_PATH_MAX];
    char prefix[VFS_PATH_MAX];
    char resolved_dir[VFS_PATH_MAX];
    uint32_t count = sizeof(nodes) / sizeof(nodes[0]);
    uint32_t match_count = 0;

    split_completion_path(partial, dir_part, prefix, display_dir);
    vfs_normalize(dir_part, resolved_dir, VFS_PATH_MAX);

    for (uint32_t i = 0; i < count; i++) {
        if (!vfs_is_child_of(&nodes[i], resolved_dir)) continue;
        if (!vfs_completion_match(&nodes[i], mode)) continue;
        if (!vfs_starts_with(nodes[i].name, prefix)) continue;

        if (match_count == choice) {
            vfs_copy(out, display_dir, out_size);
            vfs_append_part(out, nodes[i].name, vfs_strlen(nodes[i].name), out_size);
            if (nodes[i].type == VFS_TYPE_DIR) vfs_append_char(out, '/', out_size);
        }
        match_count++;
    }

    if (total) *total = match_count;
    return match_count > 0;
}

int vfs_complete_path(const char* partial, uint32_t choice, char* out, uint32_t out_size, uint32_t* total, int dirs_only) {
    int mode = dirs_only ? VFS_COMPLETE_DIRS : VFS_COMPLETE_ALL;
    return vfs_complete_node_path(partial, choice, out, out_size, total, mode);
}

int vfs_complete_app_path(const char* partial, uint32_t choice, char* out, uint32_t out_size, uint32_t* total) {
    return vfs_complete_node_path(partial, choice, out, out_size, total, VFS_COMPLETE_APPS);
}
