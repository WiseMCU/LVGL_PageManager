#ifndef PAGEMANAGER_H
#define PAGEMANAGER_H

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "../lvgl/lvgl.h"
#endif

//信息打印
#define PrintInfo printf

//页面大小
#define WIDTH       280
#define HEIGHT      240

//页面图大小
#define MaxMapX 3
#define MaxMapY 2

//页面切换时间
#define AnimMoveTime 250

//初始化页面管理器
void InitPageManager(void);

//删除页面管理器
void DeinitPageManager(void);

//添加页面
void AddPageMap(lv_coord_t X, lv_coord_t Y, lv_obj_t* page, void (*InitPage)(lv_obj_t* page), void (*DeinitPage)(lv_obj_t* page));

//删除页面
void DelPageMap(lv_coord_t X, lv_coord_t Y);

//设置主页面
void SetMainPage(lv_coord_t X, lv_coord_t Y);

//设置指定行的默认页面
void SetVerDefaultPage(lv_coord_t X, lv_coord_t Y);

//更新当前页面坐标
void UpNowPos(void);

//更新当前页面坐标
void InitPage(lv_coord_t X, lv_coord_t Y);

//设置页面的切换动画
void SetPageUpMoveAnimMode(lv_coord_t X, lv_coord_t Y, lv_coord_t Anim, lv_coord_t Mode, _Bool del);
void SetPageDownMoveAnimMode(lv_coord_t X, lv_coord_t Y, lv_coord_t Anim, lv_coord_t Mode, _Bool del);
void SetPageLeftMoveAnimMode(lv_coord_t X, lv_coord_t Y, lv_coord_t Anim, lv_coord_t Mode, _Bool del);
void SetPageRightMoveAnimMode(lv_coord_t X, lv_coord_t Y, lv_coord_t Anim, lv_coord_t Mode, _Bool del);

//页面坐标信息
typedef struct
{
    lv_coord_t MainX;
    lv_coord_t MainY;
    lv_coord_t NowX;
    lv_coord_t NowY;
    lv_coord_t NextX;
    lv_coord_t NextY;
}PageManagerStruct;

//创建页面图
typedef struct
{
    lv_coord_t Vaild;
    lv_obj_t* Page;
    void (*InitPage)(lv_obj_t* page);
    void (*DeinitPage)(lv_obj_t* page);
    lv_coord_t Up;
    lv_coord_t Down;
    lv_coord_t Left;
    lv_coord_t Right;
}PageStruct;

//页面状态
enum {
    INVAILD = 0,
    VAILD,
    INVAILD_AND_CREATED,
    VAILD_AND_CREATED,
};

//页面动画模式
enum {
    //切换动画方式
    //0立即切换（切换动画默认此项）
    LOAD_ANIM_NONE = 0,
    //1 将新页面移动朝给定方向覆盖当前的页面
    LOAD_ANIM_OVER,
    //2 将旧页面从新页面上面移动朝给定方向退出屏幕
    LOAD_ANIM_LEAVE,
    //3 将新页面和当前的页面一起朝给定方向移动
    LOAD_ANIM_MOVE,

    //左右切换页面方式
    //0 页面不可以在切换方向切换（切换模式默认此项）
    HOR_LOAD_MODE_NONE_PAGE = 0,
    //4 页面切换时只加载切换方向的相邻页面
    HOR_LOAD_MODE_ADJACENT_PAGE = (1 << 2),
    //8 页面切换时加载切换方向的有效的页面直到图的边缘
    HOR_LOAD_MODE_VAILD_PAGE = (1 << 3),
    //12 将切换方向的页面排成环，并切换到环内下一个有效的页面
    HOR_LOAD_MODE_RING_VAILD_PAGE = (1 << 3) | (1 << 2),

    //上下切换页面方式
    //0 页面不可以在切换方向切换（切换模式默认此项）
    VER_LOAD_MODE_NONE_PAGE = 0,
    //4 页面切换到相邻行的默认页面，默认是主页面同列的页面（可以修改默认页面索引号可以切换到任意相邻行页面）
    VER_LOAD_MODE_DEFAULT_PAGE = (1 << 2),
    //8 页面切换时加载切换方向的有效的页面直到图的边缘
    VER_LOAD_MODE_VAILD_PAGE = (1 << 3),
    //12 将切换方向的页面排成环，并切换到环内下一个有效的页面
    VER_LOAD_MODE_RING_VAILD_PAGE = (1 << 3) | (1 << 2),

    //128 页面切换完后删除页面（默认切换完删除）
    LOAD_ANIM_AND_DEL_PAGE = (1 << 7),
};

//宏定义函数
#define MainPage (PagesMap[PageInfo.MainX][PageInfo.MainY]->Page)
#define NowPage (PagesMap[PageInfo.NowX][PageInfo.NowY]->Page)
#define NextPage (PagesMap[PageInfo.NextX][PageInfo.NextY]->Page)

//获取页面对象
#define GetPageMap(_x, _y) (PagesMap[_x][_y])

//页面图
extern PageStruct* PagesMap[MaxMapX][MaxMapY];
//页面管理器信息
extern PageManagerStruct PageInfo;

#define Home  lv_scr_act()
#define Desktop  lv_layer_top()
#define System  lv_layer_sys()

#endif
