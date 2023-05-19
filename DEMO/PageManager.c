#include "PageManager.h"

struct
{
    // 实际运动状态
    uint8_t State;
    // 允许运动方向
    uint8_t Dir;
    // 管理器主体
    lv_obj_t* obj;
    // 管理器背景层
    lv_obj_t* BackGround;
    // 默认管理器样式
    lv_style_t DefaultStyle;
    // 永远指向无触摸时显示的页面
    PageTypeHandle* HomePage;
}PageManager;

static void MovePage(lv_obj_t* SourceObj, enum PageDir Dir, lv_obj_t* TargetObj, lv_coord_t Distance, enum MoveMode Mode)
{
    switch(Mode)
    {
        case LOAD_ANIM_MOVE:{
            lv_obj_set_pos(SourceObj, Distance, 0);
            lv_obj_align_to(TargetObj, SourceObj, LV_ALIGN_CENTER, WIDTH, 0);
        }break;

        case LOAD_ANIM_OVER:{
        }break;

        case LOAD_ANIM_LEAVE:{
        }break;

        case LOAD_ANIM_ROTATE:{
        }break;

        default:{
        }break;
    }
}

static void drag_event_handler(lv_event_t * event)
{
    if(PageManager.Dir == LV_DIR_NONE)
    {
        DEBUG("No Page Can Move\r\n");
        lv_obj_remove_event_cb(PageManager.obj, drag_event_handler);
        lv_obj_remove_event_cb(PageManager.obj, drag_event_handler);
        return;
    }

    static int xBuf = 0, yBuf = 0, act_x = 0, act_y = 0;
    lv_obj_t* obj = lv_event_get_target(event);
    if(obj != PageManager.obj)
    {
        lv_event_code_t code = lv_event_get_code(event);
        switch(code)
        {
            case LV_EVENT_PRESSING:{
                lv_indev_t * indev = lv_indev_get_act();
                if(indev == NULL)  return;

                lv_point_t vect;
                lv_indev_get_vect(indev, &vect);
                if(LV_ABS(vect.x) == LV_ABS(vect.y))
                {
                    return;
                }
                
                xBuf += vect.x;
                yBuf += vect.y;

                if(PageManager.State == LV_DIR_NONE)
                {
                    act_x = 0;
                    act_y = 0;
                    if(LV_ABS(vect.x) > LV_ABS(vect.y))
                    {
                        // 横向优先
                        if(PageManager.Dir & LV_DIR_HOR)
                        {
                            // 允许横向运动
                            PageManager.State |= (PageManager.Dir & LV_DIR_HOR);
                        }else{
                            PageManager.State |= (PageManager.Dir & LV_DIR_VER);
                        }
                    }else{
                        // 纵向优先
                        if(PageManager.Dir & LV_DIR_VER)
                        {
                            // 允许纵向运动
                            PageManager.State |= (PageManager.Dir & LV_DIR_VER);
                        }else{
                            PageManager.State |= (PageManager.Dir & LV_DIR_HOR);
                        }
                    }
                }

                switch(PageManager.State)
                {
                    case LV_DIR_LEFT:{
                        DEBUG("LV_DIR_LEFT\r\n");
                        // 页面在左边，所以允许向右运动
                        if(xBuf < 0)
                        {
                            act_x = xBuf;
                        }
                        MovePage(PageManager.HomePage->obj, LV_DIR_LEFT, 
                            PageManager.HomePage->Left.Page->obj, act_x, PageManager.HomePage->Left.Mode);
                    }break;

                    case LV_DIR_RIGHT:{
                        DEBUG("LV_DIR_RIGHT\r\n");
                        if(xBuf > 0)
                        {
                            act_x = xBuf;
                        }
                    }break;

                    case LV_DIR_TOP:{
                        DEBUG("LV_DIR_TOP\r\n");
                        if(yBuf > 0)
                        {
                            act_y = yBuf;
                        }
                    }break;

                    case LV_DIR_BOTTOM:{
                        DEBUG("LV_DIR_BOTTOM\r\n");
                        if(yBuf < 0)
                        {
                            act_y = yBuf;
                        }
                    }break;

                    case LV_DIR_HOR:{
                        DEBUG("LV_DIR_HOR\r\n");
                        act_x = xBuf;
                    }break;

                    case LV_DIR_VER:{
                        DEBUG("LV_DIR_VER\r\n");
                        act_y = yBuf;
                    }break;

                    default:{
                    }break;
                }
                DEBUG("LV_EVENT_PRESSING [%d] [%d]\r\n", xBuf, yBuf);
            }break;

            case LV_EVENT_RELEASED:{
                // 运动超过半个屏幕认为切换到下一个页面
                if(LV_ABS(act_x) > (WIDTH / 2))
                {
                    if(act_x)
                    {
                        DEBUG("Page Move To LeftPage\r\n");
                    }else{
                        DEBUG("Page Move To RightPage\r\n");
                    }
                }else if (LV_ABS(act_y) > (HEIGHT / 2))
                {
                    if(act_y)
                    {
                        DEBUG("Page Move To UpPage\r\n");
                    }else{
                        DEBUG("Page Move To DownPage\r\n");
                    }
                }
                
                PageManager.State = xBuf = yBuf = 0;
                lv_obj_set_pos(PageManager.HomePage->obj, 0, 0);
                DEBUG("LV_EVENT_RELEASED !!!\r\n\r\n");
            }break;

            default:
            break;
        }
    }
}

// 初始化页面管理器
void PM_Init(void)
{
    DEBUG("InitPageManager\r\n");

    lv_style_init(&PageManager.DefaultStyle);
    lv_style_set_pad_all(&PageManager.DefaultStyle, 0);
    lv_style_set_bg_opa(&PageManager.DefaultStyle, LV_OPA_0);
    lv_style_set_border_width(&PageManager.DefaultStyle, 0);
    lv_style_set_radius(&PageManager.DefaultStyle, 0);
    lv_style_set_align(&PageManager.DefaultStyle, LV_ALIGN_CENTER);

    PageManager.obj = lv_obj_create(Home);
    lv_obj_add_style(PageManager.obj, &PageManager.DefaultStyle, 0);
    lv_obj_set_size(PageManager.obj, WIDTH, HEIGHT);
    lv_obj_clear_flag(PageManager.obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(PageManager.obj, LV_SCROLLBAR_MODE_OFF);

    PageManager.BackGround = lv_obj_create(PageManager.obj);
    lv_obj_add_style(PageManager.BackGround, &PageManager.DefaultStyle, 0);
    lv_obj_set_style_bg_opa(PageManager.BackGround, LV_OPA_100, 0);
    lv_obj_set_size(PageManager.BackGround, LV_PCT(100), LV_PCT(100));
    lv_obj_move_background(PageManager.BackGround);

    PageManager.State = 0;
}

// 开始页面管理器
void PM_Satrt(PageTypeHandle* Page)
{
    PM_SetHomePage(Page);
    DEBUG("PageManager Is Start\r\n");
}

// 添加页面
void PM_AddPage(PageTypeHandle* Page, void (*InitPage)(lv_obj_t* page), void (*DeinitPage)(lv_obj_t* page))
{
    if(Page == NULL)
    {
        DEBUG("Add Page Failed, Page Is Null\r\n");
        return;
    }
    Page->State         = VAILD;
    Page->InitPage      = InitPage;
    Page->DeinitPage    = DeinitPage;
    Page->Up.Page       = NULL;
    Page->Up.Mode       = LOAD_ANIM_NONE;
    Page->Down.Page     = NULL;
    Page->Down.Mode     = LOAD_ANIM_NONE;
    Page->Left.Page     = NULL;
    Page->Left.Mode     = LOAD_ANIM_NONE;
    Page->Right.Page    = NULL;
    Page->Right.Mode    = LOAD_ANIM_NONE;
}

// 关闭页面
void PM_ClosePage(PageTypeHandle* SourcePage, enum PageDir Dir)
{
    PageTypeHandle* Page;
    switch(Dir)
    {
        case PageNone:{
            Page = SourcePage;
        }break;

        case PageUp:{
            Page = SourcePage->Up.Page;
        }break;

        case PageDown:{
            Page = SourcePage->Down.Page;
        }break;

        case PageLeft:{
            Page = SourcePage->Left.Page;
        }break;

        case PageRight:{
            Page = SourcePage->Right.Page;
        }break;
    }
    if(Page != NULL)
    {
        if(Page->State == VAILD_AND_CREATED)
        {
            Page->DeinitPage(Page->obj);
            lv_obj_del(Page->obj);
            Page->State = VAILD;
        }
    }else{
        DEBUG("Cannot Close Non-Existent Page\r\n");
    }
}

// 删除页面
void PM_DelPage(PageTypeHandle* SourcePage, enum PageDir Dir)
{
    PageTypeHandle* Page;
    switch(Dir)
    {
        case PageNone:{
            Page = SourcePage;
        }break;

        case PageUp:{
            Page = SourcePage->Up.Page;
            SourcePage->Up.Mode = LOAD_ANIM_NONE;
        }break;

        case PageDown:{
            Page = SourcePage->Down.Page;
            SourcePage->Down.Mode = LOAD_ANIM_NONE;
        }break;

        case PageLeft:{
            Page = SourcePage->Left.Page;
            SourcePage->Left.Mode = LOAD_ANIM_NONE;
        }break;

        case PageRight:{
            Page = SourcePage->Right.Page;
            SourcePage->Right.Mode = LOAD_ANIM_NONE;
        }break;
    }
    if(Page != NULL)
    {
        if(Page->State == VAILD_AND_CREATED)
        {
            Page->DeinitPage(Page->obj);
            lv_obj_del(Page->obj);
            Page->State = VAILD;
        }
        Page->State         = INVAILD;
        Page->obj           = NULL;
        Page->InitPage      = NULL;
        Page->DeinitPage    = NULL;
        Page->Up.Page       = NULL;
        Page->Up.Mode       = LOAD_ANIM_NONE;
        Page->Down.Page     = NULL;
        Page->Down.Mode     = LOAD_ANIM_NONE;
        Page->Left.Page     = NULL;
        Page->Left.Mode     = LOAD_ANIM_NONE;
        Page->Right.Page    = NULL;
        Page->Right.Mode    = LOAD_ANIM_NONE;
    }else{
        DEBUG("Cannot Delete Non-Existent Page\r\n");
    }
}

// 设置页面切换
void PM_SetPageMoveMode(PageTypeHandle* SourcePage, enum PageDir Dir, PageTypeHandle* TargetPage, enum MoveMode Mode)
{
    if((SourcePage == NULL) | (SourcePage->State == INVAILD))
    {
        DEBUG("SourcePage Is No Add To PageManager\r\n");
        return;
    }

    if((TargetPage == NULL) | (TargetPage->State == INVAILD))
    {
        DEBUG("TargetPage Is No Add To PageManager\r\n");
        return;
    }

    if(Mode == LOAD_ANIM_NONE)
    {
        DEBUG("Switch Is No Anim [%d]\r\n", Mode);
        return;
    }

    switch(Dir)
    {
        case PageNone:{
            DEBUG("Cannot Switch Oneself [%d]\r\n", Dir);
        }break;

        case PageUp:{
            DEBUG("Set PageUp\r\n");
            SourcePage->Up.Page = TargetPage;
            SourcePage->Up.Mode = Mode;
        }break;

        case PageDown:{
            DEBUG("Set PageDown\r\n");
            SourcePage->Down.Page = TargetPage;
            SourcePage->Down.Mode = Mode;
        }break;

        case PageLeft:{
            DEBUG("Set PageLeft\r\n");
            SourcePage->Left.Page = TargetPage;
            SourcePage->Left.Mode = Mode;
        }break;

        case PageRight:{
            DEBUG("Set PageRight\r\n");
            SourcePage->Right.Page = TargetPage;
            SourcePage->Right.Mode = Mode;
        }break;
    }
}

// 设置主页面
void PM_SetHomePage(PageTypeHandle* Page)
{
    PageTypeHandle* PageBuf;
    if(Page == NULL)
    {
        DEBUG("PageManager: HomePage Is Empty\r\n");
        return;
    }else{
        PageBuf = Page;
        PageBuf->obj = lv_obj_create(PageManager.obj);
        lv_obj_add_style(PageBuf->obj, &PageManager.DefaultStyle, 0);
        lv_obj_set_size(PageBuf->obj, LV_PCT(100), LV_PCT(100));
        lv_obj_clear_flag(PageBuf->obj, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_scrollbar_mode(PageBuf->obj, LV_SCROLLBAR_MODE_OFF);
        lv_obj_add_flag(PageBuf->obj, LV_OBJ_FLAG_EVENT_BUBBLE);
        PageBuf->InitPage(PageBuf->obj);
        PageBuf->State = VAILD_AND_CREATED;
        PageManager.Dir = LV_DIR_NONE;
        lv_obj_add_event_cb(PageManager.obj, drag_event_handler, LV_EVENT_PRESSING, NULL);
        lv_obj_add_event_cb(PageManager.obj, drag_event_handler, LV_EVENT_RELEASED, NULL);
    }

    if(PageManager.HomePage != NULL)
    {
        DEBUG("Clear Old HomePage\r\n");
        PM_ClosePage(PageManager.HomePage, PageNone);
        PM_ClosePage(PageManager.HomePage, PageUp);
        PM_ClosePage(PageManager.HomePage, PageDown);
        PM_ClosePage(PageManager.HomePage, PageLeft);
        PM_ClosePage(PageManager.HomePage, PageRight);
    }
    
    if(Page->Up.Mode != LOAD_ANIM_NONE)
    {
        PageBuf = Page->Up.Page;
        switch (PageBuf->State)
        {
            case INVAILD:{
                DEBUG("HomePage's UpPage Is INVAILD\r\n");
            }break;

            case VAILD:{
                DEBUG("HomePage's UpPage Is VAILD\r\n");
                PageBuf->obj = lv_obj_create(PageManager.obj);
                lv_obj_add_style(PageBuf->obj, &PageManager.DefaultStyle, 0);
                lv_obj_set_size(PageBuf->obj, LV_PCT(100), LV_PCT(100));
                lv_obj_clear_flag(PageBuf->obj, LV_OBJ_FLAG_SCROLLABLE);
                lv_obj_set_scrollbar_mode(PageBuf->obj, LV_SCROLLBAR_MODE_OFF);
                lv_obj_add_flag(PageBuf->obj, LV_OBJ_FLAG_EVENT_BUBBLE);
                lv_obj_align_to(PageBuf->obj, Page->obj, LV_ALIGN_OUT_TOP_MID, 0, 0);
                PageBuf->InitPage(PageBuf->obj);
                PageBuf->State = VAILD_AND_CREATED;
                PageManager.Dir |= LV_DIR_TOP;
            }break;

            case VAILD_AND_CREATED:{
                DEBUG("HomePage's UpPage Is Create\r\n");
                lv_obj_align_to(PageBuf->obj, Page->obj, LV_ALIGN_OUT_TOP_MID, 0, 0);
            }break;
        }
    }

    
    if(Page->Down.Mode != LOAD_ANIM_NONE)
    {
        PageBuf = Page->Down.Page;
        switch (PageBuf->State)
        {
            case INVAILD:{
                DEBUG("HomePage's DownPage Is INVAILD\r\n");
            }break;

            case VAILD:{
                DEBUG("HomePage's DownPage Is VAILD\r\n");
                PageBuf->obj = lv_obj_create(PageManager.obj);
                lv_obj_add_style(PageBuf->obj, &PageManager.DefaultStyle, 0);
                lv_obj_set_size(PageBuf->obj, LV_PCT(100), LV_PCT(100));
                lv_obj_clear_flag(PageBuf->obj, LV_OBJ_FLAG_SCROLLABLE);
                lv_obj_set_scrollbar_mode(PageBuf->obj, LV_SCROLLBAR_MODE_OFF);
                lv_obj_add_flag(PageBuf->obj, LV_OBJ_FLAG_EVENT_BUBBLE);
                lv_obj_align_to(PageBuf->obj, Page->obj, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
                PageBuf->InitPage(PageBuf->obj);
                PageBuf->State = VAILD_AND_CREATED;
                PageManager.Dir |= LV_DIR_BOTTOM;
            }break;

            case VAILD_AND_CREATED:{
                DEBUG("HomePage's DownPage Is Create\r\n");
                lv_obj_align_to(PageBuf->obj, Page->obj, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
            }break;
        }
    }

    
    if(Page->Left.Mode != LOAD_ANIM_NONE)
    {
        PageBuf = Page->Left.Page;
        switch (PageBuf->State)
        {
            case INVAILD:{
                DEBUG("HomePage's LeftPage Is INVAILD\r\n");
            }break;

            case VAILD:{
                DEBUG("HomePage's LeftPage Is VAILD\r\n");
                PageBuf->obj = lv_obj_create(PageManager.obj);
                lv_obj_add_style(PageBuf->obj, &PageManager.DefaultStyle, 0);
                lv_obj_set_size(PageBuf->obj, LV_PCT(100), LV_PCT(100));
                lv_obj_clear_flag(PageBuf->obj, LV_OBJ_FLAG_SCROLLABLE);
                lv_obj_set_scrollbar_mode(PageBuf->obj, LV_SCROLLBAR_MODE_OFF);
                lv_obj_add_flag(PageBuf->obj, LV_OBJ_FLAG_EVENT_BUBBLE);
                lv_obj_align_to(PageBuf->obj, Page->obj, LV_ALIGN_OUT_LEFT_MID, 0, 0);
                PageBuf->InitPage(PageBuf->obj);
                PageBuf->State = VAILD_AND_CREATED;
                PageManager.Dir |= LV_DIR_LEFT;
            }break;

            case VAILD_AND_CREATED:{
                DEBUG("HomePage's LeftPage Is Create\r\n");
                lv_obj_align_to(PageBuf->obj, Page->obj, LV_ALIGN_OUT_LEFT_MID, 0, 0);
            }break;
        }
    }

    
    if(Page->Right.Mode != LOAD_ANIM_NONE)
    {
        PageBuf = Page->Right.Page;
        switch (PageBuf->State)
        {
            case INVAILD:{
                DEBUG("HomePage's RightPage Is INVAILD\r\n");
            }break;

            case VAILD:{
                DEBUG("HomePage's RightPage Is VAILD\r\n");
                PageBuf->obj = lv_obj_create(PageManager.obj);
                lv_obj_add_style(PageBuf->obj, &PageManager.DefaultStyle, 0);
                lv_obj_set_size(PageBuf->obj, LV_PCT(100), LV_PCT(100));
                lv_obj_clear_flag(PageBuf->obj, LV_OBJ_FLAG_SCROLLABLE);
                lv_obj_set_scrollbar_mode(PageBuf->obj, LV_SCROLLBAR_MODE_OFF);
                lv_obj_add_flag(PageBuf->obj, LV_OBJ_FLAG_EVENT_BUBBLE);
                lv_obj_align_to(PageBuf->obj, Page->obj, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
                PageBuf->InitPage(PageBuf->obj);
                PageBuf->State = VAILD_AND_CREATED;
                PageManager.Dir |= LV_DIR_RIGHT;
            }break;

            case VAILD_AND_CREATED:{
                DEBUG("HomePage's RightPage Is Create\r\n");
                lv_obj_align_to(PageBuf->obj, Page->obj, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
            }break;
        }
    }

    PageManager.HomePage = Page;
}

// 页面切换
void PM_PageMove(enum PageDir Dir)
{
    PageTypeHandle* Page;
    switch(Dir)
    {
        case PageNone:{
            DEBUG("Cannot Switch Oneself [%d]\r\n", Dir);
        }break;

        case PageUp:{
            Page = PageManager.HomePage->Up.Page;
            if(Page == NULL)
            {
                DEBUG("PageUp Is Empty\r\n");
                return;
            }
            PM_ClosePage(PageManager.HomePage, PageDown);
            PM_ClosePage(PageManager.HomePage, PageLeft);
            PM_ClosePage(PageManager.HomePage, PageRight);
        }break;

        case PageDown:{
            Page = PageManager.HomePage->Down.Page;
            if(Page == NULL)
            {
                DEBUG("PageDown Is Empty\r\n");
                return;
            }
            PM_ClosePage(PageManager.HomePage, PageUp);
            PM_ClosePage(PageManager.HomePage, PageLeft);
            PM_ClosePage(PageManager.HomePage, PageRight);
        }break;

        case PageLeft:{
            Page = PageManager.HomePage->Left.Page;
            if(Page == NULL)
            {
                DEBUG("PageLeft Is Empty\r\n");
                return;
            }
            PM_ClosePage(PageManager.HomePage, PageUp);
            PM_ClosePage(PageManager.HomePage, PageDown);
            PM_ClosePage(PageManager.HomePage, PageRight);
        }break;

        case PageRight:{
            Page = PageManager.HomePage->Right.Page;
            if(Page == NULL)
            {
                DEBUG("PageRight Is Empty\r\n");
                return;
            }
            PM_ClosePage(PageManager.HomePage, PageUp);
            PM_ClosePage(PageManager.HomePage, PageDown);
            PM_ClosePage(PageManager.HomePage, PageLeft);
        }break;
    }
    PageManager.HomePage = NULL;
    PM_SetHomePage(Page);
}

// 获取管理器背景层OBJ指针
lv_obj_t* PM_GetBackGroudObj(void)
{
    if(PageManager.BackGround == NULL)
    {
        return NULL;
    }else{
        return PageManager.BackGround;
    }
}








