/*********************
 *      INCLUDES
 *********************/
#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "lvgl.h"
#else
#include "../lvgl/lvgl.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "lv_gif.h"

static lv_obj_t * main_page;
static lv_obj_t * close_btn;

static lv_obj_t * kb;
static lv_obj_t * ta;

static lv_task_t * task;
static lv_task_cb_t user_task_cb(lv_task_t * task);

static void kb_del(void)
{
    lv_obj_del(kb);
    kb = NULL;
}

static void notebook_close_event_cb(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_SHORT_CLICKED)
    {
        if(obj == close_btn)
        {
            kb_del();
            lv_obj_del(main_page);
            lv_task_del(task);
        }
    }
}

static lv_task_cb_t user_task_cb(lv_task_t * task)
{
}

static void kb_event_cb(lv_obj_t * keyboard, lv_event_t e)
{
    lv_keyboard_def_event_cb(kb, e);
    if(e == LV_EVENT_CANCEL) {
        lv_keyboard_set_textarea(kb, NULL);
        kb_del();
    }
}

static void kb_create(void)
{
    kb = lv_keyboard_create(lv_scr_act(), NULL);
    lv_keyboard_set_cursor_manage(kb, true);
    lv_keyboard_ext_t * ext = lv_obj_get_ext_attr(kb);
    lv_obj_set_event_cb(ext->btnm, kb_event_cb);
    lv_keyboard_set_textarea(kb, ta);
}

static void ta_event_cb(lv_obj_t * ta_local, lv_event_t e)
{
    if(e == LV_EVENT_CLICKED && kb == NULL) {
        kb_create();
    }
}

void app_notebook_create(lv_obj_t * parent)
{
    int i;
    main_page = lv_page_create(parent,NULL);
    lv_obj_set_size(main_page,LV_HOR_RES_MAX,LV_VER_RES_MAX);
    //lv_win_set_drag(main_page,true);
    lv_obj_set_style_local_bg_opa(main_page,LV_PAGE_PART_BG,LV_STATE_DEFAULT,LV_OPA_50);
    lv_obj_set_style_local_radius(main_page,LV_PAGE_PART_BG,LV_STATE_DEFAULT,0);
    lv_obj_set_style_local_border_width(main_page,LV_PAGE_PART_BG,LV_STATE_DEFAULT,0);
    lv_page_set_scrollable_fit(main_page,LV_FIT_PARENT);
    lv_obj_align(main_page,parent,LV_ALIGN_CENTER,0,0);

    close_btn = lv_btn_create(main_page,NULL);
    lv_obj_set_style_local_bg_color(close_btn,LV_PAGE_PART_BG,LV_STATE_DEFAULT,LV_COLOR_WHITE);
    lv_obj_set_style_local_bg_opa(close_btn,LV_PAGE_PART_BG,LV_STATE_DEFAULT,LV_OPA_60);
    lv_obj_set_style_local_radius(close_btn,LV_BTN_PART_MAIN,LV_STATE_DEFAULT,3);
    lv_obj_set_style_local_border_width(close_btn,LV_BTN_PART_MAIN,LV_STATE_DEFAULT,1);
    lv_obj_set_size(close_btn,52,32);
    lv_obj_align(close_btn,main_page,LV_ALIGN_IN_TOP_RIGHT,-15,15);

    lv_obj_t * label = lv_label_create(close_btn,NULL);
    lv_label_set_text(label,"关闭");
    lv_obj_set_style_local_text_color(label,LV_LABEL_PART_MAIN,LV_STATE_DEFAULT,LV_COLOR_RED);
    lv_obj_set_event_cb(close_btn, notebook_close_event_cb);

    ta  = lv_textarea_create(main_page, NULL);
    lv_obj_align(ta, NULL, LV_ALIGN_IN_TOP_MID, 0, LV_DPI / 16);
    lv_obj_set_event_cb(ta, ta_event_cb);
    lv_textarea_set_text(ta, "");
    lv_coord_t max_h = LV_VER_RES / 2 - LV_DPI / 10;
    if(lv_obj_get_height(ta) > max_h)
        lv_obj_set_height(ta, max_h);

    kb_create();

    task = lv_task_create(user_task_cb, 30, LV_TASK_PRIO_MID, NULL);
}
