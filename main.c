#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>
#include <libdragon.h>
#include "main.h"

static resolution_t res = RESOLUTION_320x240;
static bitdepth_t bit = DEPTH_32_BPP;

void draw_page_1(display_context_t disp);
void draw_page_2(display_context_t disp);
void draw_page_3(display_context_t disp);
void draw_page_4(display_context_t disp);

int main(void)
{
    /* Enable interrupts (on the CPU) */
    init_interrupts();

    /* Initialize Random number Seed */
    srand(64);

    /* Initialize peripherals */
    display_init(res, bit, 2, GAMMA_NONE, ANTIALIAS_OFF);
    console_init();
    controller_init();
    controller_scan();

    /* Initialize renderer */
    console_set_render_mode(RENDER_MANUAL);

    int page = 0;
    while (1)
    {
        static display_context_t disp = 0;
        while (!(disp = display_lock()));
        graphics_fill_screen(disp, 0);

        /* Handle global controller inputs and text */
        controller_scan();
        struct controller_data keys_down = get_keys_down();
        struct controller_data keys_now = get_keys_pressed();
        for (int i = 0; i < 4; i++)
        {
            if (keys_down.c[i].data == 0xFFFFFFFF || keys_now.c[i].data == 0xFFFFFFFF)
                continue;
            if(keys_now.c[i].Z && keys_down.c[i].left  && page > 0)
                page--;
            if(keys_now.c[i].Z && keys_down.c[i].right && page < 2)
                page++;
        }

        graphics_draw_custom(disp, COL(0), ROW(0), char_u, COLOUR_WHITE, 2);
        graphics_draw_custom(disp, COL(2), ROW(0), char_s, COLOUR_WHITE, 2);
        graphics_draw_custom(disp, COL(4), ROW(0), char_b, COLOUR_WHITE, 2);
        graphics_draw_custom(disp, COL(6), ROW(0), char_6, COLOUR_WHITE, 2);
        graphics_draw_custom(disp, COL(8), ROW(0), char_4, COLOUR_WHITE, 2);
        graphics_draw_coloured_text(disp, COL(0), ROW(MAX_ROWS) - ROW(0), "Z+ / : CHANGE PAGE", COLOUR_WHITE);
        graphics_draw_custom(disp, COL(2), ROW(MAX_ROWS) - ROW(0), left_arrow, COLOUR_WHITE, 1);
        graphics_draw_custom(disp, COL(4), ROW(MAX_ROWS) - ROW(0), right_arrow, COLOUR_WHITE, 1);

        /* Draw pages as required */
        switch (page)
        {
            case  0: draw_page_1(disp); break;
            case  1: draw_page_2(disp); break;
            case  2: draw_page_3(disp); break;
            case  3: draw_page_4(disp); break;
            default: draw_page_1(disp); break;
        }
        display_show(disp);
    }
}

void draw_page_1(display_context_t disp)
{
    graphics_draw_coloured_text(disp, COL(0), ROW(3), "CONTROLLER INPUT TEST", COLOUR_WHITE);

    char text_buff[80];

    /* Draw the constant screen elements */
    graphics_draw_custom(disp, COL(18), ROW(4), up_arrow, COLOUR_WHITE, 1);
    graphics_draw_custom(disp, COL(19), ROW(4), down_arrow, COLOUR_WHITE, 1);
    graphics_draw_custom(disp, COL(20), ROW(4), left_arrow, COLOUR_WHITE, 1);
    graphics_draw_custom(disp, COL(21), ROW(4), right_arrow, COLOUR_WHITE, 1);
    graphics_draw_custom(disp, COL(26), ROW(4), up_arrow, COLOUR_WHITE, 1);
    graphics_draw_custom(disp, COL(27), ROW(4), down_arrow, COLOUR_WHITE, 1);
    graphics_draw_custom(disp, COL(28), ROW(4), left_arrow, COLOUR_WHITE, 1);
    graphics_draw_custom(disp, COL(29), ROW(4), right_arrow, COLOUR_WHITE, 1);
    graphics_draw_coloured_text(disp, COL(14), ROW(5), "ABZSDDDD  LRCCCC", COLOUR_WHITE);

    /* Draw the analog stick octagonal map */
    const int s = 50; //size of map
    const int x_0 = 320 - s - COL(0); //Position of map datum x
    const int y_0 = 240 - s - ROW(0); //Position of map datum y

    //Create the octagon poly that represents roughly what a N64 stick
    //should output.
    const int _xy_0  = s * 1.00f; //Magnitude of 0/90 degree poly vertices from datum
    const int _xy_45 = s * 0.78f; //Magnitude of 45 degree poly vertices from datum
    const int poly[8][4] = { //Create the octagon poly
        { _xy_0,      0,  _xy_45, -_xy_45},
        {     0, -_xy_0,  _xy_45, -_xy_45},
        {     0, -_xy_0, -_xy_45, -_xy_45},
        {-_xy_0,      0, -_xy_45, -_xy_45},
        {-_xy_0,      0, -_xy_45,  _xy_45},
        {     0,  _xy_0, -_xy_45,  _xy_45},
        {     0,  _xy_0,  _xy_45,  _xy_45},
        { _xy_0,      0,  _xy_45,  _xy_45}
    };
    const int col = graphics_make_color(100, 100, 100, 255);

    //Draw axis and octagonal poly
    graphics_draw_line(disp, x_0, y_0 - s, x_0, y_0 + s, col);
    graphics_draw_line(disp, x_0 - s, y_0, x_0 + s, y_0, col);
    for(int i = 0; i < 8; i++)
    {
        graphics_draw_line(disp, poly[i][0] + x_0,
                                 poly[i][1] + y_0,
                                 poly[i][2] + x_0,
                                 poly[i][3] + y_0, col); 
    }

    //Draw rumble helper
    graphics_draw_coloured_text (disp, COL(0), ROW(MAX_ROWS) - ROW(1), "Z+A/B: RUMBLE", COLOUR_WHITE);

    /* Draw the variable screen elements for each controller*/
    for (int c = 0; c < 4; c++)
    {
        //Draw the controller status and what peripheral is connected
        int controllers = get_controllers_present();
        (controllers & (0xF000 >> (c * 4))) ? set_text_colour_green() : set_text_colour_red();
        sprintf(text_buff, "Cont %u", c);
        graphics_draw_text(disp, COL(0), ROW(c + 6), text_buff);
        graphics_draw_text(disp, COL(7), ROW(c + 6), format_type(identify_accessory(c)));

        //Get the current analog stick position
        struct controller_data keys_now = get_keys_pressed();
        struct controller_data keys_down = get_keys_down();
        if (keys_now.c[c].data == 0xFFFFFFFF || keys_down.c[c].data == 0xFFFFFFFF)
            continue;
        int x_pos = keys_now.c[c].x * s / 82.0f;
        int y_pos = keys_now.c[c].y * s / 82.0f;
        if (x_pos >  s * 1.3) x_pos =  s * 1.3;
        if (x_pos < -s * 1.3) x_pos = -s * 1.3;
        if (y_pos >  s * 1.3) y_pos =  s * 1.3;
        if (y_pos < -s * 1.3) y_pos = -s * 1.3;

        if(keys_now.c[c].Z && keys_down.c[c].A)
            rumble_start(c);
        if(keys_now.c[c].Z && keys_down.c[c].B)
            rumble_stop(c);

        //If this controller is connected
        if (controllers & (0xF000 >> (c * 4)))
        {
            //Draw binary representation of digital buttons
            sprintf(text_buff + 0, BIN8_FMT, BIN8(keys_now.c[c].data >> 24 & 0xFF));
            sprintf(text_buff + 8, BIN8_FMT, BIN8(keys_now.c[c].data >> 16 & 0xFF));
            graphics_draw_coloured_text(disp, COL(14), ROW(c + 6), text_buff, COLOUR_WHITE);

            //Draw the X,Y pos and a marker for the current analog stick position
            sprintf(text_buff, "Cont %u x:%4i y:%4i", c, keys_now.c[c].x, keys_now.c[c].y);
            graphics_draw_coloured_text(disp, COL(0), ROW(c + 6 + 5), text_buff, COLOUR_WHITE);
            graphics_draw_custom(disp, x_0 + x_pos - 4, y_0 - y_pos - 4, marker, COLOUR_WHITE, 1);
        }
    }
}
void draw_page_2(display_context_t disp)
{
    graphics_draw_coloured_text(disp, COL(0), ROW(3), "RAW MEMORY TEST", COLOUR_WHITE);

    char text_buff[80];
    static int cursor_row = 0;

    /* Read/Write status flags */
    static uint16_t read_address = 0;
    static uint8_t  read_packet[32 * 4] = {0};
    static int      read_status = 1;

    static uint16_t write_address = 0;
    static uint8_t  write_packet[32] = {0};
    static int      write_status = 1;

    /* Set rows for Read items */
    const int READ_PAK_START   = 5;
    const int READ_PAK_ADDRESS = READ_PAK_START + 0;
    const int READ_PAK_PACKET  = READ_PAK_START + 1;
    const int READ_PAK_RESULT  = READ_PAK_START + 9;

    /* Set rows for Write items */
    const int WRITE_PAK_START   = 16;
    const int WRITE_PAK_WARNING = WRITE_PAK_START + 0;
    const int WRITE_PAK_RANDOM  = WRITE_PAK_START + 1;
    const int WRITE_PAK_PACKET  = WRITE_PAK_START + 2;
    const int WRITE_PAK_ADDRESS = WRITE_PAK_START + 4;
    const int WRITE_PAK_RESULT  = WRITE_PAK_START + 5;

    /* READ PAK TESTS */
    sprintf(text_buff, "READ [0x%04x to 0x%04x]", read_address, read_address + sizeof(read_packet) - 1);
    graphics_draw_coloured_text(disp, COL(1), ROW(READ_PAK_ADDRESS), text_buff, COLOUR_WHITE);
    for (int i = 0; i < sizeof(read_packet); i++)
    {
        sprintf(text_buff, "%02x", read_packet[i]);
        graphics_draw_coloured_text(disp, COL((2 * i) % (16 * 2) + 1),
                                    ROW(READ_PAK_PACKET + i / 16), text_buff, COLOUR_BLUE);
    }
    graphics_draw_coloured_text(disp, COL(1), ROW(READ_PAK_RESULT), "RESULT: ", COLOUR_BLUE);

    /* WRITE PAK TEST */
    graphics_draw_coloured_text(disp, COL(1), ROW(WRITE_PAK_WARNING), "WARNING CORRUPTION MAY OCCUR", COLOUR_RED);

    sprintf(text_buff, "WRITE PACKET TO: 0x%04x", write_address);
    graphics_draw_coloured_text(disp, COL(1), ROW(WRITE_PAK_ADDRESS), text_buff, COLOUR_WHITE);
    for (int i = 0; i < 32; i++)
    {
        sprintf(text_buff, "%02x", write_packet[i]);
        graphics_draw_coloured_text(disp, COL((2 * i) % (16 * 2) + 1),
                                    ROW(WRITE_PAK_PACKET + i / 16), text_buff, COLOUR_BLUE);
    }
    graphics_draw_coloured_text(disp, COL(1), ROW(WRITE_PAK_RANDOM), "RANDOM", COLOUR_WHITE);
    graphics_draw_coloured_text(disp, COL(1), ROW(WRITE_PAK_RESULT), "RESULT: ", COLOUR_BLUE);

    /* Move or Draw Cursor */
    struct controller_data keys_down = get_keys_down();
    static const int valid_cursor_rows[] = {
        READ_PAK_ADDRESS,
        WRITE_PAK_RANDOM,
        WRITE_PAK_ADDRESS};

    //Handle controller input
    for (int c = 0; c < 4; c++)
    {
        if (keys_down.c[c].data == 0xFFFFFFFF)
            continue;
        //Adjusting cursor position
        if (keys_down.c[c].up && cursor_row > 0)
            cursor_row--;
        if (keys_down.c[c].down && cursor_row < (sizeof(valid_cursor_rows) /sizeof(valid_cursor_rows[0]) - 1))
           cursor_row++;

        if(valid_cursor_rows[cursor_row] == READ_PAK_ADDRESS)
        {
            unsigned short old_address = read_address;
            if(keys_down.c[c].A)      read_address += sizeof(read_packet);
            if(keys_down.c[c].B)      read_address -= sizeof(read_packet);
            if(keys_down.c[c].C_up)   read_address += sizeof(read_packet) * 10;
            if(keys_down.c[c].C_down) read_address -= sizeof(read_packet) * 10;

            //clear the write status if the address has changed
            if(read_address != old_address)
                read_status = 1; 

            if(keys_down.c[c].start)
                for (int i = 0; i < sizeof(read_packet); i+=32)
                    read_status = read_mempak_address(c, read_address + i, read_packet + i);
        }

        //Selected WRITE_PAK_ADDRESS row with A, B, C-up, C-down or START
        if(valid_cursor_rows[cursor_row] == WRITE_PAK_ADDRESS)
        {
            unsigned short old_address = write_address;
            if(keys_down.c[c].A)      write_address += 0x20;
            if(keys_down.c[c].B)      write_address -= 0x20;
            if(keys_down.c[c].C_up)   write_address += 0x200;
            if(keys_down.c[c].C_down) write_address -= 0x200;

            //clear the write status if the address has changed
            if(write_address != old_address)
                write_status = 1; 

            if(keys_down.c[c].start)  write_status = write_mempak_address(c, write_address,
                                                                             write_packet);
        }

        //Selected GEN_RANDOM row with A
        if(valid_cursor_rows[cursor_row] == WRITE_PAK_RANDOM && keys_down.c[c].A)
            for (int i = 0; i < sizeof(write_packet); i++)
                write_packet[i] = rand() % 256;
    }

    /* Print the row coursor */
    graphics_draw_coloured_text(disp, COL(0), ROW(valid_cursor_rows[cursor_row]), ">", COLOUR_GREEN);

    /* Print the read status for a mempak read */
    switch (read_status)
    {
        case  0: graphics_draw_coloured_text(disp, COL(9), ROW(READ_PAK_RESULT),
                                            "READ OK", COLOUR_GREEN); break;
        case -1: graphics_draw_coloured_text(disp, COL(9), ROW(READ_PAK_RESULT),
                                            "READ ERROR: CONT OUT OF RANGE", COLOUR_RED); break;
        case -2: graphics_draw_coloured_text(disp, COL(9), ROW(READ_PAK_RESULT),
                                            "READ ERROR: NO MEMPAK", COLOUR_RED); break;
        case -3: graphics_draw_coloured_text(disp, COL(9), ROW(READ_PAK_RESULT),
                                            "READ ERROR: INVALID DATA", COLOUR_RED); break;
        default: graphics_draw_coloured_text(disp, COL(9), ROW(READ_PAK_RESULT),
                                            "N/A", COLOUR_WHITE);
    }

    switch (write_status)
    {
        case  0: graphics_draw_coloured_text(disp, COL(9), ROW(WRITE_PAK_RESULT),
                                            "WRITE OK", COLOUR_GREEN); break;
        case -1: graphics_draw_coloured_text(disp, COL(9), ROW(WRITE_PAK_RESULT),
                                            "WRITE ERROR: CONT OUT OF RANGE", COLOUR_RED); break;
        case -2: graphics_draw_coloured_text(disp, COL(9), ROW(WRITE_PAK_RESULT),
                                            "WRITE ERROR: NO MEMPAK", COLOUR_RED); break;
        case -3: graphics_draw_coloured_text(disp, COL(9), ROW(WRITE_PAK_RESULT),
                                            "WRITE ERROR: INVALID DATA", COLOUR_RED); break;
        default: graphics_draw_coloured_text(disp, COL(9), ROW(WRITE_PAK_RESULT),
                                            "N/A", COLOUR_WHITE);
    }
}
void draw_page_3(display_context_t disp)
{
    graphics_draw_coloured_text(disp, COL(0), ROW(3), "TPAK TEST", COLOUR_WHITE);

    char text_buff[80];

    for (int c = 0; c < 4; c++)
    {
        struct gameboy_cartridge_header gb_header;
        int tpak_status = tpak_init(c);
        sprintf(text_buff, "CONTROLLER %u", c);
        graphics_draw_coloured_text(disp, COL(0), ROW(5 + 0 + c * 3), text_buff, COLOUR_BLUE);
        switch (tpak_status)
        {
            case -6: graphics_draw_coloured_text(disp, COL(0), ROW(5 + 1 + c * 3),
                                                "TPAK_ERROR_ADDRESS_OVERFLOW", COLOUR_RED); break;
            case -5: graphics_draw_coloured_text(disp, COL(0), ROW(5 + 1 + c * 3),
                                                "NO CART INSTALLED", COLOUR_WHITE); break;
            case -4: graphics_draw_coloured_text(disp, COL(0), ROW(5 + 1 + c * 3),
                                                "TPAK_ERROR_UNKNOWN_BEHAVIOUR", COLOUR_RED); break;
            case -3: graphics_draw_coloured_text(disp, COL(0), ROW(5 + 1 + c * 3),
                                                "NO CONTROLLER CONNECTED", COLOUR_RED); break;
            case -2: graphics_draw_coloured_text(disp, COL(0), ROW(5 + 1 + c * 3),
                                                "NO TPAK CONNECTED", COLOUR_WHITE); break;
            case -1: graphics_draw_coloured_text(disp, COL(0), ROW(5 + 1 + c * 3),
                                                "TPAK_ERROR_INVALID_ARGUMENT", COLOUR_RED); break;
            default: graphics_draw_coloured_text(disp, COL(0), ROW(5 + 1 + c * 3),
                                                "UNKNOWN ERROR", COLOUR_RED); break;
            case  0:
                if (tpak_get_cartridge_header(c, &gb_header) != 0)
                {
                    graphics_draw_coloured_text(disp, COL(0), ROW(5 + 1 + c * 3),
                                                "ERROR READING CART HEADER", COLOUR_RED);
                    break;
                }
                if (tpak_check_header(&gb_header) == 0)
                {
                    graphics_draw_coloured_text(disp, COL(0), ROW(5 + 1 + c * 3),
                                                "GB CART HEADER INVALID", COLOUR_RED);
                    break;
                }
                sprintf(text_buff, "INSTALLED CART: %.15s", gb_header.title);
                graphics_draw_coloured_text(disp, COL(0), ROW(5 + 1 + c * 3), text_buff, COLOUR_GREEN);
                break;
        }
    }
}

void draw_page_4(display_context_t disp)
{

}