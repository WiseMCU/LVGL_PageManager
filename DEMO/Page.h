#ifndef PAGE_H
#define PAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#include "sys.h"
#define Print rt_kprintf
#else
#include "../lvgl/lvgl.h"
#define Print printf
#include <time.h>
#define IMG img
LV_IMG_DECLARE(IMG);
#endif

#include "stdlib.h"
#include "stdio.h"
#include "math.h"

//页面初始化
void PageManagerInit(void);

//页面函数
void InitMainPages(lv_obj_t* page);
void DeinitMainPages(lv_obj_t* page);

void InitMonthPages(lv_obj_t* page);
void DeinitMonthPages(lv_obj_t* page);

void InitPopPages(lv_obj_t* page);
void DeinitPopPages(lv_obj_t* page);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
