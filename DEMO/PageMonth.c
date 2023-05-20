#include "Page.h"
#include "PageManager.h"

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#else
#endif

static void drag_event_handler(lv_event_t * event);

void InitMonthPages(lv_obj_t* page)
{
    // 如果要看背景需要注释掉下面两行
    lv_obj_set_style_bg_color(page, lv_palette_main(LV_PALETTE_RED), 0);
    lv_obj_set_style_bg_opa(page, LV_OPA_100, 0);
    
    lv_obj_t * obj;
    obj = lv_obj_create(page);
    lv_obj_set_size(obj, 100, 50);
    lv_obj_align_to(obj, page, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(obj, drag_event_handler, LV_EVENT_PRESSING, NULL);

    lv_obj_t * label = lv_label_create(obj);
    lv_label_set_text(label, "Drag Month");
    lv_obj_center(label);
}

static void drag_event_handler(lv_event_t * event)
{
    lv_obj_t * obj = lv_event_get_target(event);

    lv_indev_t * indev = lv_indev_get_act();
    if(indev == NULL)  return;

    lv_point_t vect;
    lv_indev_get_vect(indev, &vect);

    lv_coord_t x = lv_obj_get_x(obj) + vect.x;
    lv_coord_t y = lv_obj_get_y(obj) + vect.y;
    lv_obj_set_pos(obj, x, y);
}


void DeinitMonthPages(lv_obj_t* page)
{
}

