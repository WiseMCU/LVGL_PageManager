#ifndef PAGE_H
#define PAGE_H

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "../lvgl/lvgl.h"
#endif
#include "PageManager.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"

#define LCD_WIDTH       WIDTH
#define LCD_HEIGHT      HEIGHT
#define Print printf
#define IMG img
LV_IMG_DECLARE(IMG);

//页面初始化
void PageManagerInit(void);

//页面函数
//void InitMainPages(lv_obj_t* page);
//void DeinitMainPages(lv_obj_t* page);
//
//void InitMonthPages(lv_obj_t* page);
//void DeinitMonthPages(lv_obj_t* page);
//
//void InitPopPages(lv_obj_t* page);
//void DeinitPopPages(lv_obj_t* page);


#endif
