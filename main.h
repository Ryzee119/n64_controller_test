#ifndef _MAIN_H_
#define _MAIN_H_

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>
#include <libdragon.h>


#define XMARGIN 16
#define YMARGIN 16
#define ROW(x) ((x)*8 + YMARGIN)
#define COL(y) ((y)*8 + XMARGIN)
#define MAX_ROWS ((240 - 2 * YMARGIN) / 8)
#define MAX_COLS ((320 - 2 * XMARGIN) / 8)

#define BIN8_FMT "%c%c%c%c%c%c%c%c"
#define BIN8(byte)                 \
    (byte & 0x80 ? '1' : '0'),     \
        (byte & 0x40 ? '1' : '0'), \
        (byte & 0x20 ? '1' : '0'), \
        (byte & 0x10 ? '1' : '0'), \
        (byte & 0x08 ? '1' : '0'), \
        (byte & 0x04 ? '1' : '0'), \
        (byte & 0x02 ? '1' : '0'), \
        (byte & 0x01 ? '1' : '0')

char *format_type(int accessory)
{
    switch (accessory)
    {
    case ACCESSORY_RUMBLEPAK:
        return "(rpak)";
    case ACCESSORY_MEMPAK:
        return "(mpak)";
    case ACCESSORY_VRU:
        return "(vru )";
    case ACCESSORY_TRANSFERPAK:
        return "(tpak)";
    default:
        return "(none)";
    }
}

static const char up_arrow[8]    = {0x00, 0x18, 0x3C, 0x7E, 0xFF, 0xFF, 0x00, 0x00};
static const char down_arrow[8]  = {0x00, 0xFF, 0xFF, 0x7E, 0x3C, 0x18, 0x00, 0x00};
static const char left_arrow[8]  = {0x02, 0x0E, 0x3E, 0xFE, 0x3E, 0x0E, 0x02, 0x00};
static const char right_arrow[8] = {0x80, 0xE0, 0xF8, 0xFE, 0xF8, 0xE0, 0x80, 0x00};
static const char marker[8]      = {0x00, 0x00, 0x18, 0x3C, 0x3C, 0x18, 0x00, 0x00};
static const char char_u[8]      = {0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3E, 0x00};
static const char char_s[8]      = {0x3C, 0x66, 0x70, 0x38, 0x0E, 0x66, 0x3C, 0x00};
static const char char_b[8]      = {0x7C, 0x66, 0x66, 0x7C, 0x66, 0x66, 0x7C, 0x00};
static const char char_6[8]      = {0x38, 0x60, 0xC0, 0xF8, 0xCC, 0xCC, 0x78, 0x00};
static const char char_4[8]      = {0x1C, 0x3C, 0x6C, 0xCC, 0xFE, 0x0C, 0x0C, 0x00};

static void graphics_draw_custom(display_context_t disp, int x, int y, const char *symbol, uint32_t colour, int size)
{
    for (int row = 0; row < (8 * size); row++)
    {
        unsigned char c = symbol[row / size];
        for (int col = 0; col < (8 * size); col++)
        {
            if (c << (col / size) & 0x80)
                graphics_draw_pixel(disp, x + col, y + row, colour);
        }
    }
}

#define COLOUR_WHITE graphics_make_color(255, 255, 255, 255)
#define COLOUR_RED   graphics_make_color(255, 50, 50, 255)
#define COLOUR_GREEN graphics_make_color(50, 255, 50, 255)
#define COLOUR_BLUE  graphics_make_color(128, 206, 225, 255)

static void inline set_text_colour_white()
{
    graphics_set_color(graphics_make_color(255, 255, 255, 255), 0);
}

static void inline set_text_colour_red()
{
    graphics_set_color(graphics_make_color(255, 50, 50, 255), 0);
}

static void inline set_text_colour_green()
{
    graphics_set_color(graphics_make_color(50, 255, 50, 255), 0);
}

static void inline set_text_colour_blue()
{
    graphics_set_color(graphics_make_color(128, 206, 225, 255), 0);
}

static void inline set_text_colour_yellow()
{
    graphics_set_color(graphics_make_color(253, 253, 150, 255), 0);
}

static void inline graphics_draw_coloured_text( display_context_t disp, int x, int y, const char * const msg, uint32_t colour)
{
    graphics_set_color(colour, 0);
    graphics_draw_text(disp, x, y, msg);
}

#endif