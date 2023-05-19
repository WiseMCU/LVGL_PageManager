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
    int pos_x;
    int pos_y;
}PageManager;

static PageTypeHandle* GetDirPage(PageTypeHandle* Page, enum PageDir Dir)
{
    PageTypeHandle* Pagebuf;
    switch(Dir)
    {
        case PageNone:{
            Pagebuf = Page;
        }break;

        case PageUp:{
            Pagebuf = Page->Up.Page;
        }break;

        case PageDown:{
            Pagebuf = Page->Down.Page;
        }break;

        case PageLeft:{
            Pagebuf = Page->Left.Page;
        }break;

        case PageRight:{
            Pagebuf = Page->Right.Page;
        }break;
    }
    if(Pagebuf == NULL)
    {
        DEBUG("GetDirPage Failed [%d]\r\n", Dir);
        return NULL;
    }
    return Pagebuf;
}

static void SetDirPage(PageTypeHandle* SourcePage, enum PageDir Dir, PageTypeHandle* TargetPage)
{
    if(TargetPage == NULL)
    {
        DEBUG("SetDirPage Failed TargetPage Is NULL\r\n");
        return;
    }

    switch(Dir)
    {
        case PageNone:{
            DEBUG("Cannot Set Self DirPage\r\n");
        }break;

        case PageUp:{
            SourcePage->Up.Page = TargetPage;
        }break;

        case PageDown:{
            SourcePage->Down.Page = TargetPage;
        }break;

        case PageLeft:{
            SourcePage->Left.Page = TargetPage;
        }break;

        case PageRight:{
            SourcePage->Right.Page = TargetPage;
        }break;
    }
}

static lv_obj_t* GetDirObj(PageTypeHandle* Page, enum PageDir Dir)
{
    PageTypeHandle* Pagebuf = GetDirPage(Page, Dir);
    if(Pagebuf == NULL)
    {
        DEBUG("GetDirObj Failed DirPage[%d] Is Null\r\n", Dir);
        return NULL;
    }
    return Pagebuf->obj;
}

static uint8_t GetDirState(PageTypeHandle* Page, enum PageDir Dir)
{
    PageTypeHandle* Pagebuf = GetDirPage(Page, Dir);
    if(Pagebuf == NULL)
    {
        DEBUG("GetDirState Failed DirPage[%d] Is Null\r\n", Dir);
        return INVAILD;
    }
    return Pagebuf->State;
}

static uint8_t GetDirMode(PageTypeHandle* Page, enum PageDir Dir)
{
    enum MoveMode Mode;
    switch(Dir)
    {
        case PageNone:{
            Mode = LOAD_ANIM_NONE;
        }break;

        case PageUp:{
            Mode = Page->Up.Mode;
        }break;

        case PageDown:{
            Mode = Page->Down.Mode;
        }break;

        case PageLeft:{
            Mode = Page->Left.Mode;
        }break;

        case PageRight:{
            Mode = Page->Right.Mode;
        }break;

        default:{
        }break;
    }
    return Mode;
}

static void SetDirMode(PageTypeHandle* Page, enum PageDir Dir, enum MoveMode Mode)
{
    switch(Dir)
    {
        case PageNone:{
            DEBUG("Cannot Set Slef DirMode\r\n");
        }break;

        case PageUp:{
            Page->Up.Mode = Mode;
        }break;

        case PageDown:{
            Page->Down.Mode = Mode;
        }break;

        case PageLeft:{
            Page->Left.Mode = Mode;
        }break;

        case PageRight:{
            Page->Right.Mode = Mode;
        }break;

        default:{
        }break;
    }
}

static void PageMoveDistance(PageTypeHandle* Page, enum PageDir Dir, lv_coord_t Distance)
{
    switch(GetDirMode(Page, Dir))
    {
        case LOAD_ANIM_NONE:{
            if(Distance == 0)
            {
                lv_obj_set_pos(Page->obj, 0, 0);
                if(PageManager.State & LV_DIR_HOR)
                {
                    if(PageManager.State & LV_DIR_LEFT)
                    {
                        lv_obj_align_to(Page->Left.Page->obj, Page->obj, LV_ALIGN_CENTER, WIDTH, 0);
                    }
                    if(PageManager.State & LV_DIR_RIGHT)
                    {
                        lv_obj_align_to(Page->Right.Page->obj, Page->obj, LV_ALIGN_CENTER, -WIDTH, 0);
                    }
                }else{
                    if(PageManager.State & LV_DIR_TOP)
                    {
                        lv_obj_align_to(Page->Up.Page->obj, Page->obj, LV_ALIGN_CENTER, 0, -HEIGHT);
                    }
                    if(PageManager.State & LV_DIR_BOTTOM)
                    {
                        lv_obj_align_to(Page->Down.Page->obj, Page->obj, LV_ALIGN_CENTER, 0, HEIGHT);
                    }
                }
            }else{
                DEBUG("PageMoveDistance Failed Not Allow No Anim To Move Page\r\n");
            }
        }break;

        case LOAD_ANIM_MOVE:{
            switch(Dir)
            {
                case PageUp:{
                    lv_obj_set_pos(Page->obj, 0, Distance);
                    lv_obj_align_to(GetDirObj(Page, Dir), Page->obj, LV_ALIGN_CENTER, 0, -HEIGHT);
                }break;

                case PageDown:{
                    lv_obj_set_pos(Page->obj, 0, Distance);
                    lv_obj_align_to(GetDirObj(Page, Dir), Page->obj, LV_ALIGN_CENTER, 0, HEIGHT);
                }break;

                case PageLeft:{
                    lv_obj_set_pos(Page->obj, Distance, 0);
                    lv_obj_align_to(GetDirObj(Page, Dir), Page->obj, LV_ALIGN_CENTER, WIDTH, 0);
                }break;

                case PageRight:{
                    lv_obj_set_pos(Page->obj, Distance, 0);
                    lv_obj_align_to(GetDirObj(Page, Dir), Page->obj, LV_ALIGN_CENTER, -WIDTH, 0);
                }break;

                default:{
                }break;
            }
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

static void dragend_event_handler(lv_event_t * event)
{
    lv_coord_t act_x = lv_obj_get_x(PageManager.HomePage->obj);
    lv_coord_t act_y = lv_obj_get_y(PageManager.HomePage->obj);
    // 运动超过半个屏幕认为切换到下一个页面
    if(LV_ABS(act_x) > (WIDTH / 8 * 3))
    {
        if(act_x < 0)
        {
            DEBUG("Page Move To LeftPage\r\n");
        }else{
            DEBUG("Page Move To RightPage\r\n");
        }
    }else if (LV_ABS(act_y) > (HEIGHT / 8 * 3))
    {
        if(act_y > 0)
        {
            DEBUG("Page Move To UpPage\r\n");
        }else{
            DEBUG("Page Move To DownPage\r\n");
        }
    }
    // 页面复位
    PageMoveDistance(PageManager.HomePage, PageNone, 0);
    DEBUG("LV_EVENT_RELEASED [%d] [%d] !!!\r\n\r\n", act_x, act_y);

    PageManager.State   = LV_DIR_NONE;
    PageManager.pos_x   = 0;
    PageManager.pos_y   = 0;
}

static void draging_event_handler(lv_event_t * event)
{
    if(PageManager.Dir == LV_DIR_NONE)
    {
        DEBUG("No Page Can Move\r\n");
        lv_obj_remove_event_cb(PageManager.obj, draging_event_handler);
        lv_obj_remove_event_cb(PageManager.obj, dragend_event_handler);
        return;
    }

    lv_indev_t * indev = lv_indev_get_act();
    if(indev == NULL)  return;

    lv_point_t vect;
    lv_indev_get_vect(indev, &vect);
    if(LV_ABS(vect.x) == LV_ABS(vect.y))
    {
        return;
    }
    
    PageManager.pos_x += vect.x;
    PageManager.pos_y += vect.y;

    if(PageManager.State == LV_DIR_NONE)
    {
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
    }else{
        DEBUG("Dir[%d] State[%d]\r\n", PageManager.Dir, PageManager.State);
    }

    switch(PageManager.State)
    {
        case LV_DIR_LEFT:{
            DEBUG("LV_DIR_LEFT\r\n");
            // 页面在左边，所以允许向右运动
            if(PageManager.pos_x < 0)
            {
                PageMoveDistance(PageManager.HomePage, PageLeft, PageManager.pos_x);
            }
        }break;

        case LV_DIR_RIGHT:{
            DEBUG("LV_DIR_RIGHT\r\n");
            if(PageManager.pos_x > 0)
            {
                PageMoveDistance(PageManager.HomePage, PageRight, PageManager.pos_x);
            }
        }break;

        case LV_DIR_TOP:{
            DEBUG("LV_DIR_TOP\r\n");
            if(PageManager.pos_y > 0)
            {
                PageMoveDistance(PageManager.HomePage, PageUp, PageManager.pos_y);
            }
        }break;

        case LV_DIR_BOTTOM:{
            DEBUG("LV_DIR_BOTTOM\r\n");
            if(PageManager.pos_y < 0)
            {
                PageMoveDistance(PageManager.HomePage, PageDown, PageManager.pos_y);
            }
        }break;

        case LV_DIR_HOR:{
            DEBUG("LV_DIR_HOR\r\n");
                PageMoveDistance(PageManager.HomePage, PageLeft, PageManager.pos_x);
                PageMoveDistance(PageManager.HomePage, PageRight, PageManager.pos_x);
        }break;

        case LV_DIR_VER:{
            DEBUG("LV_DIR_VER\r\n");
                PageMoveDistance(PageManager.HomePage, PageUp, PageManager.pos_y);
                PageMoveDistance(PageManager.HomePage, PageDown, PageManager.pos_y);
        }break;

        default:{
        }break;
    }
    DEBUG("LV_EVENT_PRESSING [%d] [%d]\r\n", PageManager.pos_x, PageManager.pos_y);
}

// 初始化页面管理器
void PM_Init(void)
{
    DEBUG("InitPageManager\r\n");

    PageManager.State   = LV_DIR_NONE;
    PageManager.Dir     = LV_DIR_NONE;
    PageManager.pos_x   = 0;
    PageManager.pos_y   = 0;

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
    PageTypeHandle* Page = GetDirPage(SourcePage, Dir);
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
    PageTypeHandle* Page = GetDirPage(SourcePage, Dir);
    SetDirMode(SourcePage, Dir, LOAD_ANIM_NONE);

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

    SetDirPage(SourcePage, Dir, TargetPage);
    SetDirMode(SourcePage, Dir, Mode);
}

// 设置主页面
void PM_SetHomePage(PageTypeHandle* Page)
{
    PageTypeHandle* PageBuf;
    if(Page == NULL)
    {
        DEBUG("PM_SetHomePage Failed Page Not Add PageManager\r\n");
        return;
    }

    switch (Page->State)
    {
        case INVAILD:{
            DEBUG("PM_SetHomePage Failed Page Not Add PageManager\r\n");
            return;
        }break;

        case VAILD:{
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
            lv_obj_add_event_cb(PageManager.obj, draging_event_handler, LV_EVENT_PRESSING, NULL);
            lv_obj_add_event_cb(PageManager.obj, dragend_event_handler, LV_EVENT_RELEASED, NULL);
            
            
            if(PageManager.HomePage != NULL)
            {
                // Page的VAILD状态说明与HomePage大概率不相邻
                // HomePage若存在，则清除所有旧页面
                DEBUG("Clear Old HomePage\r\n");
                PM_ClosePage(PageManager.HomePage, PageUp);
                PM_ClosePage(PageManager.HomePage, PageDown);
                PM_ClosePage(PageManager.HomePage, PageLeft);
                PM_ClosePage(PageManager.HomePage, PageRight);
                PM_ClosePage(PageManager.HomePage, PageNone);
            }
        }break;

        case VAILD_AND_CREATED:{
            PageManager.Dir = LV_DIR_NONE;
            if(PageManager.HomePage != NULL)
            {
                // Page的VAILD_AND_CREATED状态说明与HomePage相邻
                // HomePage若存在，则清除除了自己的其他旧页面
                DEBUG("Clear Old HomePage\r\n");
                if(GetDirPage(PageManager.HomePage, PageUp) != Page)
                {
                    PM_ClosePage(PageManager.HomePage, PageUp);
                }
                if(GetDirPage(PageManager.HomePage, PageDown) != Page)
                {
                    PM_ClosePage(PageManager.HomePage, PageDown);
                }
                if(GetDirPage(PageManager.HomePage, PageLeft) != Page)
                {
                    PM_ClosePage(PageManager.HomePage, PageLeft);
                }
                if(GetDirPage(PageManager.HomePage, PageRight) != Page)
                {
                    PM_ClosePage(PageManager.HomePage, PageRight);
                }
                PM_ClosePage(PageManager.HomePage, PageNone);
            }
        }break;

        default:{
        }break;
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
                PageManager.Dir |= LV_DIR_TOP;
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
                PageManager.Dir |= LV_DIR_BOTTOM;
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
                PageManager.Dir |= LV_DIR_LEFT;
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
                PageManager.Dir |= LV_DIR_RIGHT;
                lv_obj_align_to(PageBuf->obj, Page->obj, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
            }break;
        }
    }

    PageManager.HomePage = Page;
}

// 页面切换
void PM_PageMove(enum PageDir Dir)
{
    PM_SetHomePage(GetDirPage(PageManager.HomePage, Dir));
}

// 获取管理器背景层OBJ指针
lv_obj_t* PM_GetBackGroudObj(void)
{
    if(PageManager.BackGround == NULL)
    {
        DEBUG("GetBackGroudObj Failed\r\n");
        return NULL;
    }else{
        return PageManager.BackGround;
    }
}








