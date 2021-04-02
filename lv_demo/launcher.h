#ifndef __LAUNCHER_H
#define __LAUNCHER_H

/*********************
 *      INCLUDES
 *********************/
#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "lvgl.h"
#else
#include "../lvgl/lvgl.h"
#endif

#define USE_ICON_ANIMATION  1
#define SHADOW_LABEL_EN     0
#define MAX_LANGUAGE_NUM    4
#define MAX_ICON_NUM        27
#define LED_ICON_PAGE_DIS   (LV_HOR_RES_MAX/10)
#define LAUNCHER_USE_TABVIEW 0
#define PAGE_ROW_ICON_NUM    4
#define LAUNCHER_PAGE_NUM   3

enum
{
    L_ENGLISH = 0,
    L_CHINESE = 1,
    L_CHINESE_F = 2,
}language;

typedef struct
{
    uint8_t     flag;
    uint8_t     page_num;                   /* 页面数 */
    uint8_t     icon_sta_num;               /* 静态图标数 */
    uint8_t     icon_mov_num;               /* 动态图标数 */
    uint8_t     icon_language;
    bool        virtual_btn;                /* 虚拟按键 */
    bool        rotate_icon;                /* 旋转图标 */
    bool        rasize_icon;
    lv_obj_t    * icon_img[MAX_ICON_NUM];
    lv_obj_t    * icon_name[MAX_ICON_NUM];
    lv_point_t  pos_init[MAX_ICON_NUM];
    lv_point_t  pos_anim[MAX_ICON_NUM];
    int16_t     icon_angle[MAX_ICON_NUM];

    lv_obj_t    * parent_bg;
    lv_obj_t    * parent_cont;
    lv_obj_t    * tileview;
    lv_obj_t    * tabview;
    lv_obj_t    * page[LAUNCHER_PAGE_NUM];
    bool        ppos_led_en;
    lv_obj_t    * ppos_led[LAUNCHER_PAGE_NUM];
}launcher_obj_t;

#define PAGE_MOVE_STEPS LV_HOR_RES_MAX
#define TASK_BAR_STEPS  LV_VER_RES_MAX
#define TASK_HEIGH      32 //(TASK_BAR_STEPS / 25)

typedef struct
{
    lv_obj_t * menu_page;
    lv_obj_t * light_bar;
    lv_obj_t * light_auto;
    lv_obj_t * volume_bar;
    lv_obj_t * volume_off;
    lv_obj_t * time_label;
    lv_obj_t * mess_label;
    lv_obj_t * time_img[6];
    lv_obj_t * msg_list;
}top_menu_obj_t;

extern launcher_obj_t launcher_obj_main;
extern top_menu_obj_t top_menu_obj_main;

#define PIPA_MOVE_STEP 1
#define PIPA_OBJ_MAX_NUM 20
#define PIPA_OBJ_MAX_SPD 100
#define PIPA_OBJ_MEMLESS 1

typedef struct
{
    int cur_x;
    int cur_y;
#if PIPA_OBJ_MEMLESS
    int old_x;
    int old_y;
#endif
    int width;
    int height;
    int weight;
    int speed_x;
    int speed_y;
    int speed_z;
    int accel_x;
    int accel_y;
    int accel_z;
    bool lock;
    int cont_x;
    int cont_y;
    int cont_z;
}_pipa_obj;

typedef struct
{
    int obj_num;
    int lim_x_min;
    int lim_x_max;
    int lim_y_min;
    int lim_y_max;
    int loss;
    _pipa_obj *obj;
}_pipa_group;

#define PIPA_OBJ_N 50
extern _pipa_obj pipa_obj_main[PIPA_OBJ_N];
extern _pipa_group pipa_group_main;

#define USE_3D_OBJ_DEMO 0

void launcher_obj_init(launcher_obj_t * obj,lv_obj_t * parent);
void top_menu_obj_init(top_menu_obj_t * obj,lv_obj_t * parent);
void launcher_create(void);

void pipa_group_init(_pipa_group *obj);
void pipa_group_loop(_pipa_group *obj);

void app_calendar_create(lv_obj_t * parent);
void app_foc_create(lv_obj_t * parent);
void app_paint_create(lv_obj_t * parent);
void app_notebook_create(lv_obj_t * parent);

#endif
