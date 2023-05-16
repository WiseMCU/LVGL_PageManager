#include "Page.h"

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
char ImgPathStr[50] = "S:/img/img(12).bin";
LV_IMG_DECLARE(IMG);
#else

#endif

static lv_obj_t* PopPages;
static lv_obj_t* MainPages;
static lv_obj_t* MonthPages;
static lv_obj_t* Page3;
static lv_obj_t* Page4;
static lv_obj_t* Page5;
static lv_obj_t* Page6;
static lv_obj_t* Page7;

static lv_obj_t* background;

static lv_style_t style;

void PageManagerInit(void) {
    //初始化页面管理器
    InitPageManager();

    lv_obj_clear_flag(Home, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(Desktop, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(Desktop, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(System, LV_OBJ_FLAG_SCROLLABLE);

    //创建无风格
    lv_style_init(&style);
    lv_style_set_bg_color(&style, lv_color_black());
    lv_style_set_text_color(&style, lv_color_white());
    lv_style_set_border_width(&style, 0);
    lv_style_set_pad_all(&style, 0);
    lv_style_set_radius(&style, 0);
    lv_style_set_shadow_width(&style, 0);

    lv_obj_add_style(Home, &style, 0);

    //创建背景
//    background = lv_img_create(Home);
//    lv_obj_add_style(background, &style, 0);
//    lv_img_set_src(background, &IMG);
//    lv_obj_move_background(background);

    //添加页面
    InitMainPages(Home);
}











