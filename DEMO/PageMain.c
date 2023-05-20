#include "Page.h"
#include "PageManager.h"

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#else
#endif

static void drag_event_handler(lv_event_t * event);
static void event_handler(lv_event_t * event);
void InitMainPages(lv_obj_t* page)
{
    lv_obj_set_style_bg_color(page, lv_palette_main(LV_PALETTE_GREEN), 0);
    lv_obj_set_style_bg_opa(page, LV_OPA_100, 0);

    lv_obj_t * obj;
    obj = lv_obj_create(page);
    lv_obj_set_size(obj, 100, 50);
    lv_obj_align_to(obj, page, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(obj, drag_event_handler, LV_EVENT_PRESSING, NULL);

    lv_obj_t * label = lv_label_create(obj);
    lv_label_set_text(label, "Drag me");
    lv_obj_center(label);

    lv_obj_t * btn1 = lv_btn_create(page);
    lv_obj_add_event_cb(btn1, event_handler, LV_EVENT_ALL, NULL);
    lv_obj_align_to(btn1, obj, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

    label = lv_label_create(btn1);
    lv_label_set_text(label, "Main");
    lv_obj_center(label);
}

static void event_handler(lv_event_t * event)
{
    lv_event_code_t code = lv_event_get_code(event);
    if(code == LV_EVENT_CLICKED) {
        DEBUG("LEFT\r\n");
        PM_PageMove(PageLeft);
    }
}


static void drag_event_handler(lv_event_t * event)
{
    lv_obj_t * obj = lv_event_get_target(event);
    lv_event_code_t code = lv_event_get_code(event);
    switch(code)
    {
        case LV_EVENT_PRESSING:{
            lv_indev_t * indev = lv_indev_get_act();
            if(indev == NULL)  return;

            lv_point_t vect;
            lv_indev_get_vect(indev, &vect);

            lv_coord_t x = lv_obj_get_x(obj) + vect.x;
            lv_coord_t y = lv_obj_get_y(obj) + vect.y;
            lv_obj_set_pos(obj, x, y);
        }break;

        case LV_EVENT_DRAW_POST_BEGIN:{
            DEBUG("LV_EVENT_DRAW_POST_BEGIN\r\n");
        }break;

        case LV_EVENT_RELEASED:{
        }break;

        default:{
        }break;
    }
}


void DeinitMainPages(lv_obj_t* page)
{
}

