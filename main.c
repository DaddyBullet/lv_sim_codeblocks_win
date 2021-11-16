
/**
 * @file main
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "lvgl/lvgl.h"
#include "lv_drivers/win32drv/win32drv.h"

#include <windows.h>


/*********************
 *      DEFINES
 *********************/

#define SECONDS_IN_HALF_DAY (12 * 60 * 60)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void hal_init(void);
static int tick_thread(void *data);

static void clock_gauge_init();

static void clock_buttons(lv_event_t * ptr_e);

/**********************
 *  STATIC VARIABLES
 **********************/

static lv_obj_t * ptr_clock;

static lv_meter_indicator_t * ptr_hours;
static lv_meter_indicator_t * ptr_minutes;
static lv_meter_indicator_t * ptr_seconds;

static lv_obj_t * ptr_btn_min_inc;
static lv_obj_t * ptr_btn_hour_inc;
static lv_obj_t * ptr_btn_min_dec;
static lv_obj_t * ptr_btn_hour_dec;
static lv_obj_t * ptr_btn_min_reset;

static uint32_t seconds = 0;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int nCmdShow)
{
    uint32_t ticks = 0;
    /*Initialize LittlevGL*/
    lv_init();

    /*Initialize the HAL for LittlevGL*/
    lv_win32_init(hInstance, SW_SHOWNORMAL, 800, 480, NULL);

    /*Run the demo*/
    clock_gauge_init();

    lv_obj_t * label;

    ptr_btn_min_inc = lv_btn_create(lv_scr_act());
    ptr_btn_hour_inc = lv_btn_create(lv_scr_act());
    ptr_btn_min_dec = lv_btn_create(lv_scr_act());
    ptr_btn_hour_dec = lv_btn_create(lv_scr_act());
    ptr_btn_min_reset = lv_btn_create(lv_scr_act());

    label = lv_label_create(ptr_btn_min_inc);
    lv_label_set_text(label, "min ^");
    lv_obj_center(label);

    label = lv_label_create(ptr_btn_hour_inc);
    lv_label_set_text(label, "hour ^");
    lv_obj_center(label);

    label = lv_label_create(ptr_btn_min_dec);
    lv_label_set_text(label, "min v");
    lv_obj_center(label);

    label = lv_label_create(ptr_btn_hour_dec);
    lv_label_set_text(label, "hour v");
    lv_obj_center(label);

    label = lv_label_create(ptr_btn_min_reset);
    lv_label_set_text(label, "min res");
    lv_obj_center(label);

    lv_obj_add_event_cb(ptr_btn_min_inc,   clock_buttons, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(ptr_btn_hour_inc,  clock_buttons, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(ptr_btn_min_dec,   clock_buttons, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(ptr_btn_hour_dec,  clock_buttons, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(ptr_btn_min_reset, clock_buttons, LV_EVENT_CLICKED, NULL);

    lv_coord_t column_dsc[] = {LV_GRID_FR(3), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    lv_coord_t row_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(3), LV_GRID_TEMPLATE_LAST};

    lv_obj_set_grid_dsc_array(lv_scr_act(), column_dsc, row_dsc);

    lv_obj_set_grid_cell(ptr_clock, LV_GRID_ALIGN_CENTER, 0, 1,
                                    LV_GRID_ALIGN_CENTER, 0, 4);
    lv_obj_set_grid_cell(ptr_btn_hour_inc, LV_GRID_ALIGN_CENTER, 1, 1,
                                           LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_grid_cell(ptr_btn_hour_dec, LV_GRID_ALIGN_CENTER, 2, 1,
                                           LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_grid_cell(ptr_btn_min_inc, LV_GRID_ALIGN_CENTER, 1, 1,
                                          LV_GRID_ALIGN_CENTER, 1, 1);
    lv_obj_set_grid_cell(ptr_btn_min_dec, LV_GRID_ALIGN_CENTER, 2, 1,
                                          LV_GRID_ALIGN_CENTER, 1, 1);
    lv_obj_set_grid_cell(ptr_btn_min_reset, LV_GRID_ALIGN_CENTER, 1, 2,
                                            LV_GRID_ALIGN_CENTER, 2, 1);

    while(!lv_win32_quit_signal) {
        /* Periodically call the lv_task handler.
         * It could be done in a timer interrupt or an OS task too.*/
        lv_task_handler();
        usleep(5000);       /*Just to let the system breath*/
        ticks++;
        if (ticks == 100)
        {
            ticks = 0;
            seconds++;
            seconds %= SECONDS_IN_HALF_DAY;

        }
        lv_meter_set_indicator_value(ptr_clock, ptr_seconds, seconds % 60);
        lv_meter_set_indicator_value(ptr_clock, ptr_minutes, (seconds / 60) % 60);
        lv_meter_set_indicator_value(ptr_clock, ptr_hours, (seconds * 5) / 3600);
    }
    return 0;
}


static void clock_gauge_init()
{
    ptr_clock = lv_meter_create(lv_scr_act());

    lv_meter_scale_t * ptr_ticks = lv_meter_add_scale(ptr_clock);
    lv_meter_scale_t * ptr_hour_ticks = lv_meter_add_scale(ptr_clock);

    ptr_hours = lv_meter_add_needle_line(ptr_clock,
                                         ptr_ticks,
                                         10,
                                         lv_palette_main(LV_PALETTE_BLUE),
                                         -50);
    ptr_minutes = lv_meter_add_needle_line(ptr_clock,
                                           ptr_ticks,
                                           5,
                                           lv_palette_main(LV_PALETTE_ORANGE),
                                           -20);
    ptr_seconds = lv_meter_add_needle_line(ptr_clock,
                                           ptr_ticks,
                                           3,
                                           lv_palette_main(LV_PALETTE_PURPLE),
                                           0);
    lv_meter_set_scale_range(ptr_clock, ptr_ticks, 0, 60, 360, -90);
    lv_meter_set_scale_ticks(ptr_clock, ptr_ticks, 61, 3, 20, lv_palette_main(LV_PALETTE_GREY));

    lv_meter_set_scale_range(ptr_clock, ptr_hour_ticks, 1, 12, 330, -90 + 30);
    lv_meter_set_scale_ticks(ptr_clock, ptr_hour_ticks, 12, 5, 10, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_major_ticks(ptr_clock, ptr_hour_ticks, 1, 5, 40, lv_palette_main(LV_PALETTE_GREY), 10);

    lv_meter_set_indicator_value(ptr_clock, ptr_hours, 10);
    lv_meter_set_indicator_value(ptr_clock, ptr_minutes, 20);
    lv_meter_set_indicator_value(ptr_clock, ptr_seconds, 30);

    lv_obj_set_size(ptr_clock, 400, 400);
    //lv_obj_align(ptr_clock, LV_ALIGN_CENTER, 0, 0);
}

static void clock_buttons(lv_event_t * ptr_e)
{
    if (lv_event_get_code(ptr_e) != LV_EVENT_CLICKED)
    {
        return;
    }

    lv_obj_t * ptr_tmp_btn = lv_event_get_target(ptr_e);
    if (ptr_tmp_btn == ptr_btn_min_inc)
    {
        seconds += 60;
        seconds %= SECONDS_IN_HALF_DAY;
    }
    else if (ptr_tmp_btn == ptr_btn_min_dec)
    {
        seconds += SECONDS_IN_HALF_DAY;
        seconds -= 60;
        seconds %= SECONDS_IN_HALF_DAY;
    }
    else if (ptr_tmp_btn == ptr_btn_hour_inc)
    {
        seconds += 3600;
        seconds %= SECONDS_IN_HALF_DAY;
    }
    else if (ptr_tmp_btn == ptr_btn_hour_dec)
    {
        seconds += SECONDS_IN_HALF_DAY;
        seconds -= 3600;
        seconds %= SECONDS_IN_HALF_DAY;
    }
    else
    {
        seconds /= 60;
        seconds *= 60;
    }
}
