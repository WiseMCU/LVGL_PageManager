#include "Page.h"
#include "PageManager.h"

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
extern char ImgPathStr[50];
#else

#endif

static void MonthSetUI_cb(lv_event_t* event);

void InitMonthPages(lv_obj_t* page) {

    lv_obj_t* MonthRoller = lv_roller_create(page);
    lv_obj_remove_style_all(MonthRoller);
    lv_obj_set_style_bg_opa(MonthRoller, LV_OPA_TRANSP, LV_PART_SELECTED);
    lv_obj_set_style_text_font(MonthRoller, &lv_font_montserrat_24, LV_PART_MAIN);
    lv_obj_set_style_text_font(MonthRoller, &lv_font_montserrat_32, LV_PART_SELECTED);
    lv_obj_set_style_align(MonthRoller, LV_ALIGN_CENTER, LV_PART_SELECTED);
    //lv_obj_set_style_text_color(MonthRoller, lv_palette_darken(LV_PALETTE_ORANGE, 2), 0);

    lv_roller_set_options(MonthRoller,
        "January\n"
        "February\n"
        "March\n"
        "April\n"
        "May\n"
        "June\n"
        "July\n"
        "August\n"
        "September\n"
        "October\n"
        "November\n"
        "December",
        LV_ROLLER_MODE_NORMAL);

    lv_obj_center(MonthRoller);
    lv_roller_set_visible_row_count(MonthRoller, 3);
    lv_obj_add_event_cb(MonthRoller, MonthSetUI_cb, LV_EVENT_ALL, NULL);
}

static void MonthSetUI_cb(lv_event_t* event)
{
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t* obj = lv_event_get_target(event);

    static int16_t mask_top_id = -1;
    static int16_t mask_bottom_id = -1;

    if (code == LV_EVENT_COVER_CHECK) {
        lv_event_set_cover_res(event, LV_COVER_RES_MASKED);

    }
    else if (code == LV_EVENT_DRAW_MAIN_BEGIN) {
        /* add mask */
        const lv_font_t* font = lv_obj_get_style_text_font(obj, LV_PART_MAIN);
        lv_coord_t line_space = lv_obj_get_style_text_line_space(obj, LV_PART_MAIN);
        lv_coord_t font_h = lv_font_get_line_height(font);

        lv_area_t roller_coords = { 0 };
        lv_obj_get_coords(obj, &roller_coords);

        lv_area_t rect_area = { 0 };
        rect_area.x1 = roller_coords.x1;
        rect_area.x2 = roller_coords.x2;
        rect_area.y1 = roller_coords.y1;
        rect_area.y2 = roller_coords.y1 + (lv_obj_get_height(obj) - font_h - line_space) / 2;

        lv_draw_mask_fade_param_t* fade_mask_top = lv_mem_buf_get(sizeof(lv_draw_mask_fade_param_t));
        lv_draw_mask_fade_init(fade_mask_top, &rect_area, LV_OPA_TRANSP, rect_area.y1, LV_OPA_COVER, rect_area.y2);
        mask_top_id = lv_draw_mask_add(fade_mask_top, NULL);

        rect_area.y1 = rect_area.y2 + font_h + line_space - 1;
        rect_area.y2 = roller_coords.y2;

        lv_draw_mask_fade_param_t* fade_mask_bottom = lv_mem_buf_get(sizeof(lv_draw_mask_fade_param_t));
        lv_draw_mask_fade_init(fade_mask_bottom, &rect_area, LV_OPA_COVER, rect_area.y1, LV_OPA_TRANSP, rect_area.y2);
        mask_bottom_id = lv_draw_mask_add(fade_mask_bottom, NULL);

    }
    else if (code == LV_EVENT_DRAW_POST_END) {
        lv_draw_mask_fade_param_t* fade_mask_top = lv_draw_mask_remove_id(mask_top_id);
        lv_draw_mask_fade_param_t* fade_mask_bottom = lv_draw_mask_remove_id(mask_bottom_id);
        lv_draw_mask_free_param(fade_mask_top);
        lv_draw_mask_free_param(fade_mask_bottom);
        lv_mem_buf_release(fade_mask_top);
        lv_mem_buf_release(fade_mask_bottom);
        mask_top_id = -1;
        mask_bottom_id = -1;
    }
}

void DeinitMonthPages(lv_obj_t* page) {
    lv_obj_clean(page);
}


