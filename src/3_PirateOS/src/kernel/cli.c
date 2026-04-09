#include "kernel/cli.h"

#include "apps/raycaster/raycaster.h"
#include "apps/song/song.h"
#include "common.h"
#include "memory/heap.h"
#include "kernel/pit.h"
#include "libc/stdio.h"

/*
 * This file is the terminal command system
 * The keyboard driver sends complete input lines here
 * This file stores command history and runs the matching command
 */

#define HISTORY_CAPACITY 32

static char *history_entries[HISTORY_CAPACITY];
static int history_count = 0;
static int history_start = 0;

static size_t cli_strlen(const char *text)
{
    size_t length = 0;

    while (text[length] != '\0') {
        length++;
    }

    return length;
}

static int cli_streq(const char *left, const char *right)
{
    size_t index = 0;

    while (left[index] != '\0' && right[index] != '\0') {
        if (left[index] != right[index]) {
            return 0;
        }

        index++;
    }

    return left[index] == right[index];
}

static int cli_startswith(const char *text, const char *prefix)
{
    size_t index = 0;

    while (prefix[index] != '\0') {
        if (text[index] != prefix[index]) {
            return 0;
        }

        index++;
    }

    return 1;
}

static const char *skip_spaces(const char *text)
{
    while (*text == ' ') {
        text++;
    }

    return text;
}

static void clear_history(void)
{
    int i;

    // Free all saved history lines and reset the history state
    for (i = 0; i < history_count; i++) {
        int slot = (history_start + i) % HISTORY_CAPACITY;

        free(history_entries[slot]);
        history_entries[slot] = 0;
    }

    history_count = 0;
    history_start = 0;
}

static void print_history(void)
{
    int i;

    // Print saved history in the same order the user sees it
    if (history_count == 0) {
        printf("History is empty.\n");
        return;
    }

    printf("Saved entries:\n");
    for (i = 0; i < history_count; i++) {
        int slot = (history_start + i) % HISTORY_CAPACITY;

        printf("%d: %s\n", i + 1, history_entries[slot]);
    }
}

static void print_help(void)
{
    printf("Commands:\n");
    printf("help         Show this help screen\n");
    printf("clear        Clear the display\n");
    printf("meminfo      Show heap and page memory info\n");
    printf("history      Show saved command history\n");
    printf("clearhistory Free saved history entries\n");
    printf("ticks        Show current PIT tick count\n");
    printf("uptime       Show uptime in milliseconds\n");
    printf("music <idx>  Play the song with index (0-8)\n");
    printf("game         Play the ASCII raycaster FPS\n");
    printf("echo <text>  Print text back to the screen\n");
    printf("about        Show kernel feature summary\n");
    printf("Keyboard:\n");
    printf("ESC          Stop playing music or exit game\n");
    printf("PgUp/PgDn    Scroll terminal history by pages\n");
    printf("Up/Down      Scroll terminal history line by line\n");
    printf("Home/End     Jump to top or bottom of scrollback\n");
}

static void print_about(void)
{
    terminal_print_logo();
    printf("Interrupts, paging, heap, PIT, keyboard history, scrollback\n");
    printf("History entries are stored on the heap.\n");
}

static void save_history_entry(const char *line)
{
    size_t length = cli_strlen(line);
    char *entry;
    int slot;
    size_t i;

    if (length == 0) {
        return;
    }

    // Save a copy of the command line on the heap
    entry = (char *)malloc(length + 1);
    if (entry == 0) {
        printf("History allocation failed.\n");
        return;
    }

    for (i = 0; i <= length; i++) {
        entry[i] = line[i];
    }

    // If the history is full, replace the oldest saved line
    if (history_count == HISTORY_CAPACITY) {
        slot = history_start;
        free(history_entries[slot]);
        history_start = (history_start + 1) % HISTORY_CAPACITY;
    } else {
        slot = (history_start + history_count) % HISTORY_CAPACITY;
        history_count++;
    }

    history_entries[slot] = entry;
}

static void execute_command(const char *command)
{
    const char *argument;

    // Compare the command text and run the right action
    if (cli_streq(command, "help")) {
        print_help();
        return;
    }

    if (cli_streq(command, "clear")) {
        terminal_initialize();
        return;
    }

    if (cli_streq(command, "meminfo")) {
        print_memory_layout();
        printf("History entries: %d of %d\n", history_count, HISTORY_CAPACITY);
        printf("Ticks: %d\n", (int)pit_get_ticks());
        return;
    }

    if (cli_streq(command, "history")) {
        print_history();
        return;
    }

    if (cli_streq(command, "clearhistory")) {
        clear_history();
        printf("History cleared.\n");
        return;
    }

    if (cli_streq(command, "ticks")) {
        printf("Ticks: %d\n", (int)pit_get_ticks());
        return;
    }

    if (cli_streq(command, "uptime")) {
        printf("Uptime: %d ms\n", (int)pit_get_ticks());
        return;
    }

    if (cli_streq(command, "about")) {
        print_about();
        return;
    }

    if (cli_startswith(command, "music")) {
        const char *arg = skip_spaces(command + 5);
        int song_index = 0;

        if (*arg == '\0') {
            printf("Usage: music <song_index>\nAvailable songs: 0-8\n");
            return;
        }

        while (*arg >= '0' && *arg <= '9') {
            song_index = song_index * 10 + (*arg - '0');
            arg++;
        }

        if (*arg != '\0') {
            printf("Invalid song index: %s\n", skip_spaces(command + 5));
            return;
        }

        play_music(song_index);
        return;
    }

    if (cli_startswith(command, "echo")) {
        argument = skip_spaces(command + 4);
        printf("%s\n", argument);
        return;
    }

    if (cli_streq(command, "game")) {
        raycaster_input_request_launch();
        printf("Starting game...\n");
        return;
    }

    printf("Unknown command: %s\n", command);
    printf("Type help to see available commands.\n");
}

void cli_print_prompt(void)
{
    printf("> ");
}

void cli_submit_line(const char *line)
{
    // Move to a new line before printing command output
    printf("\n");

    if (line[0] == '\0') {
        cli_print_prompt();
        return;
    }

    // Save the line, run it, and then show a new prompt
    save_history_entry(line);
    execute_command(line);
    cli_print_prompt();
}

void cli_handle_escape(void)
{
    // In terminal mode ESC is used as a quick stop for music
    stop_music();
}
