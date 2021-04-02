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
static lv_obj_t * image;

static lv_obj_t * canvas;
static void * canvas_buf;

#define USE_3D 1

#if USE_3D
static lv_obj_t * btn_obj;
static lv_obj_t * btn_bg;
static lv_obj_t * btn_auto;
static lv_obj_t * sl_angle_x,* sl_angle_y,* sl_obj_dis,* sl_runtime;

extern int Str_dis_to_obj;      //距离
extern int Str_runtime;         //背景驱动 0-360
extern int Str_rotx,Str_roty;   //旋转角
extern uint8_t Str_obj_p;
extern uint8_t Str_bgmdoe;      //背景模式
extern uint8_t run_flag;

static uint8_t auto_mode = 0;
#endif

#define CANVAS_W (LV_HOR_RES_MAX - 50)
#define CANVAS_H (LV_HOR_RES_MAX - 100)

static lv_task_t * task;
static lv_task_cb_t user_task_cb(lv_task_t * task);

static void paint_close_event_cb1(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_SHORT_CLICKED)
        {
            if(obj == close_btn)
            {
                #if USE_3D
                run_flag = 0;
                #endif
                free(canvas_buf);
                lv_obj_del(main_page);
                lv_task_del(task);
            }
            #if USE_3D
            else if(obj == btn_obj)
            {
                Str_obj_p ++;
                if(Str_obj_p >= 13)
                {
                    Str_obj_p = 0;
                }
            }
            else if(obj == btn_bg)
            {
                Str_bgmdoe ++;
                if(Str_bgmdoe >= 5)
                {
                    Str_bgmdoe = 0;
                }
            }
            else if(obj == btn_auto)
            {
                if(auto_mode == 0)
                {
                    auto_mode = 1;
                }
                else
                {
                    auto_mode = 0;
                }
            }
            #endif
        }
        #if USE_3D
        else if(event == LV_EVENT_VALUE_CHANGED)
        {
            if(obj == sl_angle_x)
            {
                Str_rotx = lv_slider_get_value(obj);
            }
            else if(obj == sl_angle_y)
            {
                Str_roty = lv_slider_get_value(obj);
            }
            else if(obj == sl_obj_dis)
            {
                Str_dis_to_obj = lv_slider_get_value(obj);
            }
        }
        #endif
}

static lv_task_cb_t user_task_cb(lv_task_t * task)
{
    #if USE_3D
    Str_runtime += lv_slider_get_value(sl_runtime);
    Str_runtime = Str_runtime % 360;
    if(auto_mode)
    {
        Str_rotx += 2;Str_rotx = Str_rotx % 360;lv_slider_set_value(sl_angle_x,Str_rotx,LV_ANIM_OFF);
        Str_roty += 3;Str_roty = Str_roty % 360;lv_slider_set_value(sl_angle_y,Str_roty,LV_ANIM_OFF);
        Str_dis_to_obj = 130 +  (300 * _lv_trigo_sin((lv_tick_get() / 15) % 360) >> 15);
        lv_slider_set_value(sl_obj_dis,Str_dis_to_obj,LV_ANIM_OFF);
    }
    #endif
}

void app_paint_create(lv_obj_t * parent)
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
    lv_obj_set_event_cb(close_btn, paint_close_event_cb1);
#if 0
    canvas = lv_canvas_create(main_page,NULL);
    lv_obj_set_size(canvas,CANVAS_W,CANVAS_H);
    lv_obj_align(canvas,main_page,LV_ALIGN_IN_TOP_MID,0,50);
    canvas_buf = malloc(CANVAS_W*CANVAS_H*2);
    lv_canvas_set_buffer(canvas,canvas_buf,CANVAS_W,CANVAS_H,LV_IMG_CF_TRUE_COLOR);
    lv_canvas_fill_bg(canvas,LV_COLOR_WHITE,LV_OPA_10);
#endif

    #if USE_3D
    btn_obj = lv_btn_create(main_page,NULL);
    lv_obj_set_size(btn_obj,100,38);
    btn_bg = lv_btn_create(main_page,btn_obj);
    btn_auto = lv_btn_create(main_page,btn_obj);

    lv_obj_t * labe2 = lv_label_create(btn_obj,NULL);
    lv_label_set_text(labe2,"3D对象");
    lv_obj_set_style_local_text_color(labe2,LV_LABEL_PART_MAIN,LV_STATE_DEFAULT,LV_COLOR_RED);

    lv_obj_t * labe3 = lv_label_create(btn_bg,NULL);
    lv_label_set_text(labe3,"背景模式");
    lv_obj_set_style_local_text_color(labe3,LV_LABEL_PART_MAIN,LV_STATE_DEFAULT,LV_COLOR_RED);

    lv_obj_t * labe4 = lv_label_create(btn_auto,NULL);
    lv_label_set_text(labe4,"自动模式");
    lv_obj_set_style_local_text_color(labe4,LV_LABEL_PART_MAIN,LV_STATE_DEFAULT,LV_COLOR_RED);

    lv_obj_align(btn_obj,main_page,LV_ALIGN_IN_LEFT_MID,20,180);
    lv_obj_align(btn_bg,btn_obj,LV_ALIGN_OUT_RIGHT_MID,20,0);
    lv_obj_align(btn_auto,btn_bg,LV_ALIGN_OUT_RIGHT_MID,20,0);
    lv_obj_set_event_cb(btn_obj,paint_close_event_cb1);
    lv_obj_set_event_cb(btn_bg,paint_close_event_cb1);
    lv_obj_set_event_cb(btn_auto,paint_close_event_cb1);

    sl_angle_x = lv_slider_create(main_page,NULL);
    lv_obj_set_width(sl_angle_x,LV_HOR_RES_MAX - 80);
    sl_angle_y = lv_slider_create(main_page,sl_angle_x);
    sl_obj_dis = lv_slider_create(main_page,sl_angle_x);
    sl_runtime = lv_slider_create(main_page,sl_angle_x);

    lv_slider_set_range(sl_angle_x,0,360);
    lv_slider_set_range(sl_angle_y,0,360);
    lv_slider_set_range(sl_obj_dis,-170,430);
    lv_slider_set_range(sl_runtime,1,10);

    lv_obj_align(sl_angle_x,btn_obj,LV_ALIGN_OUT_BOTTOM_LEFT,20,30);
    lv_obj_align(sl_angle_y,sl_angle_x,LV_ALIGN_OUT_BOTTOM_MID,0,30);
    lv_obj_align(sl_obj_dis,sl_angle_y,LV_ALIGN_OUT_BOTTOM_MID,0,30);
    lv_obj_align(sl_runtime,sl_obj_dis,LV_ALIGN_OUT_BOTTOM_MID,0,30);

    lv_obj_set_event_cb(sl_angle_x,paint_close_event_cb1);
    lv_obj_set_event_cb(sl_angle_y,paint_close_event_cb1);
    lv_obj_set_event_cb(sl_obj_dis,paint_close_event_cb1);
    lv_obj_set_event_cb(sl_runtime,paint_close_event_cb1);

    run_flag = 1;
    #endif

    //image = lv_img_create(main_page,NULL);
    //image = lv_gif_create_from_file(main_page, "S/QQicon/qq/f018.gif");//"S/lv_lib_gif/example.gif");
    //lv_img_set_src(image, small_image_sjpg);
    //lv_obj_align(image, main_page, LV_ALIGN_CENTER, 0, 0);

    task = lv_task_create(user_task_cb, 30, LV_TASK_PRIO_MID, NULL);
}
