#include "Page.h"

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#else
#endif

void CreateBackgroud(enum BackGroundMode Mode, lv_obj_t* background);

// 页面
static PageTypeHandle PopPages;
static PageTypeHandle MainPages;
static PageTypeHandle MonthPages;
static PageTypeHandle Page3;
static PageTypeHandle Page4;
static PageTypeHandle Page5;
static PageTypeHandle Page6;
static PageTypeHandle Page7;

void PageManagerInit(void)
{
    lv_obj_remove_style_all(lv_scr_act());

    //初始化页面管理器
    PM_Init();

    //创建背景
    CreateBackgroud(Mode2, PM_GetBackGroudObj());

    //添加页面
    PM_AddPage(&MainPages, InitMainPages, DeinitMainPages);
    PM_AddPage(&MonthPages, InitMonthPages, DeinitMonthPages);
    PM_AddPage(&PopPages, InitPopPages, DeinitPopPages);

    PM_SetPageMoveMode(&MainPages, PageLeft, &MonthPages, LOAD_ANIM_MOVE);
    PM_SetPageMoveMode(&MainPages, PageRight, &PopPages, LOAD_ANIM_MOVE);
    PM_SetPageMoveMode(&MainPages, PageUp, &PopPages, LOAD_ANIM_MOVE);
    PM_SetPageMoveMode(&MainPages, PageDown, &MonthPages, LOAD_ANIM_MOVE);

    // PM_SetPageMoveMode(&MonthPages, PageLeft, &MonthPages, LOAD_ANIM_MOVE);
    // PM_SetPageMoveMode(&MonthPages, PageRight, &MonthPages, LOAD_ANIM_MOVE);

    // PM_SetPageMoveMode(&PopPages, PageDown, &MonthPages, LOAD_ANIM_MOVE);
    
    // 页面管理器开始
    PM_Satrt(&MainPages);
}

static void draw_event_cb(lv_event_t * event);
static void add_data(lv_timer_t * timer);
void CreateBackgroud(enum BackGroundMode Mode, lv_obj_t* background)
{
    switch(Mode)
    {
        case NONE:{
            lv_obj_set_style_bg_color(background, lv_color_black(), 0);
        }break;

        case Mode1:{
            lv_img_t* BgImg = lv_img_create(background);
            lv_obj_set_style_radius(BgImg, 0, 0);
            lv_obj_set_style_pad_all(BgImg, 0, 0);
            lv_obj_set_style_border_width(BgImg, 0, 0);
            lv_img_set_src(BgImg, &IMG);
            lv_obj_move_background(BgImg);
        }break;

        case Mode2:{
            // 显示点的数量
            #define PointNum    80
            lv_obj_t * chart = lv_chart_create(background);
            lv_obj_set_style_radius(chart, 0, 0);
            lv_obj_set_style_pad_all(chart, 0, 0);
            lv_obj_set_style_border_width(chart, 0, 0);
            lv_obj_set_size(chart, LV_PCT(100), LV_PCT(100));
            lv_obj_align(chart, LV_ALIGN_CENTER, 0, 0);
            lv_obj_set_style_bg_color(chart, lv_color_black(), 0);
            lv_obj_add_event_cb(chart, draw_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL);

            lv_obj_set_style_line_width(chart, 0, LV_PART_ITEMS);
            lv_chart_set_type(chart, LV_CHART_TYPE_SCATTER);

            lv_chart_set_div_line_count(chart, 0, 0);
            lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_X, 0, (WIDTH * 10));
            lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, (HEIGHT* 10));

            lv_chart_set_point_count(chart, PointNum);

            lv_chart_series_t * ser = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
            for(uint32_t i = 0; i < PointNum; i++) {
                lv_chart_set_next_value2(chart, lv_chart_get_series_next(chart, NULL), lv_rand(0,(WIDTH * 10)), lv_rand(0,(HEIGHT* 10)));
            }
            lv_timer_create(add_data, 50, chart);
        }break;
    }
}

static void draw_event_cb(lv_event_t * event)
{
    lv_obj_draw_part_dsc_t * dsc = lv_event_get_draw_part_dsc(event);
    if(dsc->part == LV_PART_ITEMS) {
        lv_obj_t * obj = lv_event_get_target(event);
        lv_chart_series_t * ser = lv_chart_get_series_next(obj, NULL);
        uint32_t cnt = lv_chart_get_point_count(obj);
        /*Make older value more transparent*/
        dsc->rect_dsc->bg_opa = (LV_OPA_COVER *  dsc->id) / (cnt - 1);

        /*Make smaller values blue, higher values red*/
        lv_coord_t * x_array = lv_chart_get_x_array(obj, ser);
        lv_coord_t * y_array = lv_chart_get_y_array(obj, ser);
        /*dsc->id is the tells drawing order, but we need the ID of the point being drawn.*/
        uint32_t start_point = lv_chart_get_x_start_point(obj, ser);
        uint32_t p_act = (start_point + dsc->id) % cnt; /*Consider start point to get the index of the array*/
        lv_opa_t x_opa = (x_array[p_act] * LV_OPA_100) / (WIDTH * 10);
        lv_opa_t y_opa = (y_array[p_act] * LV_OPA_100) / (HEIGHT* 10);

        dsc->rect_dsc->bg_color = lv_color_mix(lv_palette_main(LV_PALETTE_RED),
                                                lv_palette_main(LV_PALETTE_BLUE),
                                                x_opa + y_opa);
    }
}

static void add_data(lv_timer_t * timer)
{
    LV_UNUSED(timer);
    lv_obj_t * obj = timer->user_data;
    lv_chart_set_next_value2(obj, lv_chart_get_series_next(obj, NULL), lv_rand(0, (WIDTH * 10)), lv_rand(0, (HEIGHT* 10)));
}







