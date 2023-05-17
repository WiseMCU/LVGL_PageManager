#include "PageManager.h"

static PageTypeHandle* HomePage = NULL;

// 初始化页面管理器
void InitPageManager(void)
{
    DEBUG("InitPageManager\r\n");
}

// 开始页面管理器
static void drag_event_handler(lv_event_t * event);
void PageManagerStart(void)
{
    DEBUG("PageManagerStart\r\n");
    if(HomePage == NULL)
    {
        DEBUG("PageManager No Set HomePage\r\n");
        return;
    }

    HomePage->Page = lv_obj_create(Home);
    lv_obj_set_style_border_width(HomePage->Page, 0, 0);
    lv_obj_set_style_radius(HomePage->Page, 0, 0);
    lv_obj_set_style_bg_opa(HomePage->Page, LV_OPA_0, 0);
    lv_obj_set_size(HomePage->Page, LV_PCT(100), LV_PCT(100));
    lv_obj_align(HomePage->Page, LV_ALIGN_CENTER, POS_X, POS_Y);
    lv_obj_clear_flag(HomePage->Page, LV_OBJ_FLAG_SCROLLABLE);
    // lv_obj_add_event_cb(HomePage->Page, drag_event_handler, LV_EVENT_PRESSING, HomePage->Page);

    HomePage->InitPage(HomePage->Page);
}

// 添加页面
void AddPage(PageTypeHandle* Page, void (*InitPage)(lv_obj_t* page), void (*DeinitPage)(lv_obj_t* page))
{
    Page->State         = VAILD;
    Page->InitPage      = InitPage;
    Page->DeinitPage    = DeinitPage;
    Page->Up            = Page;
    Page->Down          = Page;
    Page->Left          = Page;
    Page->Right         = Page;
}

// 删除页面
void DelPage(PageTypeHandle* Page)
{
}

// 设置主页面
void SetHomePage(PageTypeHandle* Page)
{
    HomePage = Page;
}

// 设置页面切换
void SetPageMoveMode(PageTypeHandle* SourcePage, enum PageDir Dir, PageTypeHandle* TargetPage, enum MoveMode Mode)
{
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









