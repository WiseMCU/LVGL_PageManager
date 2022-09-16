#include "PageManager.h"

PageManagerStruct PageInfo = { 0 };
PageStruct* PagesMap[MaxMapX][MaxMapY];
lv_coord_t PageHorIndex[MaxMapY] = { 0 };

static void AnimClose(void) {
    UpNowPos();
}

static void AnimDel(void) {
    (PagesMap[PageInfo.NowX][PageInfo.NowY]->DeinitPage)(PagesMap[PageInfo.NowX][PageInfo.NowY]->Page);
    PagesMap[PageInfo.NowX][PageInfo.NowY]->Vaild -= (1 << 1);
    UpNowPos();
}

static void Anim_close_cb(lv_event_t* event) {
    lv_anim_t* a = (lv_anim_t*)lv_event_get_user_data(event);
    lv_timer_t* timer = (lv_timer_t*)lv_event_get_target(event);
    
    lv_anim_del(a, (void*)lv_obj_set_x);
    lv_anim_del(a, (void*)lv_obj_set_y);
    lv_anim_del(a, (void*)lv_obj_set_style_opa);
    AnimClose();
}

static void Anim_del_cb(lv_event_t* event) {

    lv_anim_t* a = (lv_anim_t*)lv_event_get_user_data(event);
    lv_timer_t* timer = (lv_timer_t*)lv_event_get_target(event);

    lv_anim_del(a, (void*)lv_obj_set_x);
    lv_anim_del(a, (void*)lv_obj_set_y);
    lv_anim_del(a, (void*)lv_obj_set_style_opa);
    AnimDel();
}

static _Bool SearchNextPages(void) {
    switch (PagesMap[PageInfo.NextX][PageInfo.NextY]->Vaild) {
    case INVAILD:
    case INVAILD_AND_CREATED:
        //如果切换方向的相邻页面无效就退出
        return false;

    case VAILD:
        (PagesMap[PageInfo.NextX][PageInfo.NextY]->InitPage)(PagesMap[PageInfo.NextX][PageInfo.NextY]->Page);
        PagesMap[PageInfo.NextX][PageInfo.NextY]->Vaild = VAILD_AND_CREATED;
        break;

    case VAILD_AND_CREATED:
        break;
    }
    return true;
}

static void scr_event_cb(lv_event_t* event)
{
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());

    _Bool Vaild = false;//是否找到下一个有效页面标志位

    lv_anim_t a;
    lv_timer_t* Anim_del;

    lv_coord_t mode;//切换模式
    lv_coord_t anim;//动画方式

    switch (dir) {
    case LV_DIR_LEFT:
        PrintInfo("LEFT\n");

        PageInfo.NextY = PageInfo.NowY;
        //取0，1位
        anim = PagesMap[PageInfo.NowX][PageInfo.NowY]->Left % (1 << 2);
        //取2，3位
        mode = (PagesMap[PageInfo.NowX][PageInfo.NowY]->Left - anim) % (1 << 4);

        //加载切换页面
        switch (mode) {
        case HOR_LOAD_MODE_NONE_PAGE:
            PrintInfo("HOR_LOAD_MODE_NONE_PAGE\n");
            return;

        case HOR_LOAD_MODE_ADJACENT_PAGE:
            PrintInfo("HOR_LOAD_MODE_ADJACENT_PAGE\n");
            if (PageInfo.NowX < (MaxMapX - 1)) {
                PageInfo.NextX = PageInfo.NowX + 1;
            }
            else {
                //如果到了图边缘就退出
                return;
            }

            if (SearchNextPages()) {
                Vaild = true;
            }
            else {
                //相邻的不是有效页面就退出
                return;
            }
            break;

        case HOR_LOAD_MODE_VAILD_PAGE:
            PrintInfo("HOR_LOAD_MODE_VAILD_PAGE\n");
            if (PageInfo.NowX < (MaxMapX - 1)) {
                for (int i = (PageInfo.NowX + 1); i < MaxMapX; i++) {
                    PageInfo.NextX = i;
                    if (SearchNextPages()) {
                        Vaild = true;
                        break;
                    }
                }
            }
            break;

        case HOR_LOAD_MODE_RING_VAILD_PAGE:
            PrintInfo("HOR_LOAD_MODE_RING_VAILD_PAGE\n");
            for (int i = 1; i < MaxMapX; i++) {
                PageInfo.NextX = (PageInfo.NowX + i) % MaxMapX;
                if (SearchNextPages()) {
                    Vaild = true;
                    break;
                }
            }
            break;

        default:
            PrintInfo("LEFT Error mode: %d\n", mode);
            break;
        }

        if (Vaild == false) {
            //没有找到有效页面
            return;
        }

        PrintInfo("X: %d , Y: %d\n", PageInfo.NextX, PageInfo.NextY);
        lv_obj_align_to(NextPage, NowPage, LV_ALIGN_OUT_LEFT_MID, 0, 0);

        //切换动画
        switch (anim) {
        case LOAD_ANIM_NONE:
            PrintInfo("LOAD_ANIM_NONE\n");
            lv_obj_align_to(NextPage, lv_obj_get_parent(NextPage), LV_ALIGN_CENTER, 0, 0);
            //是否删除被切换的页面
            if (PagesMap[PageInfo.NowX][PageInfo.NowY]->Left >= LOAD_ANIM_AND_DEL_PAGE) {
                AnimDel();
            }
            else {
                AnimClose();
            }
            break;

        case LOAD_ANIM_OVER:
            PrintInfo("LOAD_ANIM_OVER\n");
            lv_anim_init(&a);
            lv_anim_set_var(&a, NextPage);
            lv_anim_set_values(&a, WIDTH, 0);
            lv_anim_set_time(&a, AnimMoveTime);
            lv_anim_set_exec_cb(&a, lv_obj_set_x);
            lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);
            lv_anim_start(&a);
            //是否删除被切换的页面
            if (PagesMap[PageInfo.NowX][PageInfo.NowY]->Left >= LOAD_ANIM_AND_DEL_PAGE) {
                Anim_del = lv_timer_create((void*)Anim_del_cb, AnimMoveTime, &a);
            }
            else {
                Anim_del = lv_timer_create((void*)Anim_close_cb, AnimMoveTime, &a);
            }
            lv_timer_set_repeat_count(Anim_del, 1);
            break;

        case LOAD_ANIM_LEAVE:
            PrintInfo("LOAD_ANIM_LEAVE\n");
            lv_obj_align_to(NextPage, lv_obj_get_parent(NextPage), LV_ALIGN_CENTER, 0, 0);
            lv_anim_init(&a);
            lv_anim_set_var(&a, NowPage);
            lv_anim_set_values(&a, 0, -WIDTH);
            lv_anim_set_time(&a, AnimMoveTime);
            lv_anim_set_exec_cb(&a, lv_obj_set_x);
            lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);
            lv_anim_start(&a);
            //是否删除被切换的页面
            if (PagesMap[PageInfo.NowX][PageInfo.NowY]->Left >= LOAD_ANIM_AND_DEL_PAGE) {
                Anim_del = lv_timer_create((void*)Anim_del_cb, AnimMoveTime, &a);
            }
            else {
                Anim_del = lv_timer_create((void*)Anim_close_cb, AnimMoveTime, &a);
            }
            lv_timer_set_repeat_count(Anim_del, 1);
            break;

        case LOAD_ANIM_MOVE:
            PrintInfo("LOAD_ANIM_MOVE\n");
            lv_anim_init(&a);
            lv_anim_set_var(&a, NextPage);
            lv_anim_set_values(&a, WIDTH, 0);
            lv_anim_set_time(&a, AnimMoveTime);
            lv_anim_set_exec_cb(&a, lv_obj_set_x);
            lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);
            lv_anim_start(&a);

            lv_anim_set_var(&a, NowPage);
            lv_anim_set_values(&a, 0, -WIDTH);
            lv_anim_start(&a);
            //是否删除被切换的页面
            if (PagesMap[PageInfo.NowX][PageInfo.NowY]->Left >= LOAD_ANIM_AND_DEL_PAGE) {
                Anim_del = lv_timer_create((void*)Anim_del_cb, AnimMoveTime, &a);
            }
            else {
                Anim_del = lv_timer_create((void*)Anim_close_cb, AnimMoveTime, &a);
            }
            lv_timer_set_repeat_count(Anim_del, 1);
            break;

        default:
            PrintInfo("LEFT Error anim: %d\n", anim);
            break;
        }
        break;

    case LV_DIR_RIGHT:
        PrintInfo("RIGHT\n");

        PageInfo.NextY = PageInfo.NowY;
        //取0，1位
        anim = PagesMap[PageInfo.NowX][PageInfo.NowY]->Right % (1 << 2);
        //取2，3位
        mode = (PagesMap[PageInfo.NowX][PageInfo.NowY]->Right - anim) % (1 << 4);

        //加载切换页面
        switch (mode) {
        case HOR_LOAD_MODE_NONE_PAGE:
            PrintInfo("HOR_LOAD_MODE_NONE_PAGE\n");
            return;

        case HOR_LOAD_MODE_ADJACENT_PAGE:
            PrintInfo("HOR_LOAD_MODE_ADJACENT_PAGE\n");
            if (PageInfo.NowX > 0) {
                PageInfo.NextX = PageInfo.NowX - 1;
            }
            else {
                //到了图边缘就退出
                return;
            }

            if (SearchNextPages()) {
                Vaild = true;
                break;
            }
            else {
                //相邻的不是有效页面就退出
                return;
            }
            break;

        case HOR_LOAD_MODE_VAILD_PAGE:
            PrintInfo("HOR_LOAD_MODE_VAILD_PAGE\n");
            if (PageInfo.NowX > 0) {
                for (int i = (PageInfo.NowX - 1); i >= 0; i--) {
                    PageInfo.NextX = i;
                    if (SearchNextPages()) {
                        Vaild = true;
                        break;
                    }
                }
            }
            break;

        case HOR_LOAD_MODE_RING_VAILD_PAGE:
            PrintInfo("HOR_LOAD_MODE_RING_VAILD_PAGE\n");
            for (int i = 1; i < MaxMapX; i++) {
                PageInfo.NextX = (PageInfo.NowX - i + MaxMapX) % MaxMapX;
                if (SearchNextPages()) {
                    Vaild = true;
                    break;
                }
            }
            break;

        default:
            PrintInfo("RIGHT Error mode: %d\n", mode);
            break;
        }

        if (Vaild == false) {
            //没有找到有效页面
            return;
        }

        PrintInfo("X: %d , Y: %d\n", PageInfo.NextX, PageInfo.NextY);
        lv_obj_align_to(NextPage, NowPage, LV_ALIGN_OUT_RIGHT_MID, 0, 0);

        //切换动画
        switch (anim) {
        case LOAD_ANIM_NONE:
            PrintInfo("LOAD_ANIM_NONE\n");
            lv_obj_align_to(NextPage, lv_obj_get_parent(NextPage), LV_ALIGN_CENTER, 0, 0);
            //是否删除被切换的页面
            if (PagesMap[PageInfo.NowX][PageInfo.NowY]->Right >= LOAD_ANIM_AND_DEL_PAGE) {
                AnimDel();
            }
            else {
                AnimClose();
            }
            break;

        case LOAD_ANIM_OVER:
            PrintInfo("LOAD_ANIM_OVER\n");
            lv_anim_init(&a);
            lv_anim_set_var(&a, NextPage);
            lv_anim_set_values(&a, -WIDTH, 0);
            lv_anim_set_time(&a, AnimMoveTime);
            lv_anim_set_exec_cb(&a, lv_obj_set_x);
            lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);
            lv_anim_start(&a);
            //是否删除被切换的页面
            if (PagesMap[PageInfo.NowX][PageInfo.NowY]->Right >= LOAD_ANIM_AND_DEL_PAGE) {
                Anim_del = lv_timer_create((void*)Anim_del_cb, AnimMoveTime, &a);
            }
            else {
                Anim_del = lv_timer_create((void*)Anim_close_cb, AnimMoveTime, &a);
            }
            lv_timer_set_repeat_count(Anim_del, 1);
            break;

        case LOAD_ANIM_LEAVE:
            PrintInfo("LOAD_ANIM_LEAVE\n");
            lv_obj_align_to(NextPage, lv_obj_get_parent(NextPage), LV_ALIGN_CENTER, 0, 0);
            lv_anim_init(&a);
            lv_anim_set_var(&a, NowPage);
            lv_anim_set_values(&a, 0, WIDTH);
            lv_anim_set_time(&a, AnimMoveTime);
            lv_anim_set_exec_cb(&a, lv_obj_set_x);
            lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);
            lv_anim_start(&a);
            //是否删除被切换的页面
            if (PagesMap[PageInfo.NowX][PageInfo.NowY]->Left >= LOAD_ANIM_AND_DEL_PAGE) {
                Anim_del = lv_timer_create((void*)Anim_del_cb, AnimMoveTime, &a);
            }
            else {
                Anim_del = lv_timer_create((void*)Anim_close_cb, AnimMoveTime, &a);
            }
            lv_timer_set_repeat_count(Anim_del, 1);
            break;

        case LOAD_ANIM_MOVE:
            PrintInfo("LOAD_ANIM_MOVE\n");
            lv_anim_init(&a);
            lv_anim_set_var(&a, NextPage);
            lv_anim_set_values(&a, -WIDTH, 0);
            lv_anim_set_time(&a, AnimMoveTime);
            lv_anim_set_exec_cb(&a, lv_obj_set_x);
            lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);
            lv_anim_start(&a);

            lv_anim_set_var(&a, NowPage);
            lv_anim_set_values(&a, 0, WIDTH);
            lv_anim_start(&a);
            //是否删除被切换的页面
            if (PagesMap[PageInfo.NowX][PageInfo.NowY]->Right >= LOAD_ANIM_AND_DEL_PAGE) {
                Anim_del = lv_timer_create((void*)Anim_del_cb, AnimMoveTime, &a);
            }
            else {
                Anim_del = lv_timer_create((void*)Anim_close_cb, AnimMoveTime, &a);
            }
            lv_timer_set_repeat_count(Anim_del, 1);
            break;

        default:
            PrintInfo("RIGHT Error anim: %d\n", anim);
            break;
        }
        break;

    case LV_DIR_TOP:
        PrintInfo("UP\n");

        PageInfo.NextX = PageInfo.NowX;
        //取0，1位
        anim = PagesMap[PageInfo.NowX][PageInfo.NowY]->Up % (1 << 2);
        //取2，3位
        mode = (PagesMap[PageInfo.NowX][PageInfo.NowY]->Up - anim) % (1 << 4);

        //加载切换页面
        switch (mode) {
        case VER_LOAD_MODE_NONE_PAGE:
            PrintInfo("VER_LOAD_MODE_NONE_PAGE\n");
            return;

        case VER_LOAD_MODE_DEFAULT_PAGE:
            PrintInfo("VER_LOAD_MODE_DEFAULT_PAGE\n");
            
            if (PageInfo.NowY < (MaxMapY - 1)) {
                PageInfo.NextY = PageInfo.NowY + 1;
            }
            else {
                //已经在图边缘了
                return;
            }

            PageInfo.NextX = PageHorIndex[PageInfo.NextY];

            if (SearchNextPages()) {
                Vaild = true;
                break;
            }
            else {
                //相邻的不是有效页面就退出
                return;
            }
            break;

        case VER_LOAD_MODE_VAILD_PAGE:
            PrintInfo("VER_LOAD_MODE_VAILD_PAGE\n");
            if (PageInfo.NowY < (MaxMapY - 1)) {
                for (int i = (PageInfo.NowY + 1); i < MaxMapY; i++) {
                    PageInfo.NextY = i;
                    if (SearchNextPages()) {
                        Vaild = true;
                        break;
                    }
                }
            }
            break;

        case VER_LOAD_MODE_RING_VAILD_PAGE:
            PrintInfo("VER_LOAD_MODE_RING_VAILD_PAGE\n");
            for (int i = 1; i < MaxMapY; i++) {
                PageInfo.NextY = (PageInfo.NowY + i) % MaxMapY;
                if (SearchNextPages()) {
                    Vaild = true;
                    break;
                }
            }
            break;

        default:
            PrintInfo("UP Error mode: %d\n", mode);
            break;
        }

        if (Vaild == false) {
            //没有找到有效页面
            return;
        }

        PrintInfo("X: %d , Y: %d\n", PageInfo.NextX, PageInfo.NextY);
        lv_obj_align_to(NextPage, NowPage, LV_ALIGN_OUT_TOP_MID, 0, 0);

        //切换动画
        switch (anim) {
        case LOAD_ANIM_NONE:
            PrintInfo("LOAD_ANIM_NONE\n");
            lv_obj_align_to(NextPage, lv_obj_get_parent(NextPage), LV_ALIGN_CENTER, 0, 0);
            //是否删除被切换的页面
            if (PagesMap[PageInfo.NowX][PageInfo.NowY]->Up >= LOAD_ANIM_AND_DEL_PAGE) {
                AnimDel();
            }
            else {
                AnimClose();
            }
            break;

        case LOAD_ANIM_OVER:
            PrintInfo("LOAD_ANIM_OVER\n");
            lv_anim_init(&a);
            lv_anim_set_var(&a, NextPage);
            lv_anim_set_values(&a, HEIGHT, 0);
            lv_anim_set_time(&a, AnimMoveTime);
            lv_anim_set_exec_cb(&a, lv_obj_set_y);
            lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);
            lv_anim_start(&a);
            //是否删除被切换的页面
            if (PagesMap[PageInfo.NowX][PageInfo.NowY]->Up >= LOAD_ANIM_AND_DEL_PAGE) {
                Anim_del = lv_timer_create((void*)Anim_del_cb, AnimMoveTime, &a);
            }
            else {
                Anim_del = lv_timer_create((void*)Anim_close_cb, AnimMoveTime, &a);
            }
            lv_timer_set_repeat_count(Anim_del, 1);
            break;

        case LOAD_ANIM_LEAVE:
            PrintInfo("LOAD_ANIM_LEAVE\n");
            lv_obj_align_to(NextPage, lv_obj_get_parent(NextPage), LV_ALIGN_CENTER, 0, 0);
            lv_anim_init(&a);
            lv_anim_set_var(&a, NowPage);
            lv_anim_set_values(&a, 0, -HEIGHT);
            lv_anim_set_time(&a, AnimMoveTime);
            lv_anim_set_exec_cb(&a, lv_obj_set_y);
            lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);
            lv_anim_start(&a);
            //是否删除被切换的页面
            if (PagesMap[PageInfo.NowX][PageInfo.NowY]->Left >= LOAD_ANIM_AND_DEL_PAGE) {
                Anim_del = lv_timer_create((void*)Anim_del_cb, AnimMoveTime, &a);
            }
            else {
                Anim_del = lv_timer_create((void*)Anim_close_cb, AnimMoveTime, &a);
            }
            lv_timer_set_repeat_count(Anim_del, 1);
            break;

        case LOAD_ANIM_MOVE:
            PrintInfo("LOAD_ANIM_MOVE\n");
            lv_anim_init(&a);
            lv_anim_set_var(&a, NextPage);
            lv_anim_set_values(&a, HEIGHT, 0);
            lv_anim_set_time(&a, AnimMoveTime);
            lv_anim_set_exec_cb(&a, lv_obj_set_y);
            lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);
            lv_anim_start(&a);

            lv_anim_set_var(&a, NowPage);
            lv_anim_set_values(&a, 0, -HEIGHT);
            lv_anim_start(&a);
            //是否删除被切换的页面
            if (PagesMap[PageInfo.NowX][PageInfo.NowY]->Up >= LOAD_ANIM_AND_DEL_PAGE) {
                Anim_del = lv_timer_create((void*)Anim_del_cb, AnimMoveTime, &a);
            }
            else {
                Anim_del = lv_timer_create((void*)Anim_close_cb, AnimMoveTime, &a);
            }
            lv_timer_set_repeat_count(Anim_del, 1);
            break;

        default:
            PrintInfo("UP Error anim: %d\n", anim);
            break;
        }
        break;

    case LV_DIR_BOTTOM:
        PrintInfo("Down\n");

        PageInfo.NextX = PageInfo.NowX;
        //取0，1位
        anim = PagesMap[PageInfo.NowX][PageInfo.NowY]->Down % (1 << 2);
        //取2，3位
        mode = (PagesMap[PageInfo.NowX][PageInfo.NowY]->Down - anim) % (1 << 4);
        //PrintInfo("anim: %d ,mode: %d\n", anim, mode);

        //加载切换页面
        switch (mode) {
        case VER_LOAD_MODE_NONE_PAGE:
            PrintInfo("VER_LOAD_MODE_NONE_PAGE\n");
            return;

        case VER_LOAD_MODE_DEFAULT_PAGE:
            PrintInfo("VER_LOAD_MODE_DEFAULT_PAGE\n");

            if (PageInfo.NowY > 0) {
                PageInfo.NextY = PageInfo.NowY - 1;
            }
            else {
                //已经在图边缘了
                return;
            }

            PageInfo.NextX = PageHorIndex[PageInfo.NextY];

            if (SearchNextPages()) {
                Vaild = true;
                break;
            }
            else {
                //相邻的不是有效页面就退出
                return;
            }
            break;

        case VER_LOAD_MODE_VAILD_PAGE:
            PrintInfo("VER_LOAD_MODE_VAILD_PAGE\n");
            if (PageInfo.NowY > 0) {
                for (int i = (PageInfo.NowY - 1); i >= 0; i--) {
                    PageInfo.NextY = i;
                    if (SearchNextPages()) {
                        Vaild = true;
                        break;
                    }
                }
            }
            break;

        case VER_LOAD_MODE_RING_VAILD_PAGE:
            PrintInfo("VER_LOAD_MODE_RING_VAILD_PAGE\n");
            for (int i = 1; i < MaxMapY; i++) {
                PageInfo.NextY = (PageInfo.NowY - i + MaxMapY) % MaxMapY;
                if (SearchNextPages()) {
                    Vaild = true;
                    break;
                }
            }
            break;

        default:
            PrintInfo("DOWN Error mode: %d\n", mode);
            break;
        }

        if (Vaild == false) {
            //没有找到有效页面
            return;
        }

        PrintInfo("X: %d , Y: %d\n", PageInfo.NextX, PageInfo.NextY);
        lv_obj_align_to(NextPage, NowPage, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

        //切换动画
        switch (anim) {
        case LOAD_ANIM_NONE:
            PrintInfo("LOAD_ANIM_NONE\n");
            lv_obj_align_to(NextPage, lv_obj_get_parent(NextPage), LV_ALIGN_CENTER, 0, 0);
            //是否删除被切换的页面
            if (PagesMap[PageInfo.NowX][PageInfo.NowY]->Down >= LOAD_ANIM_AND_DEL_PAGE) {
                AnimDel();
            }
            else {
                AnimClose();
            }
            break;

        case LOAD_ANIM_OVER:
            PrintInfo("LOAD_ANIM_OVER\n");
            lv_anim_init(&a);
            lv_anim_set_var(&a, NextPage);
            lv_anim_set_values(&a, -HEIGHT, 0);
            lv_anim_set_time(&a, AnimMoveTime);
            lv_anim_set_exec_cb(&a, lv_obj_set_y);
            lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);
            lv_anim_start(&a);
            //是否删除被切换的页面
            if (PagesMap[PageInfo.NowX][PageInfo.NowY]->Down >= LOAD_ANIM_AND_DEL_PAGE) {
                Anim_del = lv_timer_create((void*)Anim_del_cb, AnimMoveTime, &a);
            }
            else {
                Anim_del = lv_timer_create((void*)Anim_close_cb, AnimMoveTime, &a);
            }
            lv_timer_set_repeat_count(Anim_del, 1);
            break;

        case LOAD_ANIM_LEAVE:
            PrintInfo("LOAD_ANIM_LEAVE\n");
            lv_obj_align_to(NextPage, lv_obj_get_parent(NextPage), LV_ALIGN_CENTER, 0, 0);
            lv_anim_init(&a);
            lv_anim_set_var(&a, NowPage);
            lv_anim_set_values(&a, 0, HEIGHT);
            lv_anim_set_time(&a, AnimMoveTime);
            lv_anim_set_exec_cb(&a, lv_obj_set_y);
            lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);
            lv_anim_start(&a);
            //是否删除被切换的页面
            if (PagesMap[PageInfo.NowX][PageInfo.NowY]->Left >= LOAD_ANIM_AND_DEL_PAGE) {
                Anim_del = lv_timer_create((void*)Anim_del_cb, AnimMoveTime, &a);
            }
            else {
                Anim_del = lv_timer_create((void*)Anim_close_cb, AnimMoveTime, &a);
            }
            lv_timer_set_repeat_count(Anim_del, 1);
            break;

        case LOAD_ANIM_MOVE:
            PrintInfo("LOAD_ANIM_MOVE\n");
            lv_anim_init(&a);
            lv_anim_set_var(&a, NextPage);
            lv_anim_set_values(&a, -HEIGHT, 0);
            lv_anim_set_time(&a, AnimMoveTime);
            lv_anim_set_exec_cb(&a, lv_obj_set_y);
            lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);
            lv_anim_start(&a);

            lv_anim_set_var(&a, NowPage);
            lv_anim_set_values(&a, 0, HEIGHT);
            lv_anim_start(&a);
            //是否删除被切换的页面
            if (PagesMap[PageInfo.NowX][PageInfo.NowY]->Down >= LOAD_ANIM_AND_DEL_PAGE) {
                Anim_del = lv_timer_create((void*)Anim_del_cb, AnimMoveTime, &a);
            }
            else {
                Anim_del = lv_timer_create((void*)Anim_close_cb, AnimMoveTime, &a);
            }
            lv_timer_set_repeat_count(Anim_del, 1);
            break;

        default:
            PrintInfo("DOWN Error anim: %d\n", anim);
            break;
        }
    }
}

void DelPageMap(lv_coord_t X, lv_coord_t Y)
{
    if ((X >= MaxMapX) | (Y >= MaxMapY)) {
        return;
    }
    else {
        PagesMap[X][Y]->Vaild = INVAILD;
        lv_obj_del(PagesMap[X][Y]->Page);
    }
}

void AddPageMap(lv_coord_t X, lv_coord_t Y, lv_obj_t* page, void (*InitPage)(lv_obj_t* page), void (*DeinitPage)(lv_obj_t* page)) {
    if ((X >= MaxMapX) | (Y >= MaxMapY)) {
        return;
    }
    else {
        page = lv_obj_create(Home);

        lv_obj_remove_style_all(page);
        lv_obj_set_size(page, LV_PCT(100), LV_PCT(100));
        lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_bg_opa(page, LV_OPA_0, 0);
        lv_obj_align(page, LV_ALIGN_OUT_LEFT_MID, 0, 0);

        PagesMap[X][Y]->Vaild = VAILD;
        PagesMap[X][Y]->Page = page;
        PagesMap[X][Y]->InitPage = InitPage;
        PagesMap[X][Y]->DeinitPage = DeinitPage;
    }
}

void UpNowPos(void) {
    PageInfo.NowX = PageInfo.NextX;
    PageInfo.NowY = PageInfo.NextY;
    PageHorIndex[PageInfo.NowY] = PageInfo.NowX;
}

void InitPage(lv_coord_t X, lv_coord_t Y)
{
    if ((X >= MaxMapX) | (Y >= MaxMapY)) {
        return;
    }
    if (PagesMap[X][Y]->Vaild = VAILD) {
        (PagesMap[X][Y]->InitPage)(PagesMap[X][Y]->Page);
        lv_obj_align_to(PagesMap[X][Y]->Page, lv_obj_get_parent(PagesMap[X][Y]->Page), LV_ALIGN_OUT_TOP_MID, 0, 0);
        PagesMap[X][Y]->Vaild = VAILD_AND_CREATED;
    }
}

void SetMainPage(lv_coord_t X, lv_coord_t Y) {
    for (int j = 0; j < MaxMapY; j++) {
        PageHorIndex[j] = X;
    }
    PageInfo.MainX = X;
    PageInfo.MainY = Y;
    PageInfo.NowX = PageInfo.MainX;
    PageInfo.NowY = PageInfo.MainY;
    (PagesMap[PageInfo.MainX][PageInfo.MainY]->InitPage)(PagesMap[PageInfo.MainX][PageInfo.MainY]->Page);
    PagesMap[PageInfo.NowX][PageInfo.NowY]->Vaild = VAILD_AND_CREATED;
}

void InitPageManager(void) {
    //初始化页面图
    for (int i = 0; i < MaxMapX; i++) {
        for (int j = 0; j < MaxMapY; j++) {
            PagesMap[i][j] = lv_mem_alloc(sizeof(PageStruct));
            PagesMap[i][j]->Vaild = INVAILD;
            PagesMap[i][j]->Up = LOAD_ANIM_NONE + HOR_LOAD_MODE_NONE_PAGE + LOAD_ANIM_AND_DEL_PAGE;
            PagesMap[i][j]->Down = LOAD_ANIM_NONE + HOR_LOAD_MODE_NONE_PAGE + LOAD_ANIM_AND_DEL_PAGE;
            PagesMap[i][j]->Left = LOAD_ANIM_NONE + HOR_LOAD_MODE_NONE_PAGE + LOAD_ANIM_AND_DEL_PAGE;
            PagesMap[i][j]->Right = LOAD_ANIM_NONE + HOR_LOAD_MODE_NONE_PAGE + LOAD_ANIM_AND_DEL_PAGE;
        }
    }
    lv_obj_add_event_cb(Home, scr_event_cb, LV_EVENT_GESTURE, NULL);
}

void DeinitPageManager(void) {
    //清除页面并释放空间
    for (int i = 0; i < MaxMapX; i++) {
        for (int j = 0; j < MaxMapY; j++) {
            if (PagesMap[i][j]->Vaild == VAILD_AND_CREATED) {
                (PagesMap[i][j]->DeinitPage)(PagesMap[i][j]->Page);
                PagesMap[i][j]->Vaild == VAILD;
            }
            lv_mem_free(PagesMap[i][j]);
        }
    }
    lv_obj_remove_event_cb(Home, scr_event_cb);
}

//设置向上切换的动画方式、切换方式和是否删除被切换页面
void SetPageUpMoveAnimMode(lv_coord_t X, lv_coord_t Y, lv_coord_t Anim, lv_coord_t Mode, _Bool del) {
    if ((X >= MaxMapX) | (Y >= MaxMapY)) {
        return NULL;
    }
    if (del) {
        PagesMap[X][Y]->Up = Mode + Anim + LOAD_ANIM_AND_DEL_PAGE;
    }
    else {
        PagesMap[X][Y]->Up = Mode + Anim;
    }
    
}

//设置向下切换的动画方式、切换方式和是否删除被切换页面
void SetPageDownMoveAnimMode(lv_coord_t X, lv_coord_t Y, lv_coord_t Anim, lv_coord_t Mode, _Bool del) {
    if ((X >= MaxMapX) | (Y >= MaxMapY)) {
        return NULL;
    }
    if (del) {
        PagesMap[X][Y]->Down = Mode + Anim + LOAD_ANIM_AND_DEL_PAGE;
    }
    else {
        PagesMap[X][Y]->Down = Mode + Anim;
    }
}

//设置向左切换的动画方式、切换方式和是否删除被切换页面
void SetPageLeftMoveAnimMode(lv_coord_t X, lv_coord_t Y, lv_coord_t Anim, lv_coord_t Mode, _Bool del) {
    if ((X >= MaxMapX) | (Y >= MaxMapY)) {
        return NULL;
    }
    if (del) {
        PagesMap[X][Y]->Left = Mode + Anim + LOAD_ANIM_AND_DEL_PAGE;
    }
    else {
        PagesMap[X][Y]->Left = Mode + Anim;
    }
}

//设置向右切换的动画方式、切换方式和是否删除被切换页面
void SetPageRightMoveAnimMode(lv_coord_t X, lv_coord_t Y, lv_coord_t Anim, lv_coord_t Mode, _Bool del) {
    if ((X >= MaxMapX) | (Y >= MaxMapY)) {
        return NULL;
    }
    if (del) {
        PagesMap[X][Y]->Right = Mode + Anim + LOAD_ANIM_AND_DEL_PAGE;
    }
    else {
        PagesMap[X][Y]->Right = Mode + Anim;
    }
}

void SetVerDefaultPage(lv_coord_t X, lv_coord_t Y) {
    if ((X >= MaxMapX) | (Y >= MaxMapY)) {
        return NULL;
    }
    PageHorIndex[Y] = X;
}
