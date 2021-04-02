/*********************
 *      INCLUDES
 *********************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "launcher.h"

#include "./lvgl/src/lv_3d/lv_3d.h"

static lv_obj_t * img_vir_btn[3];

LV_IMG_DECLARE(screenbg)
LV_IMG_DECLARE(girl)
LV_IMG_DECLARE(night)
LV_IMG_DECLARE(night1)

LV_IMG_DECLARE(icon_011)
LV_IMG_DECLARE(icon_014)
LV_IMG_DECLARE(icon_017)
LV_IMG_DECLARE(icon_021)
LV_IMG_DECLARE(icon_024)
LV_IMG_DECLARE(icon_027)
LV_IMG_DECLARE(icon_031)
LV_IMG_DECLARE(icon_034)
LV_IMG_DECLARE(icon_037)
LV_IMG_DECLARE(icon_041)
LV_IMG_DECLARE(icon_044)
LV_IMG_DECLARE(icon_047)
LV_IMG_DECLARE(icon_051)
LV_IMG_DECLARE(icon_054)
LV_IMG_DECLARE(icon_057)
LV_IMG_DECLARE(icon_061)
LV_IMG_DECLARE(icon_064)
LV_IMG_DECLARE(icon_067)
LV_IMG_DECLARE(icon_071)
LV_IMG_DECLARE(icon_074)
LV_IMG_DECLARE(icon_077)
LV_IMG_DECLARE(icon_081)
LV_IMG_DECLARE(icon_084)
LV_IMG_DECLARE(icon_087)
LV_IMG_DECLARE(icon_091)
LV_IMG_DECLARE(icon_094)
LV_IMG_DECLARE(icon_097)

static const lv_img_dsc_t * img_array_l[MAX_ICON_NUM] =
{
     &icon_011,     &icon_014,     &icon_017,
     &icon_021,     &icon_024,     &icon_027,
     &icon_031,     &icon_034,     &icon_037,
     &icon_041,     &icon_044,     &icon_047,
     &icon_051,     &icon_054,     &icon_057,
     &icon_061,     &icon_064,     &icon_067,
     &icon_071,     &icon_074,     &icon_077,
     &icon_081,     &icon_084,     &icon_087,
     &icon_091,     &icon_094,     &icon_097,
};

static int move_icon_id[3][8] =
{
    {21,8 ,16,15,6 ,17,19,7 },
    {9 ,10,11,12,13,14,1 ,3 },
    {20,0 ,22,23,24,25,4 ,5 }
};
static int stat_icon_id[] = {26,2,18};
//乐书事二仪传信傳像儀关冊册制南发器场場姆子市序应开录應戶户指据控據数數文无时時書本机档樂機檔汤浏湯灯無燈猫用电發相短码碼程算系統維線編线统维编置者螺視覽视览計訊記設話计讯记设话貓輯輸辑输通針錄鐘针钟開關陀電音頻频騮又双叒叕收到一条未读，点击查看
//无线通讯录话陀螺仪浏览器文档编辑时钟指南针电子相册应用市场户程序二维码汤姆猫开关控制音机计算乐系统设置记事本发者书灯视频像数据传输短信無線訊錄話儀騮覽檔編輯時鐘針電冊應場戶維碼湯貓開關機計樂統設記發書燈視頻數據傳輸英中简体繁韩又双叒叕收到一条未读，点击查看
static const char * icon_name_tal[MAX_LANGUAGE_NUM][MAX_ICON_NUM]=
{
    {
        "Wireless",
        "Address book",
        "Phone",
        "Gyroscopes",
        "Browser",
        "Editing",
        "Clock",
        "Compass",
        "Photo",
        "Market",
        "Program",
        "QR code",
        "Tom cat",
        "Switch",
        "Recorder",
        "Calculator",
        "Music",
        "Setting",
        "Camera",
        "Notepad",
        "Developer",
        "E-book",
        "LED light",
        "Video",
        "Video recorder",
        "Data trans",
        "SMS",
    },
    {
        "无线",
        "通讯录",
        "通话",
        "陀螺仪",
        "浏览器",
        "文档编辑",
        "时钟",
        "指南针",
        "电子相册",
        "应用市场",
        "用户程序",
        "二维码",
        "汤姆猫",
        "开关控制",
        "录音机",
        "计算器",
        "音乐",
        "系统设置",
        "相机",
        "记事本",
        "开发者",
        "电子书",
        "LED灯",
        "视频",
        "录像机",
        "数据传输",
        "短信",
    },
    {
        "無線",
        "通訊錄",
        "通話",
        "陀螺儀",
        "騮覽器",
        "文檔編輯",
        "時鐘",
        "指南針",
        "電子相冊",
        "應用市場",
        "用戶程序",
        "二維碼",
        "湯姆貓",
        "開關控制",
        "錄音機",
        "計算器",
        "音樂",
        "系統設置",
        "相機",
        "記事本",
        "開發者",
        "電子書",
        "LED燈",
        "視頻",
        "錄像機",
        "數據傳輸",
        "短信",
    },
    {
    }
};

LV_FONT_DECLARE(IconFont12)

static const char * lang_tab =
{
    "英文\n"
    "中文简体\n"
    "中文繁体\n"
    "韩文"
};

LV_IMG_DECLARE(key_back)
LV_IMG_DECLARE(key_home)
LV_IMG_DECLARE(key_tool)

LV_FONT_DECLARE(IconFont12)

static const lv_img_dsc_t * img_array_vir_key[3] =
{
    &key_back,
    &key_home,
    &key_tool
};

static void color_chg_event_cb(lv_obj_t * sw, lv_event_t e)
{
    if(LV_THEME_DEFAULT_INIT != lv_theme_material_init) return;
    if(e == LV_EVENT_VALUE_CHANGED) {
        uint32_t flag = LV_THEME_MATERIAL_FLAG_LIGHT;
        if(lv_switch_get_state(sw)) flag = LV_THEME_MATERIAL_FLAG_DARK;

        LV_THEME_DEFAULT_INIT(lv_theme_get_color_primary(), lv_theme_get_color_primary(),
                flag,
                lv_theme_get_font_small(), lv_theme_get_font_normal(), lv_theme_get_font_subtitle(), lv_theme_get_font_title());
    }
}

static bool launcher_set_flag = false;
#define WIN_OBJ_PAGE 1
#define WIN_OBJ_CLOSE_BTN 2
#define WIN_OBJ_SWT 3
#define WIN_OBJ_DROP 4
#define WIN_OBJ_BTN 5
static lv_obj_t * win_obj[9];

void launcher_set_win_close_event_cb1(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_RELEASED)
    {
        lv_obj_del(win_obj[WIN_OBJ_PAGE]);
        launcher_set_flag = false;
    }
}
void launcher_set_obj_change_event_cb1(lv_obj_t * obj, lv_event_t event)
{
    int i;
    if(obj == win_obj[WIN_OBJ_SWT])
    {
        if(event == LV_EVENT_VALUE_CHANGED)
        {
            launcher_obj_main.virtual_btn = lv_switch_get_state(obj);
        }
    }
    else if(obj == win_obj[WIN_OBJ_DROP])
    {
        if(event == LV_EVENT_VALUE_CHANGED)
        {
            launcher_obj_main.icon_language = lv_dropdown_get_selected(obj);
            for(i=0;i<MAX_ICON_NUM;i++)
            {
                lv_label_set_text(launcher_obj_main.icon_name[i],icon_name_tal[launcher_obj_main.icon_language][move_icon_id[i/8][i%8]]);
                lv_obj_align(launcher_obj_main.icon_name[i],launcher_obj_main.icon_img[i],LV_ALIGN_OUT_BOTTOM_MID,0,5);
            }
        }
    }
}
void launcher_set_create1(lv_obj_t * par)
{
    if(launcher_set_flag == true)
    {
        return;
    }
    else {
        win_obj[WIN_OBJ_PAGE] = lv_page_create(par,NULL);
        lv_obj_set_size(win_obj[WIN_OBJ_PAGE],LV_HOR_RES_MAX*3>>2,LV_VER_RES_MAX>>2);
        lv_win_set_drag(win_obj[WIN_OBJ_PAGE],true);
        lv_obj_set_style_local_bg_opa(win_obj[WIN_OBJ_PAGE],LV_PAGE_PART_BG,LV_STATE_DEFAULT,LV_OPA_0);
        lv_obj_set_style_local_radius(win_obj[WIN_OBJ_PAGE],LV_PAGE_PART_BG,LV_STATE_DEFAULT,5);
        lv_obj_set_style_local_border_width(win_obj[WIN_OBJ_PAGE],LV_PAGE_PART_BG,LV_STATE_DEFAULT,1);
        lv_page_set_scrollable_fit(win_obj[WIN_OBJ_PAGE],LV_FIT_PARENT);
        lv_obj_align(win_obj[WIN_OBJ_PAGE],lv_scr_act(),LV_ALIGN_CENTER,0,0);

        win_obj[WIN_OBJ_CLOSE_BTN] = lv_btn_create(win_obj[WIN_OBJ_PAGE], NULL);
        lv_obj_set_style_local_bg_color(win_obj[WIN_OBJ_CLOSE_BTN],LV_PAGE_PART_BG,LV_STATE_DEFAULT,LV_COLOR_BLUE);
        lv_obj_set_style_local_bg_opa(win_obj[WIN_OBJ_CLOSE_BTN],LV_PAGE_PART_BG,LV_STATE_DEFAULT,LV_OPA_20);
        lv_obj_set_style_local_radius(win_obj[WIN_OBJ_CLOSE_BTN],LV_BTN_PART_MAIN,LV_STATE_DEFAULT,3);
        lv_obj_set_style_local_border_width(win_obj[WIN_OBJ_CLOSE_BTN],LV_BTN_PART_MAIN,LV_STATE_DEFAULT,1);
        lv_obj_set_size(win_obj[WIN_OBJ_CLOSE_BTN],52,32);
        lv_obj_align(win_obj[WIN_OBJ_CLOSE_BTN],win_obj[WIN_OBJ_PAGE],LV_ALIGN_IN_TOP_RIGHT,-15,15);

        win_obj[5] = lv_label_create(win_obj[WIN_OBJ_CLOSE_BTN],NULL);
        lv_label_set_text(win_obj[5],"关闭");
        lv_obj_set_style_local_text_color(win_obj[5],LV_LABEL_PART_MAIN,LV_STATE_DEFAULT,LV_COLOR_RED);
        lv_obj_set_event_cb(win_obj[WIN_OBJ_CLOSE_BTN], launcher_set_win_close_event_cb1);

        win_obj[6] = lv_label_create(win_obj[WIN_OBJ_PAGE],win_obj[5]);
        lv_label_set_text(win_obj[6],"桌面参数设置");
        lv_obj_align(win_obj[6],win_obj[WIN_OBJ_PAGE],LV_ALIGN_IN_TOP_MID,0,25);

        win_obj[7] = lv_label_create(win_obj[WIN_OBJ_PAGE],win_obj[5]);
        lv_label_set_text(win_obj[7],"虚拟按键开关");
        lv_obj_align(win_obj[7],win_obj[WIN_OBJ_PAGE],LV_ALIGN_IN_TOP_LEFT,32,75);
        win_obj[WIN_OBJ_SWT] = lv_switch_create(win_obj[WIN_OBJ_PAGE],NULL);
        if(launcher_obj_main.virtual_btn == true)
        {
            lv_switch_on(win_obj[WIN_OBJ_SWT],LV_ANIM_ON);
        }
        else
        {
            lv_switch_off(win_obj[WIN_OBJ_SWT],LV_ANIM_ON);
        }
        lv_obj_align(win_obj[WIN_OBJ_SWT],win_obj[7],LV_ALIGN_OUT_RIGHT_MID,15,0);
        lv_obj_set_event_cb(win_obj[WIN_OBJ_SWT], launcher_set_obj_change_event_cb1);

        static char * dropdown_text = "英语\n中文简体\n中文繁体\n韩文";
        win_obj[8] = lv_label_create(win_obj[WIN_OBJ_PAGE],win_obj[5]);
        lv_label_set_text(win_obj[8],"图标名称语言");
        lv_obj_align(win_obj[8],win_obj[7],LV_ALIGN_OUT_BOTTOM_LEFT,0, 25);

        win_obj[WIN_OBJ_DROP] = lv_dropdown_create(win_obj[WIN_OBJ_PAGE],NULL);
        lv_dropdown_set_dir(win_obj[WIN_OBJ_DROP],LV_DROPDOWN_DIR_DOWN);
        lv_dropdown_set_options(win_obj[WIN_OBJ_DROP],dropdown_text);
        lv_dropdown_set_selected(win_obj[WIN_OBJ_DROP],launcher_obj_main.icon_language);
        lv_obj_align(win_obj[WIN_OBJ_DROP],win_obj[8],LV_ALIGN_OUT_RIGHT_MID,15, 0);
        lv_obj_set_event_cb(win_obj[WIN_OBJ_DROP], launcher_set_obj_change_event_cb1);

        launcher_set_flag = true;
    }
}

static lv_event_cb_t icon_click_event_cb(lv_obj_t * icon, lv_event_t e)
{
    int i;
    if(e == LV_EVENT_SHORT_CLICKED)
    {
        for(i=0;i<MAX_ICON_NUM;i++)
        {
            if(icon == launcher_obj_main.icon_img[i])
            {
                if(launcher_set_flag == true)
                {
                    lv_obj_del(win_obj[WIN_OBJ_PAGE]);
                    launcher_set_flag = false;
                }
                printf("icon num:%d\n",i);
                if(i == 4)  /* clock */
                {
                    app_calendar_create(lv_scr_act());
                }
                else if(i == 3)  /* mpu */
                {
                    app_foc_create(lv_scr_act());
                }
                else if(i == 9)
                {
                    app_paint_create(lv_scr_act());
                }
                else if(i == 0)
                {
                    app_notebook_create(lv_scr_act());
                }
            }
        }
    }
    if(e == LV_EVENT_LONG_PRESSED)
    {
        for(i=0;i<MAX_ICON_NUM;i++)
        {
            if(icon == launcher_obj_main.icon_img[i])
            {
                printf("long press icon num:%d\n",i);
            }
        }
        launcher_set_create1(lv_scr_act());
    }
}

static void launcher_static_icon_create(lv_obj_t * par)
{
    int i;
#if SHADOW_LABEL_EN
    lv_obj_t * shadow_label;
#endif
    if(launcher_obj_main.virtual_btn == true)
    {
        for(i=0;i<3;i++)
        {
            if(img_vir_btn[i] == NULL)
            {
                img_vir_btn[i] = lv_imgbtn_create(par,NULL);
            }
            lv_imgbtn_set_src(img_vir_btn[i],LV_BTN_STATE_RELEASED,img_array_vir_key[i]);
            lv_imgbtn_set_src(img_vir_btn[i],LV_BTN_STATE_PRESSED,img_array_vir_key[i]);

            lv_obj_align(img_vir_btn[i],par,LV_ALIGN_IN_BOTTOM_MID,\
                         (1-i) * lv_obj_get_width(par) >> 2,-lv_obj_get_height(par)>>4);
            lv_obj_set_style_local_image_recolor(img_vir_btn[i], \
                                                 LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
            lv_obj_set_style_local_image_recolor_opa(img_vir_btn[i], \
                                                     LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
            if(lv_obj_get_hidden(img_vir_btn[i]) == true)
            lv_obj_set_hidden(img_vir_btn[i],false);
        }
    }
    else
    {
        for(i=0;i<3;i++)
        {
            if(lv_obj_get_hidden(img_vir_btn[i]) == false)
            lv_obj_set_hidden(img_vir_btn[i],true);
        }
    }
}

//static void led_icon_page_handle(void)
//{
//    int i;
//    lv_coord_t coordsx = 0;
//    for(i=0;i<launcher_obj_main.page_num;i++)
//    {
//        lv_obj_align(led_icon_page[0],lv_obj_get_parent(led_icon_page[0]),LV_ALIGN_IN_TOP_MID,\
//                     -(coordsx*LED_ICON_PAGE_DIS/LV_HOR_RES_MAX),6);
//        if(coordsx < 0)
//        {
//            lv_obj_align(led_icon_page[2],lv_obj_get_parent(led_icon_page[2]),LV_ALIGN_IN_TOP_MID,\
//                    (coordsx*LED_ICON_PAGE_DIS/LV_HOR_RES_MAX) + LED_ICON_PAGE_DIS,6);
//            lv_obj_align(led_icon_page[1],lv_obj_get_parent(led_icon_page[1]),LV_ALIGN_IN_TOP_MID,\
//                         (coordsx*LED_ICON_PAGE_DIS/LV_HOR_RES_MAX) - LED_ICON_PAGE_DIS,6);
//        }
//    }
//}

#include <time.h>
static char res_str[100];
static char *wday[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
time_t timep;
struct tm *p;

char * get_time_str(int mode)
{
    time(&timep);
    p = gmtime(&timep);
    sprintf(res_str,"%4d.%2d.%2d %s %2d:%02d:%02d",(1900+p->tm_year), (1+p->tm_mon), p->tm_mday, wday[p->tm_wday], (p->tm_hour + 8) % 24, p->tm_min, p->tm_sec);
    return res_str;
}

#if USE_3D_OBJ_DEMO
static lv_obj_t * obj_3d;
#endif
_pipa_obj pipa_obj_main[PIPA_OBJ_N];
_pipa_group pipa_group_main;
static lv_signal_cb_t ancestor_signal_cb;
static lv_task_t * task_test;
static lv_task_cb_t user_task_cb_test(lv_task_t * task);
launcher_obj_t launcher_obj_main;
top_menu_obj_t top_menu_obj_main;

static lv_task_cb_t user_task_cb_test(lv_task_t * task)
{
    static int old_x = 0;
    int i;
    lv_coord_t x_pos = (-1) * lv_obj_get_x(lv_page_get_scrollable(launcher_obj_main.tileview));
    if(old_x != x_pos)
    {
        //printf("Page pos:%d\r\n",x_pos);
        for(i=0;i<MAX_ICON_NUM;i++)
        {
            if(i<launcher_obj_main.icon_mov_num)
            {
                lv_obj_set_pos(launcher_obj_main.icon_img[i],launcher_obj_main.pos_init[i].x - x_pos,launcher_obj_main.pos_init[i].y);
                if(launcher_obj_main.rotate_icon)
                {
                    lv_img_set_angle(launcher_obj_main.icon_img[i],x_pos*3600/LV_HOR_RES_MAX);/* x*3600/480 */
                }
                if(launcher_obj_main.rasize_icon)
                {
                    lv_img_set_zoom(launcher_obj_main.icon_img[i],256 - (((LV_HOR_RES_MAX >> 1) - abs((x_pos % LV_HOR_RES_MAX) - (LV_HOR_RES_MAX >> 1))) >> 1));
                }
                lv_obj_align(launcher_obj_main.icon_name[i],launcher_obj_main.icon_img[i],LV_ALIGN_OUT_BOTTOM_MID,0,5);
                if(launcher_obj_main.ppos_led_en)
                {
                    lv_obj_align(launcher_obj_main.ppos_led[0],launcher_obj_main.icon_img[MAX_ICON_NUM-1-1],LV_ALIGN_OUT_TOP_MID,\
                                 (x_pos * LED_ICON_PAGE_DIS / LV_HOR_RES_MAX) - LED_ICON_PAGE_DIS,-7);
                    if(x_pos - LV_HOR_RES_MAX >= 0)
                    {
                        lv_obj_align(launcher_obj_main.ppos_led[2],launcher_obj_main.icon_img[MAX_ICON_NUM-1-1],LV_ALIGN_OUT_TOP_MID,\
                                     -(x_pos * LED_ICON_PAGE_DIS / LV_HOR_RES_MAX) + 2*LED_ICON_PAGE_DIS,-7);
                    }
                    else
                    {
                        lv_obj_align(launcher_obj_main.ppos_led[1],launcher_obj_main.icon_img[MAX_ICON_NUM-1-1],LV_ALIGN_OUT_TOP_MID,\
                                     -(x_pos * LED_ICON_PAGE_DIS / LV_HOR_RES_MAX),-7);
                    }
                }
            }
        }
        old_x = x_pos;
    }
    lv_label_set_text_fmt(top_menu_obj_main.time_label,"%s",get_time_str(1));
}

static lv_res_t top_menu_signal_cb(lv_obj_t * obj, lv_signal_t signal, void * param)
{
    lv_res_t res;
    res = ancestor_signal_cb(obj, signal, param);
    if(res != LV_RES_OK) return res;

    if(signal == LV_SIGNAL_DRAG_THROW_BEGIN) {
        lv_coord_t y = lv_obj_get_y(obj);
        lv_indev_t * indev = lv_indev_get_act();
        lv_point_t vect;
        lv_indev_get_vect(indev, &vect);
        lv_coord_t y_predict = 0;
        while(vect.y != 0) {
            y_predict += vect.y;
            vect.y = vect.y * (100 - LV_INDEV_DEF_DRAG_THROW) / 100;
        }
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, obj);
        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t) lv_obj_set_y);
        lv_anim_set_time(&a, 200);
        lv_indev_finish_drag(lv_indev_get_act());
        if(y + y_predict > -LV_VER_RES / 2) {
            lv_anim_set_values(&a, y, -TASK_HEIGH);
        } else {
            lv_anim_set_values(&a, y, -lv_obj_get_height(obj) + TASK_HEIGH);
        }
        lv_anim_start(&a);
    }
    else if(signal == LV_SIGNAL_COORD_CHG) {
        lv_coord_t y = lv_obj_get_y(obj);
        lv_coord_t h = lv_obj_get_height(obj);
        if(y > -TASK_HEIGH) lv_obj_set_y(obj, -TASK_HEIGH);
        if(obj->coords.y2 < TASK_HEIGH) lv_obj_set_y(obj, -h + TASK_HEIGH);
    }
    return LV_RES_OK;
}

#if 0
lv_obj_t * top_menu_create(lv_obj_t * parent)
{
    static lv_obj_t * top_menu;
    /*Create an empty white main container*/
    top_menu = lv_obj_create(parent, NULL);
    lv_obj_set_size(top_menu, LV_HOR_RES, LV_VER_RES + TASK_HEIGH);
    lv_obj_set_style_local_bg_opa(top_menu, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_20);
    lv_obj_set_style_local_radius(top_menu, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_border_width(top_menu, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_add_protect(top_menu, LV_PROTECT_PRESS_LOST);
    lv_obj_set_drag(top_menu, true);
    lv_obj_set_drag_throw(top_menu, true);
    lv_obj_set_drag_dir(top_menu, LV_DRAG_DIR_VER);
    lv_obj_set_y(top_menu, -TASK_BAR_STEPS);
    ancestor_signal_cb = lv_obj_get_signal_cb(top_menu);
    lv_obj_set_signal_cb(top_menu, top_menu_signal_cb);

    lv_obj_t * slider = lv_slider_create(top_menu,NULL);
    lv_slider_set_range(slider,0,100);
    lv_slider_set_type(slider,LV_SLIDER_TYPE_NORMAL);
    lv_slider_set_value(slider,50,200);
    lv_obj_align(slider,top_menu,LV_ALIGN_IN_TOP_MID,0,TASK_HEIGH * 2);

    lv_obj_t * time_label = lv_label_create(top_menu,NULL);
    get_time_str(0);
    lv_label_set_text(time_label,res_str);
    lv_obj_align(time_label,top_menu,LV_ALIGN_IN_BOTTOM_LEFT,10,-10);
    lv_obj_set_style_local_text_color(time_label,LV_LABEL_PART_MAIN,LV_STATE_DEFAULT,LV_COLOR_BLUE);

    return top_menu;
}
#endif

void top_menu_obj_init(top_menu_obj_t * obj,lv_obj_t * parent)
{
    int i;
    obj->menu_page = lv_obj_create(parent, NULL);
    lv_obj_set_size(obj->menu_page, LV_HOR_RES, LV_VER_RES + TASK_HEIGH);
    lv_obj_set_style_local_bg_opa(obj->menu_page, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_60);
    lv_obj_set_style_local_radius(obj->menu_page, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_border_width(obj->menu_page, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_add_protect(obj->menu_page, LV_PROTECT_PRESS_LOST);
    lv_obj_set_drag(obj->menu_page, true);
    lv_obj_set_drag_throw(obj->menu_page, true);
    lv_obj_set_drag_dir(obj->menu_page, LV_DRAG_DIR_VER);
    lv_obj_set_y(obj->menu_page, -TASK_BAR_STEPS);
    ancestor_signal_cb = lv_obj_get_signal_cb(obj->menu_page);
    lv_obj_set_signal_cb(obj->menu_page, top_menu_signal_cb);

    obj->time_label = lv_label_create(obj->menu_page,NULL);
    lv_label_set_text_fmt(obj->time_label,"%s",get_time_str(1));
    lv_obj_set_style_local_text_font(obj->time_label,LV_LABEL_PART_MAIN,LV_STATE_DEFAULT,&lv_font_montserrat_22);
    lv_obj_align(obj->time_label,obj->menu_page,LV_ALIGN_IN_BOTTOM_LEFT,5,-5);

    obj->msg_list = lv_list_create(obj->menu_page,NULL);
    lv_obj_set_style_local_text_font(obj->msg_list,LV_LABEL_PART_MAIN,LV_STATE_DEFAULT,&myFont12);
    lv_obj_set_style_local_border_width(obj->msg_list,LV_LIST_PART_BG,LV_STATE_DEFAULT,0);
    lv_obj_set_style_local_bg_opa(obj->msg_list,LV_LIST_PART_BG,LV_STATE_DEFAULT,LV_OPA_20);
    lv_obj_set_style_local_radius(obj->msg_list,LV_LIST_PART_BG,LV_STATE_DEFAULT,0);
    lv_obj_set_width_fit(obj->msg_list,lv_obj_get_width(obj->menu_page));
    lv_list_add_btn(obj->msg_list,LV_SYMBOL_CALL,"You receive a message!");
    lv_list_add_btn(obj->msg_list,LV_SYMBOL_CALL,"You receive a message too!");
    lv_list_add_btn(obj->msg_list,LV_SYMBOL_CALL,"You receive a message tooo!");
    lv_list_add_btn(obj->msg_list,LV_SYMBOL_CALL,"You receive a message toooo!");
    lv_list_add_btn(obj->msg_list,LV_SYMBOL_CALL,"You receive a message tooooo!");
    lv_list_add_btn(obj->msg_list,LV_SYMBOL_CALL,"You receive a message toooooo!");
    lv_list_add_btn(obj->msg_list,LV_SYMBOL_CALL,"You receive a message tooooooo!");
    lv_list_add_btn(obj->msg_list,LV_SYMBOL_CALL,"You receive a message toooooooo!");
    lv_list_add_btn(obj->msg_list,LV_SYMBOL_CALL,"You receive a message tooooooooo!");
    lv_list_add_btn(obj->msg_list,LV_SYMBOL_CALL,"You receive a message toooooooooo!");
    lv_list_add_btn(obj->msg_list,LV_SYMBOL_CALL,"You receive a message tooooooooooo!");
    lv_list_add_btn(obj->msg_list,LV_SYMBOL_CALL,"You receive a message toooooooooooo!");
    lv_list_add_btn(obj->msg_list,LV_SYMBOL_CALL,"You receive a message tooooooooooooo!");
    lv_list_add_btn(obj->msg_list,LV_SYMBOL_CALL,"You receive a message toooooooooooooo!");
    lv_list_add_btn(obj->msg_list,LV_SYMBOL_CALL,"You receive a message tooooooooooooooo!");
//    lv_list_add_btn(obj->msg_list,LV_SYMBOL_CALL,"收到一条未读短信");
//    lv_list_add_btn(obj->msg_list,LV_SYMBOL_CALL,"又收到一条未读短信");
//    lv_list_add_btn(obj->msg_list,LV_SYMBOL_CALL,"双收到一条未读短信");
//    lv_list_add_btn(obj->msg_list,LV_SYMBOL_CALL,"叒收到一条未读短信");
//    lv_list_add_btn(obj->msg_list,LV_SYMBOL_CALL,"叕收到一条未读短信");
    lv_obj_align(obj->msg_list,obj->menu_page,LV_ALIGN_IN_TOP_LEFT,0,35);
#if 0
    for(i=0;i<6;i++)
    {
        obj->time_img[i] = lv_img_create(obj->menu_page,NULL);
        lv_img_set_src(obj->time_img[i],num_img_array[0][0]);
        lv_obj_align(obj->time_img[i],obj->menu_page,LV_ALIGN_IN_BOTTOM_LEFT,i * (LV_HOR_RES_MAX / 6) + 10,-30);
    }
#endif
}

void init_icon_anim_cb(lv_obj_t * obj, lv_coord_t v)
{
#define INIT_X_POS LV_HOR_RES_MAX/2
#define INIT_Y_POS LV_VER_RES_MAX/2
    int i;
    int id = 0;
    for(i=0;i<MAX_ICON_NUM;i++)
    {
        if(obj == launcher_obj_main.icon_img[i])
        {
            id = i;
            break;
        }
    }
    int x_step,y_step;
    int icon_w = lv_obj_get_width(launcher_obj_main.icon_img[0])/2;
    x_step = INIT_X_POS - icon_w + v * (launcher_obj_main.pos_init[id].x - (INIT_X_POS - icon_w)) / PAGE_MOVE_STEPS;
    y_step = INIT_Y_POS - icon_w + v * (launcher_obj_main.pos_init[id].y - (INIT_Y_POS - icon_w)) / PAGE_MOVE_STEPS;
    lv_obj_set_pos(obj,x_step,y_step);
}

void launcher_obj_init(launcher_obj_t * obj,lv_obj_t * parent)
{
    int i = 0;
    int widget1,widget2;

    obj->parent_bg = lv_img_create(parent,NULL);
    if(LV_HOR_RES_MAX > LV_VER_RES_MAX)
    {
        lv_img_set_src(obj->parent_bg,&night);
    }
    else
    {
        lv_img_set_src(obj->parent_bg,&girl);
    }
    lv_obj_set_size(obj->parent_bg,LV_HOR_RES_MAX,LV_VER_RES_MAX);

    obj->icon_sta_num = 3;
    obj->icon_mov_num = MAX_ICON_NUM - obj->icon_sta_num;
    obj->page_num = 3;
    obj->icon_language = L_CHINESE;
    obj->rotate_icon = true;
    obj->rasize_icon = true;
    obj->ppos_led_en = true;

#if LAUNCHER_USE_TABVIEW
    obj->tabview = lv_tabview_create(obj->parent_bg,NULL);
#else
    static lv_point_t valid_pos[] = {{0,0}, {1, 0}, {2,0}};
    obj->tileview = lv_tileview_create(obj->parent_bg, NULL);
    lv_tileview_set_valid_positions(obj->tileview, valid_pos, 3);
    lv_tileview_set_edge_flash(obj->tileview, true);
    lv_obj_set_style_local_bg_opa(obj->tileview,LV_TILEVIEW_PART_BG,LV_STATE_DEFAULT,LV_OPA_0);
    lv_obj_set_style_local_bg_opa(obj->tileview,LV_TILEVIEW_PART_SCROLLBAR,LV_STATE_DEFAULT,LV_OPA_0);
    for(i=0;i<obj->page_num;i++)
    {
        obj->page[i] = lv_obj_create(obj->tileview, NULL);
        lv_obj_set_size(obj->page[i], LV_HOR_RES_MAX, LV_VER_RES_MAX);
        lv_obj_set_pos(obj->page[i], (LV_HOR_RES_MAX) * i, 0);
        lv_tileview_add_element(obj->tileview, obj->page[i]);
        lv_obj_set_style_local_bg_opa(obj->page[i],LV_TILEVIEW_PART_BG,LV_STATE_DEFAULT,LV_OPA_0);
        lv_obj_set_style_local_border_width(obj->page[i],LV_TILEVIEW_PART_BG,LV_STATE_DEFAULT,0);
    }

#endif

    for(i=0;i<MAX_ICON_NUM;i++)
    {
        obj->icon_img[i] = lv_img_create(obj->parent_bg,NULL);
        lv_img_set_src(obj->icon_img[i],img_array_l[move_icon_id[i/8][i%8]]);

        obj->icon_name[i] = lv_label_create(obj->parent_bg,NULL);

        lv_obj_set_style_local_text_font(obj->icon_name[i],LV_LABEL_PART_MAIN,LV_STATE_DEFAULT,&IconFont12);
        lv_label_set_text(obj->icon_name[i],icon_name_tal[launcher_obj_main.icon_language][move_icon_id[i/8][i%8]]);
        lv_obj_set_style_local_text_color(obj->icon_name[i],LV_LABEL_PART_MAIN,LV_STATE_DEFAULT,LV_COLOR_WHITE);
    }

    /*动态图标左右间隙宽度*/
    widget1 = (LV_HOR_RES_MAX - lv_obj_get_width(obj->icon_img[0]) * PAGE_ROW_ICON_NUM) / (PAGE_ROW_ICON_NUM + 1);
    /*静止图标左右间隙宽度*/
    widget2 = (LV_HOR_RES_MAX - lv_obj_get_width(obj->icon_img[0]) * obj->icon_sta_num - (obj->icon_sta_num - 1) * widget1) / 2;
    /*图标布局*/
    for(i=0;i<MAX_ICON_NUM;i++)
    {
        if(i<obj->icon_mov_num)
        {
            obj->pos_init[i].x = widget1 + ((i % 8) % 4) * (widget1 + lv_obj_get_width(obj->icon_img[0])) + ((i / 8)) * LV_HOR_RES_MAX;
            if(LV_HOR_RES_MAX > LV_VER_RES_MAX)
            {
                obj->pos_init[i].y = (widget1 >> 1) + ((i % 8) / 4) * ((widget1 >> 1) + lv_obj_get_width(obj->icon_img[0]));
            }
            else
            {
                obj->pos_init[i].y = widget1 * 2 + ((i % 8) / 4) * (widget1 * 2 + lv_obj_get_width(obj->icon_img[0]));
            }
            lv_obj_set_pos(obj->icon_img[i],obj->pos_init[i].x,obj->pos_init[i].y);
        }
        else
        {
            obj->pos_init[i].x = widget2 + (i - obj->icon_mov_num) * (widget1 + lv_obj_get_width(obj->icon_img[0]));
            if(LV_HOR_RES_MAX > LV_VER_RES_MAX)
            {
                obj->pos_init[i].y = (LV_VER_RES_MAX - 1) - (widget1 >> 1) - lv_obj_get_width(obj->icon_img[0]);
            }
            else
            {
                obj->pos_init[i].y = (LV_VER_RES_MAX - 1) - widget1 * 2 - lv_obj_get_width(obj->icon_img[0]);
            }
            lv_obj_set_pos(obj->icon_img[i],obj->pos_init[i].x,obj->pos_init[i].y);
        }
        lv_obj_align(obj->icon_name[i],obj->icon_img[i],LV_ALIGN_OUT_BOTTOM_MID,0,5);
        lv_obj_set_click(obj->icon_img[i],true);/*使能监听点击事件*/
        lv_obj_set_event_cb(obj->icon_img[i],icon_click_event_cb);
        //lv_obj_fade_in(obj->icon_img[i], 300, 100 * i);
        //lv_obj_fade_in(obj->icon_name[i], 300, 100 * i);
    }

    if(obj->ppos_led_en)
    {
        for(i=0;i<launcher_obj_main.page_num;i++)
        {
            if(i == 0)
            {
                obj->ppos_led[i] = lv_led_create(obj->parent_bg,NULL);
                lv_obj_set_style_local_bg_color(obj->ppos_led[0],LV_PAGE_PART_BG,LV_STATE_DEFAULT,LV_COLOR_WHITE);
                lv_obj_set_style_local_border_color(obj->ppos_led[0],LV_PAGE_PART_BG,LV_STATE_DEFAULT,LV_COLOR_WHITE);
                lv_obj_set_style_local_shadow_color(obj->ppos_led[0],LV_PAGE_PART_BG,LV_STATE_DEFAULT,LV_COLOR_WHITE);
                lv_obj_set_style_local_bg_opa(obj->ppos_led[0],LV_PAGE_PART_BG,LV_STATE_DEFAULT,LV_OPA_100);
                lv_obj_set_size(obj->ppos_led[0],7,7);
                lv_led_set_bright(obj->ppos_led[0],LV_LED_BRIGHT_MAX);
            }
            else
            {
                obj->ppos_led[i] = lv_led_create(obj->parent_bg,obj->ppos_led[i-1]);
                lv_obj_set_size(obj->ppos_led[i],5,5);
                lv_led_set_bright(obj->ppos_led[i],LV_LED_BRIGHT_MIN);
            }
            lv_obj_align(obj->ppos_led[i],obj->icon_img[MAX_ICON_NUM-1-1],LV_ALIGN_OUT_TOP_MID,(i-1)*LED_ICON_PAGE_DIS,-7);
        }
    }

    lv_anim_t icon_show_anim[MAX_ICON_NUM];
    lv_anim_path_t path;
    lv_anim_path_init(&path);
    lv_anim_path_set_cb(&path, lv_anim_path_bounce);
    for(i=0;i<MAX_ICON_NUM;i++)
    {
        lv_anim_init(&icon_show_anim[i]);                                               /*初始化动画变量*/
        lv_anim_set_path(&icon_show_anim[i], &path);                                    /*设定动画路径*/
        lv_anim_set_var(&icon_show_anim[i], obj->icon_img[i]);                          /*设定控件变量支持动画*/
        lv_anim_set_time(&icon_show_anim[i], 1200);                                     /*设定动画持续时间*/
        lv_anim_set_delay(&icon_show_anim[i], i * 100);                                 /*设定动画延迟*/
        lv_anim_set_values(&icon_show_anim[i], 0, PAGE_MOVE_STEPS);                     /*设定动画变量范围*/
        lv_anim_set_exec_cb(&icon_show_anim[i], (lv_anim_exec_xcb_t) init_icon_anim_cb);/*设定动画回调*/
        lv_anim_set_ready_cb(&icon_show_anim[i], NULL);                                 /*设定动画就绪回调*/
        lv_anim_start(&icon_show_anim[i]);                                              /*开始动画*/
    }
#if USE_3D_OBJ_DEMO
    obj_3d = lv_3d_create(obj->parent_bg,NULL);
    lv_obj_align(obj_3d,obj->parent_bg,LV_ALIGN_IN_BOTTOM_MID,0,-150);
    lv_obj_set_drag(obj_3d,true);
    lv_obj_set_drag_throw(obj_3d,true);
    lv_obj_set_style_local_border_width(obj_3d,LV_3D_PART_BG,LV_STATE_DEFAULT,0);
    lv_3d_set_distance(obj_3d,150);
#endif
    lv_obj_t * image = lv_img_create(obj->parent_bg,NULL);
    //image = lv_gif_create_from_file(obj->parent_bg, "QQicon/qq/f018.gif");//"S/lv_lib_gif/example.gif");
    lv_img_set_src(image, "QQicon/chat_tool_camera.png");
    lv_obj_align(image, obj->parent_bg, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
}

#if USE_3D_OBJ_DEMO
static lv_task_cb_t lv_3d_obj_ref_cb(lv_task_t * task)
{
    lv_3d_set_angle(obj_3d,(lv_tick_get()/50) % 360,(lv_tick_get()/60) % 360,0);
    pipa_group_loop(&pipa_group_main);
    //lv_obj_set_pos(obj_3d,pipa_obj_main[0].cur_x,pipa_obj_main[0].cur_y);
    //printf("pos:%d,%d spd:%d,%d\n",pipa_obj_main[0].cur_x,pipa_obj_main[0].cur_y,pipa_obj_main[0].speed_x,pipa_obj_main[0].speed_y);
//    lv_obj_set_pos(launcher_obj_main.icon_img[24],pipa_obj_main[0].cur_x,pipa_obj_main[0].cur_y);
//    lv_obj_set_pos(launcher_obj_main.icon_img[25],pipa_obj_main[1].cur_x,pipa_obj_main[1].cur_y);
//    lv_obj_set_pos(launcher_obj_main.icon_img[26],pipa_obj_main[2].cur_x,pipa_obj_main[2].cur_y);
}
#endif
void launcher_create(void)
{
    launcher_obj_init(&launcher_obj_main,lv_scr_act());
    top_menu_obj_init(&top_menu_obj_main,lv_scr_act());

    task_test = lv_task_create(user_task_cb_test, 30, LV_TASK_PRIO_MID, NULL);
#if USE_3D_OBJ_DEMO
    lv_task_create(lv_3d_obj_ref_cb, 20, LV_TASK_PRIO_LOW, NULL);
#endif
    pipa_group_init(&pipa_group_main);
}

void pipa_group_init(_pipa_group *obj)
{
    int i;
    obj->obj_num = PIPA_OBJ_N;
    obj->lim_x_max = LV_HOR_RES_MAX - 1;
    obj->lim_x_min = 0;
    obj->lim_y_max = LV_VER_RES_MAX - 1;
    obj->lim_y_min = 0;
    obj->loss = 100;/* 碰撞损失:95% */
    obj->obj = pipa_obj_main;
    for(i=0;i<obj->obj_num;i++)
    {
        if(1)
        {
            obj->obj[i].width = (rand() % 40) + 10;
            obj->obj[i].height = obj->obj[i].width;
        }
        obj->obj[i].cur_x = obj->lim_x_max / 2 + (rand()%(obj->lim_x_max/4)) - (obj->lim_x_max/8);
        obj->obj[i].cur_y = obj->lim_y_min + (rand()%(obj->lim_y_max*2/3));
        #if PIPA_OBJ_MEMLESS
        obj->obj[i].old_x = obj->obj[i].cur_x;
        obj->obj[i].old_y = obj->obj[i].cur_y;
        #endif
        obj->obj[i].lock = false;
        obj->obj[i].accel_x = 0;
        obj->obj[i].accel_y = 0;
        obj->obj[i].accel_z = 0;
        obj->obj[i].speed_x = (rand() % 180) - 90;
        obj->obj[i].speed_y = (rand() % 80) + 15;
        obj->obj[i].speed_z = 0;
        obj->obj[i].weight = obj->obj[i].width * obj->obj[i].height;
        obj->obj[i].cont_x = 0;
        obj->obj[i].cont_y = 0;
        obj->obj[i].cont_z = 0;
    }
}

void pipa_group_loop(_pipa_group *obj)
{
    int i,j;
    for(i=0;i<obj->obj_num;i++)
    {
        #if PIPA_OBJ_MEMLESS
        obj->obj[i].old_x = obj->obj[i].cur_x;
        obj->obj[i].old_y = obj->obj[i].cur_y;
        #endif
        if(obj->obj[i].lock != true)
        {
            if(obj->obj[i].speed_x > 0)
            {
                obj->obj[i].cont_x ++;
                if(obj->obj[i].cont_x >= (PIPA_OBJ_MAX_SPD - obj->obj->speed_x))
                {
                    obj->obj[i].cont_x = 0;
                    obj->obj[i].cur_x += PIPA_MOVE_STEP;
                    if(obj->obj[i].cur_x + obj->obj[i].width >= obj->lim_x_max)
                    {
                        obj->obj[i].speed_x *= -obj->loss;
                        obj->obj[i].speed_x /= 100;
                        obj->obj[i].cur_x -= PIPA_MOVE_STEP;
                    }
                }
            }
            else if(obj->obj[i].speed_x < 0)
            {
                obj->obj[i].cont_x --;
                if(obj->obj[i].cont_x <= (-PIPA_OBJ_MAX_SPD - obj->obj->speed_x))
                {
                    obj->obj[i].cont_x = 0;
                    obj->obj[i].cur_x -= PIPA_MOVE_STEP;
                    if(obj->obj[i].cur_x <= obj->lim_x_min)
                    {
                        obj->obj[i].speed_x *= -obj->loss;
                        obj->obj[i].speed_x /= 100;
                        obj->obj[i].cur_x += PIPA_MOVE_STEP;
                    }
                }
            }
            if(obj->obj[i].speed_y > 0)
            {
                obj->obj[i].cont_y ++;
                if(obj->obj[i].cont_y >= (PIPA_OBJ_MAX_SPD - obj->obj->speed_y))
                {
                    obj->obj[i].cont_y = 0;
                    obj->obj[i].cur_y += PIPA_MOVE_STEP;
                }
                if(obj->obj[i].cur_y + obj->obj[i].height >= obj->lim_y_max)
                {
                    obj->obj[i].speed_y *= -obj->loss;
                    obj->obj[i].speed_y /= 100;
                    obj->obj[i].cur_y -= PIPA_MOVE_STEP;
                }
            }
            else if(obj->obj[i].speed_y < 0)
            {
                obj->obj[i].cont_y --;
                if(obj->obj[i].cont_y <= (-PIPA_OBJ_MAX_SPD - obj->obj->speed_y))
                {
                    obj->obj[i].cont_y = 0;
                    obj->obj[i].cur_y -= PIPA_MOVE_STEP;
                }
                if(obj->obj[i].cur_y <= obj->lim_y_min)
                {
                    obj->obj[i].speed_y *= -obj->loss;
                    obj->obj[i].speed_y /= 100;
                    obj->obj[i].cur_y += PIPA_MOVE_STEP;
                }
            }
        }
    }
    for(i=0;i<obj->obj_num;i++)
    {

        for(j=0;j<obj->obj_num;j++)
        {
            if(j == i)
            {
                continue;
            }
            else
            {
                if((obj->obj[i].width + obj->obj[j].width)*(obj->obj[i].width + obj->obj[j].width) <\
                   (abs(obj->obj[i].cur_x - obj->obj[j].cur_x) * abs(obj->obj[i].cur_x - obj->obj[j].cur_x)) +\
                   (abs(obj->obj[i].cur_y - obj->obj[j].cur_y) * abs(obj->obj[i].cur_y - obj->obj[j].cur_y)))
                {
                    if(obj->obj[i].cur_x == obj->obj[j].cur_x)
                    {
                        int emc = obj->obj[i].weight * abs(obj->obj[i].speed_x) + obj->obj[j].weight * abs(obj->obj[j].speed_x);
                    }
                    if(obj->obj[i].cur_y == obj->obj[j].cur_y)
                    {
                        ;
                    }
                }
            }
        }
    }
}

/*
    pipa_group_init(&pipa_group_main);
    while(1)
    {
        for(i = 0;i < 100;i ++)
        {
            pipa_group_loop(&pipa_group_main);

        for(value = 0;value < pipa_group_main.obj_num;value ++)
        {
            monitor_draw_cicle(\
            pipa_group_main.obj[value].old_x + \
            pipa_group_main.obj[value].width/2,\
            pipa_group_main.obj[value].old_y + \
            pipa_group_main.obj[value].width/2,\
            pipa_group_main.obj[value].width/2,LV_COLOR_BLACK);

            monitor_draw_cicle(\
            pipa_group_main.obj[value].cur_x + \
            pipa_group_main.obj[value].width/2,\
            pipa_group_main.obj[value].cur_y + \
            pipa_group_main.obj[value].width/2,\
            pipa_group_main.obj[value].width/2,LV_COLOR_RED);
        }
        }
        usleep(5 * 200);
    }
*/
