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
#define DEBUG(format, ...) printf(format, ##__VA_ARGS__)
#else
#define DEBUG(format, ...)
#endif

// 管理页面大小和位置
#define WIDTH       280
#define HEIGHT      240
#define POS_X       0
#define POS_Y       0

// 页面切换时间
// #define AnimMoveTime 250

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
    // 上面的页面
    PageUp = 0,
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
    // 0立即切换（切换动画默认此项）
    LOAD_ANIM_NONE = 0,
    // 1 将新页面移动朝给定方向覆盖当前的页面
    LOAD_ANIM_OVER,
    // 2 将旧页面从新页面上面移动朝给定方向退出屏幕
    LOAD_ANIM_LEAVE,
    // 3 将新页面和当前的页面一起朝给定方向移动
    LOAD_ANIM_MOVE,
};

// 页面结构体
typedef struct PageNode {
    enum PageState State;
    lv_obj_t* Page;
    void (*InitPage)    (lv_obj_t* page);
    void (*DeinitPage)  (lv_obj_t* page);
    struct PageNode* Up;
    struct PageNode* Down;
    struct PageNode* Left;
    struct PageNode* Right;
}PageTypeHandle;

// 初始化页面管理器
void InitPageManager(void);

// 开始页面管理器
void PageManagerStart(void);

// 删除页面管理器
void DeinitPageManager(void);

// 添加页面
void AddPage(PageTypeHandle* Page, void (*InitPage)(lv_obj_t* page), void (*DeinitPage)(lv_obj_t* page));

// 删除页面
void DelPage(PageTypeHandle* Page);

// 设置主页面
void SetHomePage(PageTypeHandle* Page);

// 设置页面切换
void SetPageMoveMode(PageTypeHandle* SourcePage, enum PageDir Dir, PageTypeHandle* TargetPage, enum MoveMode Mode);

#define Home        lv_scr_act()
#define Desktop     lv_layer_top()
#define System      lv_layer_sys()

#endif
