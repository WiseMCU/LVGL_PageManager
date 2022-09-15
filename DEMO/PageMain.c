#include "Page.h"
#include "PageManager.h"

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
extern char ImgPathStr[50];
#else
static time_t timep;
static struct tm p[2] = { 0 };
#endif

static void TimeAnim_cb(void* obj, int32_t v);
static void TimeTimer_cb(lv_event_t* event);

#define LV_ANIM_TIME_DEFAULT    300
#define TimeLabelNum            6

/* 时间标签1、2，交替使用实现上下滑动效果 */
static lv_obj_t* labelTime_Grp[TimeLabelNum];
static lv_obj_t* labelTime_Grp2[TimeLabelNum];
/*标签对象*/
static lv_obj_t* next_label[TimeLabelNum];
static lv_obj_t* now_label[TimeLabelNum];
static lv_coord_t y_offset[TimeLabelNum];

static lv_timer_t* timer;
static lv_style_t style;
void InitMainPages(lv_obj_t* page) {

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#else
    memset(p, 0, sizeof(struct tm) * 2);
#endif

	lv_obj_add_event_cb(page, TimeTimer_cb, LV_EVENT_VALUE_CHANGED, NULL);
	
    lv_style_init(&style);
    lv_style_set_bg_opa(&style, LV_OPA_0);
    lv_style_set_bg_color(&style, lv_color_black());
    lv_style_set_text_color(&style, lv_color_white());
    lv_style_set_border_width(&style, 0);
    lv_style_set_pad_all(&style, 0);
    lv_style_set_radius(&style, 0);
	lv_style_set_shadow_width(&style, 0);

    /* contTime */
    lv_obj_t* contTime = lv_obj_create(page);
    lv_obj_add_style(contTime, &style, 0);
	lv_obj_set_style_bg_opa(contTime, LV_OPA_0, 0);
	lv_obj_set_style_text_color(contTime, lv_palette_darken(LV_PALETTE_ORANGE, 2), 0);
    lv_obj_set_size(contTime, 280, 64);
    lv_obj_align(contTime, LV_ALIGN_CENTER, 0, 0);
    lv_obj_clear_flag(contTime, LV_OBJ_FLAG_SCROLLABLE);

    /* labelTime */
	const lv_coord_t x_mod[TimeLabelNum] = { -50, -20, 20, 50 , 0, 16 };

    lv_obj_t* label = lv_label_create(contTime);
    lv_obj_add_style(label, &style, 0);
    lv_label_set_text(label, ":");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_48, 0);

    for (int i = 0; i < TimeLabelNum; i++)
    {
        lv_obj_t* label = lv_label_create(contTime);
        lv_obj_add_style(label, &style, 0);
        lv_label_set_text(label, "0");

        if (i < 4) {
            lv_obj_align(label, LV_ALIGN_CENTER, x_mod[i], 0);
            lv_obj_set_style_text_font(label, &lv_font_montserrat_48, 0);
        }
        else {
			#ifdef LV_LVGL_H_INCLUDE_SIMPLE
			lv_obj_align_to(label, labelTime_Grp[3], LV_ALIGN_OUT_RIGHT_BOTTOM, x_mod[i], -30);
			#else
			lv_obj_align_to(label, labelTime_Grp[3], LV_ALIGN_OUT_RIGHT_BOTTOM, x_mod[i], -16);
			#endif
            lv_obj_set_style_text_font(label, &lv_font_montserrat_22, 0);
        }
        labelTime_Grp[i] = label;
    }

    for (int i = 0; i < TimeLabelNum; i++)
    {
        lv_obj_t* label = lv_label_create(contTime);
        lv_obj_add_style(label, &style, 0);
        lv_label_set_text(label, "0");
        lv_obj_align_to(label, labelTime_Grp[i], LV_ALIGN_OUT_BOTTOM_MID, 0, 50);

        if (i < 4) {
            lv_obj_set_style_text_font(label, &lv_font_montserrat_48, 0);
        }
        else {
            lv_obj_set_style_text_font(label, &lv_font_montserrat_22, 0);
        }
        labelTime_Grp2[i] = label;
    }
#ifdef LV_LVGL_H_INCLUDE_SIMPLE
	HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
#else
    timer = lv_timer_create((void*)TimeTimer_cb, 1000, NULL);
#endif
}

static void TimeAnim_cb(void* obj, int32_t v) {
    int i = 0;
    for (i = 0; i < TimeLabelNum; i++) {
        if (obj == now_label[i])
            break;
    }
    lv_obj_set_y(now_label[i], v);
    lv_obj_set_y(next_label[i], v - y_offset[i]);
}

/**
  * @brief  滑动改变时间标签
  * @param  val_now:当前值
  * @param  val_last:上一次的值
  * @param  index:标签索引
  * @retval 无
  */
static void TimeAddAnim(int val_now, int val_last, int index) {
    /*当前值发生改变时*/
    if ((val_now) != (val_last))
    {
        /*判断两个标签的相对位置，确定谁是下一个标签*/
        if (lv_obj_get_y(labelTime_Grp2[index]) > lv_obj_get_y(labelTime_Grp[index]))
        {
            now_label[index] = labelTime_Grp[index];
            next_label[index] = labelTime_Grp2[index];
        }
        else
        {
            now_label[index] = labelTime_Grp2[index];
            next_label[index] = labelTime_Grp[index];
        }
        lv_label_set_text_fmt(now_label[index], "%d", (val_last));
        lv_label_set_text_fmt(next_label[index], "%d", (val_now));
        /*对齐*/
        if (index < 4) {
            lv_obj_align_to(next_label[index], now_label[index], LV_ALIGN_OUT_TOP_MID, 0, -10);
        }
        else {
            lv_obj_align_to(next_label[index], now_label[index], LV_ALIGN_OUT_TOP_MID, 0, -80);
        }
        /*计算需要的Y偏移量*/
        y_offset[index] = lv_obj_get_y(next_label[index]) - lv_obj_get_y(now_label[index]);
        /*滑动动画*/
        static lv_anim_t anim;
        lv_anim_init(&anim);
        lv_anim_set_var(&anim, now_label[index]);
        lv_anim_set_exec_cb(&anim, (lv_anim_exec_xcb_t)TimeAnim_cb);
        lv_anim_set_values(&anim, lv_obj_get_y(now_label[index]), lv_obj_get_y(now_label[index]) + y_offset[index]);
        lv_anim_set_path_cb(&anim, lv_anim_path_ease_out);
        lv_anim_set_time(&anim, LV_ANIM_TIME_DEFAULT);
        lv_anim_start(&anim);
    }
}

static void TimeTimer_cb(lv_event_t* event) {
	static uint8_t RTC_Index = 0;
    if (GetPageMap(1, 1)->Vaild == VAILD_AND_CREATED) {
#if defined(LV_LVGL_H_INCLUDE_SIMPLE)

        HAL_RTC_GetTime(&hrtc, &RTC_Time[RTC_Index], RTC_FORMAT_BIN);
        HAL_RTC_GetDate(&hrtc, &RTC_Date[RTC_Index], RTC_FORMAT_BIN);

        TimeAddAnim(RTC_Time[RTC_Index].Seconds % 10, RTC_Time[!RTC_Index].Seconds % 10, 5);
        TimeAddAnim(RTC_Time[RTC_Index].Seconds / 10 % 10, RTC_Time[!RTC_Index].Seconds / 10 % 10, 4);

        TimeAddAnim(RTC_Time[RTC_Index].Minutes % 10, RTC_Time[!RTC_Index].Minutes % 10, 3);
        TimeAddAnim(RTC_Time[RTC_Index].Minutes / 10 % 10, RTC_Time[!RTC_Index].Minutes / 10 % 10, 2);

        TimeAddAnim(RTC_Time[RTC_Index].Hours % 10, RTC_Time[!RTC_Index].Hours % 10, 1);
        TimeAddAnim(RTC_Time[RTC_Index].Hours / 10 % 10, RTC_Time[!RTC_Index].Hours / 10 % 10, 0);
#else
        time(&timep);

        memcpy(&p[RTC_Index], localtime(&timep), sizeof(struct tm));

        TimeAddAnim(p[RTC_Index].tm_sec % 10, p[!RTC_Index].tm_sec % 10, 5);
        TimeAddAnim(p[RTC_Index].tm_sec / 10 % 10, p[!RTC_Index].tm_sec / 10 % 10, 4);

        TimeAddAnim(p[RTC_Index].tm_min % 10, p[!RTC_Index].tm_min % 10, 3);
        TimeAddAnim(p[RTC_Index].tm_min / 10 % 10, p[!RTC_Index].tm_min / 10 % 10, 2);

        TimeAddAnim(p[RTC_Index].tm_hour % 10, p[!RTC_Index].tm_hour % 10, 1);
        TimeAddAnim(p[RTC_Index].tm_hour / 10 % 10, p[!RTC_Index].tm_hour / 10 % 10, 0);
#endif
        RTC_Index = !RTC_Index;
    }

}

void DeinitMainPages(lv_obj_t* page) {
    lv_style_reset(&style);
    lv_timer_del(timer);
    lv_obj_clean(page);
}



