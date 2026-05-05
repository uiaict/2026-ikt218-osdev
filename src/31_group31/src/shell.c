#include "shell.h"
#include "demo.h"
#include "dodger.h"
#include "matrix.h"
#include "pit.h"
#include "programs.h"
#include "vfs.h"

extern void printf(const char* format, ...);
extern int cursor_x;
extern int cursor_y;

#define CMD_BUFFER_SIZE 256
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_ATTR_WHITE 0x0F
#define VGA_ATTR_GREEN 0x0A
#define VGA_ATTR_YELLOW 0x0E
#define VGA_ATTR_RED 0x0C
#define VGA_ATTR_GRAY 0x08
#define SPLASH_LARGE_FRAMES 6
#define SPLASH_LARGE_DELAY_MS 300
#define SPLASH_LARGE_HOLD_MS 700
#define SPLASH_RISE_DELAY_MS 110
#define SHELL_SCANCODE_ESC 0x01
#define SHELL_SCANCODE_RELEASE 0x80

static char cmd_buffer[CMD_BUFFER_SIZE];
static char last_command[CMD_BUFFER_SIZE] = "";
static char tab_seed[CMD_BUFFER_SIZE] = "";
static int cmd_index = 0;
static int tab_active = 0;
static uint32_t tab_choice = 0;
volatile int command_ready = 0;

volatile int matrix_running = 0;
volatile int song_running = 0;
volatile int dodger_running = 0;

void execute_command();

static uint32_t shell_strlen(const char* text) {
    uint32_t len = 0;
    while (text[len] != '\0') len++;
    return len;
}

static uint16_t shell_vga_entry(char c, uint8_t attr) {
    return (uint16_t)c | ((uint16_t)attr << 8);
}

static void shell_vga_set_cursor(int x, int y) {
    cursor_x = x;
    cursor_y = y;
}

static void shell_vga_clear(uint8_t attr) {
    volatile uint16_t* vga = (volatile uint16_t*)0xB8000;

    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga[i] = shell_vga_entry(' ', attr);
    }
    shell_vga_set_cursor(0, 0);
}

static void shell_vga_write_at(int row, int col, const char* text, uint8_t attr) {
    volatile uint16_t* vga = (volatile uint16_t*)0xB8000;
    int i = 0;

    if (row < 0 || row >= VGA_HEIGHT || col >= VGA_WIDTH) return;
    if (col < 0) col = 0;

    while (text[i] != '\0' && col + i < VGA_WIDTH) {
        vga[row * VGA_WIDTH + col + i] = shell_vga_entry(text[i], attr);
        i++;
    }
}

static void shell_vga_write_centered(int row, const char* text, uint8_t attr) {
    uint32_t len = shell_strlen(text);
    int col = 0;

    if (len < VGA_WIDTH) col = (VGA_WIDTH - (int)len) / 2;
    shell_vga_write_at(row, col, text, attr);
}

static void draw_large_splash(int frame) {
    static const char* title[] = {
        "88                  88                     88",
        "88   ,d8  ,adPPYba, 88,dPPYba,  ,adPPYYba, 88,dPPYba,",
        "88 ,a8\"  a8P_____88 88P'    \"8a \"\"     `Y8 88P'    \"8a",
        "8888[    8PP\"\"\"\"\"\"\" 88       d8 ,adPPPPP88 88       d8",
        "88`\"Yba, \"8b,   ,aa 88b,   ,a8\" 88,    ,88 88b,   ,a8\"",
        "88   `Y8a `\"Ybbd8\"' 8Y\"Ybbd8\"'  `\"8bbdP\"Y8 8Y\"Ybbd8\"'",
        "         ,adPPYba,  ,adPPYba,",
        "        a8\"     \"8a I8[    \"\"",
        "        8b       d8  `\"Y8ba,",
        "        \"8a,   ,a8\" aa    ]8I",
        "         `\"YbbdP\"'  `\"YbbdP\"'"
    };
    const uint8_t title_attr = (frame % 2) ? VGA_ATTR_YELLOW : VGA_ATTR_WHITE;

    shell_vga_clear(0x00);
    for (uint32_t i = 0; i < sizeof(title) / sizeof(title[0]); i++) {
        shell_vga_write_centered(1 + (int)i, title[i], title_attr);
    }

    shell_vga_write_centered(15, "Group 31 x86 teaching OS", VGA_ATTR_GREEN);
    shell_vga_write_centered(17, "Assignment demo shell", VGA_ATTR_GRAY);

    if (frame == 0) shell_vga_write_centered(22, "warming up the terminal .", VGA_ATTR_GRAY);
    else if (frame == 1) shell_vga_write_centered(22, "warming up the terminal ..", VGA_ATTR_GRAY);
    else shell_vga_write_centered(22, "warming up the terminal ...", VGA_ATTR_GRAY);
}

static void draw_compact_logo(int top_row) {
    static const char* logo[] = {
        "      ||        _        _           _        ___  ___",
        "   .-====-.    | |      | |         | |      / _ \\/ __|",
        "  /  ####  \\   | | _____| |__   __ _| |__   | (_) \\__ \\",
        "  |  ####  |   | |/ / _ \\ '_ \\ / _` | '_ \\   \\___/|___/",
        "  \\  ####  /   |   <  __/ |_) | (_| | |_) |",
        "   '-====-'    |_|\\_\\___|_.__/ \\__,_|_.__/",
        "      ||"
    };

    for (uint32_t i = 0; i < sizeof(logo) / sizeof(logo[0]); i++) {
        shell_vga_write_at(top_row + (int)i, 9, logo[i], VGA_ATTR_GREEN);
    }
}

static void draw_final_menu(void) {
    shell_vga_write_centered(10, "Group 31 x86 teaching OS", VGA_ATTR_WHITE);
    shell_vga_write_centered(12, "Type 'demo' to run the assignment checklist.", VGA_ATTR_YELLOW);
    shell_vga_write_centered(13, "Type 'help' for shell commands or 'tree /' for the virtual catalog.", VGA_ATTR_GRAY);
    shell_vga_write_centered(15, "Demos: cd /assignments/6, run matrix.exe, run dodger.exe", VGA_ATTR_WHITE);
    shell_vga_write_centered(16, "Audio: cd /assignments/5, run playlist.mp3", VGA_ATTR_GRAY);
    shell_vga_set_cursor(0, 19);
}

void shell_show_home(void) {
    for (int top = 12; top >= 2; top -= 2) {
        shell_vga_clear(0x00);
        draw_compact_logo(top);
        sleep_busy(SPLASH_RISE_DELAY_MS);
    }

    shell_vga_clear(0x00);
    draw_compact_logo(2);
    draw_final_menu();
}

static void shell_intro_animation(void) {
    for (int frame = 0; frame < SPLASH_LARGE_FRAMES; frame++) {
        draw_large_splash(frame);
        sleep_busy(SPLASH_LARGE_DELAY_MS);
    }

    sleep_busy(SPLASH_LARGE_HOLD_MS);

    shell_show_home();
}

int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

static int starts_with(const char* text, const char* prefix) {
    while (*prefix) {
        if (*text != *prefix) return 0;
        text++;
        prefix++;
    }
    return 1;
}

static void copy_string(char* dst, const char* src, int max_len) {
    int i = 0;
    while (src[i] != '\0' && i < max_len - 1) {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';
}

static void clear_current_input() {
    while (cmd_index > 0) {
        printf("\b");
        cmd_index--;
    }
}

static void write_command_to_input(const char* command) {
    clear_current_input();

    for (int i = 0; command[i] != '\0' && cmd_index < CMD_BUFFER_SIZE - 1; i++) {
        cmd_buffer[cmd_index++] = command[i];
        printf("%c", command[i]);
    }
    cmd_buffer[cmd_index] = '\0';
}

static void reset_tab_state() {
    tab_active = 0;
    tab_choice = 0;
    tab_seed[0] = '\0';
}

static int line_has_argument(const char* line, const char* command, const char** arg) {
    int i = 0;
    while (command[i] != '\0') {
        if (line[i] != command[i]) return 0;
        i++;
    }

    if (line[i] != ' ') return 0;
    while (line[i] == ' ') i++;
    *arg = &line[i];
    return 1;
}

static const char* command_arg(const char* command) {
    int i = 0;
    while (command[i] != '\0') {
        if (cmd_buffer[i] != command[i]) return 0;
        i++;
    }

    if (cmd_buffer[i] == '\0') return &cmd_buffer[i];
    if (cmd_buffer[i] != ' ') return 0;
    while (cmd_buffer[i] == ' ') i++;
    return &cmd_buffer[i];
}

static void make_line_with_arg(const char* command, const char* arg, char* out) {
    int pos = 0;
    for (int i = 0; command[i] != '\0' && pos < CMD_BUFFER_SIZE - 1; i++) {
        out[pos++] = command[i];
    }
    if (pos < CMD_BUFFER_SIZE - 1) out[pos++] = ' ';
    for (int i = 0; arg[i] != '\0' && pos < CMD_BUFFER_SIZE - 1; i++) {
        out[pos++] = arg[i];
    }
    out[pos] = '\0';
}

static int complete_command_line(const char* seed, uint32_t choice, char* out) {
    const char* commands[] = {
        "help",
        "clear",
        "pwd",
        "ls",
        "cd",
        "cat",
        "demo",
        "tree",
        "run"
    };
    uint32_t n_commands = sizeof(commands) / sizeof(commands[0]);
    uint32_t matches = 0;

    for (uint32_t i = 0; i < n_commands; i++) {
        if (!starts_with(commands[i], seed)) continue;
        if (matches == choice) copy_string(out, commands[i], CMD_BUFFER_SIZE);
        matches++;
    }

    if (matches == 0) return 0;
    if (choice >= matches) {
        return complete_command_line(seed, choice % matches, out);
    }
    return 1;
}

static int complete_path_line(const char* command, const char* arg, uint32_t choice, char* out, int dirs_only) {
    char completed_path[CMD_BUFFER_SIZE];
    uint32_t total = 0;

    if (!vfs_complete_path(arg, 0, completed_path, CMD_BUFFER_SIZE, &total, dirs_only) || total == 0) {
        return 0;
    }

    vfs_complete_path(arg, choice % total, completed_path, CMD_BUFFER_SIZE, &total, dirs_only);
    make_line_with_arg(command, completed_path, out);
    return 1;
}

static int complete_run_line(const char* arg, uint32_t choice, char* out) {
    char completed_path[CMD_BUFFER_SIZE];
    uint32_t total = 0;

    if (!vfs_complete_app_path(arg, 0, completed_path, CMD_BUFFER_SIZE, &total) || total == 0) {
        return 0;
    }

    vfs_complete_app_path(arg, choice % total, completed_path, CMD_BUFFER_SIZE, &total);
    make_line_with_arg("run", completed_path, out);
    return 1;
}

static int make_completion(const char* seed, uint32_t choice, char* out) {
    const char* arg;

    if (line_has_argument(seed, "cd", &arg)) return complete_path_line("cd", arg, choice, out, 1);
    if (line_has_argument(seed, "ls", &arg)) return complete_path_line("ls", arg, choice, out, 0);
    if (line_has_argument(seed, "cat", &arg)) return complete_path_line("cat", arg, choice, out, 0);
    if (line_has_argument(seed, "tree", &arg)) return complete_path_line("tree", arg, choice, out, 0);
    if (line_has_argument(seed, "run", &arg)) return complete_run_line(arg, choice, out);

    return complete_command_line(seed, choice, out);
}

static void print_help() {
    printf("Available commands:\n");
    printf("  help            - Show this message\n");
    printf("  clear           - Clear the screen\n");
    printf("  pwd             - Print current virtual directory\n");
    printf("  ls [path]       - List virtual files/directories\n");
    printf("  cd <path>       - Change virtual directory\n");
    printf("  cat <file>      - Print a virtual file\n");
    printf("  demo            - Run assignment verification checklist\n");
    printf("  tree [path]     - Show the virtual file tree\n");
    printf("  run <program>   - Run executable in current dir or by path\n");
    printf("  /bin tools      - cd /bin, then run soundtest or run tone\n");
}

static void run_program(const char* target) {
    int app = VFS_APP_NONE;

    if (target[0] == '\0') {
        printf("Usage: run <program>\n");
        printf("Try: cd /assignments/6 then run matrix.exe\n");
        printf("Or: run /assignments/5/playlist.mp3\n");
        return;
    }

    app = vfs_resolve_app(target);

    if (app == VFS_APP_NONE) {
        printf("run: not an executable in this directory: %s\n", target);
        return;
    }

    program_run_app(app);
}

void print_prompt() {
    printf("kebab%s> ", vfs_current_dir());
}

void shell_init() {
    shell_intro_animation();
    print_prompt();
}

void shell_update() {
    if (command_ready) {
        execute_command();
        command_ready = 0;
    }
}

void execute_command() {
    if (cmd_index == 0) return;
    cmd_buffer[cmd_index] = '\0';
    printf("\n");

    for (int i = 0; i <= cmd_index; i++) {
        last_command[i] = cmd_buffer[i];
    }

    const char* arg;

    if (strcmp(cmd_buffer, "help") == 0) {
        print_help();
    } else if (strcmp(cmd_buffer, "clear") == 0) {
        shell_show_home();
    } else if (strcmp(cmd_buffer, "pwd") == 0) {
        vfs_pwd();
    } else if ((arg = command_arg("ls"))) {
        vfs_ls(arg);
    } else if ((arg = command_arg("cd"))) {
        if (arg[0] == '\0') vfs_cd("/");
        else vfs_cd(arg);
    } else if ((arg = command_arg("cat"))) {
        if (arg[0] == '\0') printf("Usage: cat <file>\n");
        else vfs_cat(arg);
    } else if (strcmp(cmd_buffer, "demo") == 0) {
        run_demo_checklist();
        shell_show_home();
    } else if ((arg = command_arg("tree"))) {
        vfs_tree(arg);
    } else if ((arg = command_arg("run"))) {
        run_program(arg);
    } else {
        printf("Unknown command: %s\n", cmd_buffer);
    }

    cmd_index = 0;
    reset_tab_state();
    print_prompt();
}

void shell_handle_keypress(char c) {
    if (demo_running) {
        return;
    }

    if (dodger_running) {
        dodger_handle_key(c);
        return;
    }

    if (matrix_running) {
        matrix_handle_key(c);
        return;
    }

    if (song_running) {
        if (c == 'q' || c == 'Q') {
            song_running = 0;
        }
        return;
    }

    if (c == '\n') {
        reset_tab_state();
        if (!command_ready) command_ready = 1;
    } else if (c == '\b') {
        reset_tab_state();
        if (cmd_index > 0) {
            cmd_index--;
            printf("\b"); // kernel.c'deki putchar'imiz ekrandan harfi silecek
        }
    } else if (c == '\t') {
        char completion[CMD_BUFFER_SIZE];
        cmd_buffer[cmd_index] = '\0';

        if (!tab_active) {
            copy_string(tab_seed, cmd_buffer, CMD_BUFFER_SIZE);
            tab_choice = 0;
            tab_active = 1;
        } else {
            tab_choice++;
        }

        if (make_completion(tab_seed, tab_choice, completion)) {
            write_command_to_input(completion);
        }
    } else if (c == 17) {
        reset_tab_state();
        if (last_command[0] != '\0') {
            while(cmd_index > 0) { printf("\b"); cmd_index--; }
            for(int i=0; last_command[i] != '\0'; i++) {
                cmd_buffer[cmd_index++] = last_command[i];
                printf("%c", last_command[i]);
            }
        }
    } else if (c == 18) {
        reset_tab_state();
        while(cmd_index > 0) { printf("\b"); cmd_index--; }
    } else if (c != '?') {
        reset_tab_state();
        if (cmd_index < CMD_BUFFER_SIZE - 1) {
            cmd_buffer[cmd_index++] = c;
            printf("%c", c);
        }
    }
}

int shell_handle_scancode(uint8_t scancode) {
    uint8_t code = scancode & 0x7F;
    int released = (scancode & SHELL_SCANCODE_RELEASE) != 0;

    if (demo_running) {
        if (code == SHELL_SCANCODE_ESC && !released) demo_running = 0;
        return 1;
    }

    if (code == SHELL_SCANCODE_ESC && !released) {
        if (dodger_running) {
            dodger_running = 0;
            return 1;
        }
        if (matrix_running) {
            matrix_running = 0;
            return 1;
        }
        if (song_running) {
            song_running = 0;
            return 1;
        }
    }

    if (dodger_running) {
        dodger_handle_scancode(scancode);
        return 1;
    }

    return 0;
}
