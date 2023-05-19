#ifndef PAGEMANAGER_H
#define PAGEMANAGER_H

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "../lvgl/lvgl.h"
#endif

//信息打印
#define UsePrint
#ifdef UsePrint
#include "stdio.h"
#define DEBUG(format, ...) printf(format, ##__VA_ARGS__)
#else
#define DEBUG(format, ...)
#endif

// 管理页面大小和位置
#define WIDTH       280
#define HEIGHT      240

// 页面切换时间
#define AnimMoveTime 250

// 页面状态
enum PageState
{
    // 无效页面
    INVAILD = 0,
    // 有效页面
    VAILD,
    // 有效并已创建的页面
    VAILD_AND_CREATED,
};

// 页面的位置方向
enum PageDir
{
    // 本页面
    PageNone    = 0,
    // 上面的页面
    PageUp,
    // 下面的页面
    PageDown,
    // 左面的页面
    PageLeft,
    // 右面的页面
    PageRight,
};

// 页面动画模式
enum MoveMode
{
    // 切换动画方式
    // 0 没有切换方式（此模式应该无法切换）
    LOAD_ANIM_NONE = 0,
    // 1 平推模式
    LOAD_ANIM_MOVE,
    // 2 覆盖模式
    LOAD_ANIM_OVER,
    // 3 离开模式
    LOAD_ANIM_LEAVE,
    // 4 旋转模式
    LOAD_ANIM_ROTATE,
};

// 页面结构体
typedef struct PageNode {
    enum PageState State;
    lv_obj_t* obj;
    void (*InitPage)    (lv_obj_t* page);
    void (*DeinitPage)  (lv_obj_t* page);
    struct
    {
        struct PageNode* Page;
        enum MoveMode Mode;
    }Up;
    struct
    {
        struct PageNode* Page;
        enum MoveMode Mode;
    }Down;
    struct
    {
        struct PageNode* Page;
        enum MoveMode Mode;
    }Left;
    struct
    {
        struct PageNode* Page;
        enum MoveMode Mode;
    }Right;
}PageTypeHandle;

// 初始化页面管理器
void PM_Init(void);

// 开始页面管理器
void PM_Satrt(PageTypeHandle* Page);

// 删除页面管理器
void PM_Deinit(void);

// 添加页面
void PM_AddPage(PageTypeHandle* Page, void (*InitPage)(lv_obj_t* page), void (*DeinitPage)(lv_obj_t* page));

// 关闭页面
void PM_ClosePage(PageTypeHandle* SourcePage, enum PageDir Dir);

// 删除页面
void PM_DelPage(PageTypeHandle* SourcePage, enum PageDir Dir);

// 设置主页面
void PM_SetHomePage(PageTypeHandle* Page);

// 设置页面切换
void PM_SetPageMoveMode(PageTypeHandle* SourcePage, enum PageDir Dir, PageTypeHandle* TargetPage, enum MoveMode Mode);

// 页面切换
void PM_PageMove(enum PageDir Dir);

// 获取管理器背景层OBJ指针
lv_obj_t* PM_GetBackGroudObj(void);

#define Home        lv_scr_act()
#define Desktop     lv_layer_top()
#define System      lv_layer_sys()

#endif
