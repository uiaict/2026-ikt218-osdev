#include "kernel/video.h"

#include "kernel/keyboard.h"

#include "kernel/pit.h"
#include "kernel/terminal.h"
#include "libc/stdio.h"
#include "starwars_data.h"

static const char* advance_one_line(const char* text) {
    if (*text == '\r') {
        text++;
        if (*text == '\n') {
            text++;
        }
    } else if (*text == '\n') {
        text++;
    }

    return text;
}

static const char* line_end(const char* text) {
    while (*text != '\0' && *text != '\n' && *text != '\r') {
        text++;
    }

    return text;
}

static bool line_is_number(const char* begin, const char* end) {
    if (begin >= end) {
        return false;
    }

    for (const char* text = begin; text < end; text++) {
        if (*text < '0' || *text > '9') {
            return false;
        }
    }

    return true;
}

static int parse_delay_ms(const char* begin, const char* end) {
    int value = 0;

    for (const char* text = begin; text < end; text++) {
        value = (value * 10) + (*text - '0');
    }

    if (value <= 0) {
        return 100;
    }

    return value * 100;
}

static void print_range(const char* begin, const char* end) {
    if (end > begin) {
        print(begin, (size_t)(end - begin));
    }
}

void show_video() {
    terminal_clear();
    const char* cursor = STARWARS_TEXT;
    int frame_delay_ms = 100;

    while (*cursor != '\0') {
        const char* line_start = cursor;
        const char* line_finish = line_end(line_start);

        if (line_is_number(line_start, line_finish)) {
            frame_delay_ms = parse_delay_ms(line_start, line_finish);
            cursor = advance_one_line(line_finish);
            continue;
        }

        const char* frame_start = line_start;
        cursor = advance_one_line(line_finish);

        while (*cursor != '\0') {
            const char* next_line_finish = line_end(cursor);
            if (line_is_number(cursor, next_line_finish)) {
                break;
            }

            cursor = advance_one_line(next_line_finish);
        }

        terminal_clear();
        print_range(frame_start, cursor);
        printf("\nPress q to quit\n");
        sleep_busy((uint32_t)frame_delay_ms);

        if (keyboard_has_input() && getchar() == 'q') {
            break;
        }
    }
}
