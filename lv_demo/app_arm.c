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

#define BASE_H          109 //底座高度
#define BASE_R          65  //底座半徑
#define HEAD_H          14  //末端高度偏移
#define HEAD_R          50  //末端长度偏移
#define ARM_MAIN_LEN    135 //主臂长度
#define ARM_AID_LEN     170 //副臂长度

/*
                         |
             135mm     |  |
                     |     |
                   |        |  170mm
                 |           |
       __65mm__|              |
      |                        |
  \   |                         |__50mm___
   \109mm                                 | 14mm
    \ |
     \|
  Base\
       \
        \
         \
*/

#define PI 3.1415926f
#define RAD2DEG(x) (x * PI / 180)
#define DEG2RAD(x) (x * 180 / PI)

#define D2RVAL 0.01745329f //0.01745329251994329576923690768489f
#define R2DVAL 57.2957795f  //57.295779513082320876798154814105f

//三维坐标点
typedef struct
{
    float x;
    float y;
    float z;
}point_3d;
//二维坐标点
typedef struct
{
    float x;
    float y;
}point_2d;

//线段数
#define LINE_NUM 11

//线段三维端点、二维坐标
point_3d main_point_3d[LINE_NUM];
point_2d main_point_2d[LINE_NUM];

#define BASE_NET_NUM 12

point_3d base_net_point_3d[BASE_NET_NUM * 2];
point_2d base_net_point_2d[BASE_NET_NUM * 2];

bool is_show_base_net = true;

//单位旋转矢量
float main_matrix[4][4];

typedef struct
{
    float x;
    float y;
    float z;
    float angle_base;
    float angle_main;
    float angle_aid;
}robot_arm;

robot_arm main_robot_arm;

//底座宽度
#define BASE_W 100

//仿真模拟主线
static lv_obj_t * main_arm_line[LINE_NUM];
static lv_obj_t * base_arm_line[BASE_NET_NUM];
//仿真模拟主线样式
static lv_style_t main_arm_line_style[LINE_NUM];
static lv_style_t base_line_style;
//仿真模拟主线短点
static lv_point_t main_arm_line_points[LINE_NUM][2];
static lv_point_t base_line_points[BASE_NET_NUM][2];
//仿真模拟主线颜色
static lv_color_t color_tab[4];
//仿真模拟定时任务
static lv_task_t * task;

//主视角参数调节
static float main_cam_ang[12];
////机械臂角度调节拖动条
//static lv_obj_t * main_arm_slider[3];

//连接点数据
int connect_arrays[LINE_NUM][2]=
{
    {0,1},
    {0,2},
    {0,3},
    {0,4},
    {0,5},
    {5,6},
    {6,7},
    {7,8},
    {8,9},
    {9,10},
};

typedef struct
{
    char * title;
    lv_obj_t * name;
    lv_obj_t * slider;
    lv_obj_t * value;
}lv_obj_slider_set;

typedef struct
{
    lv_obj_slider_set * slider;
    lv_obj_t * list;
    lv_coord_t w,h;
    lv_task_cb_t * chg_cb;
}lv_obj_setter;

//调整的参数个数
#define MAX_VAL_SETTER 12
//调整拖动条主体
lv_obj_slider_set main_slider_set[MAX_VAL_SETTER];
lv_obj_setter main_setter;
//调整范围
static int setter_range[MAX_VAL_SETTER][2]=
{
    {0,360},
    {0,360},
    {0,360},
    {512,8000},
    {0,480},
    {0,800},
    {0,180},
    {-85,175},
    {0,180},
    {-1000,1000},
    {0,1000},
    {-1000,1000},
};
//调整参数名
static char * setter_name[MAX_VAL_SETTER]=
{
    "X视角:",
    "Y视角:",
    "Z视角:",
    "视角距离:",
    "视角X偏移:",
    "视角Y偏移:",
    "主轴角度:",
    "主臂角度:",
    "副臂角度:",
    "X坐标:",
    "Y坐标:",
    "Z坐标:"
};

static char * setter_title[]=
{
    "视角调整",
    "",
    "",
    ""
    "",
    "",
    "",
    "角度调整",
    ""
    "",
    "",
    "坐标调整",
    "",
    ""
    "",
};

void Structure_3D(float MAT[4][4])
{
    MAT[0][0]=1;
    MAT[0][1]=0;
    MAT[0][2]=0;
    MAT[0][3]=0;// 1 0 0 0
    MAT[1][0]=0;
    MAT[1][1]=1;
    MAT[1][2]=0;
    MAT[1][3]=0;// 0 1 0 0
    MAT[2][0]=0;
    MAT[2][1]=0;
    MAT[2][2]=1;
    MAT[2][3]=0;// 0 0 1 0
    MAT[3][0]=0;
    MAT[3][1]=0;
    MAT[3][2]=0;
    MAT[3][3]=1;// 0 0 0 1
}

void MATRIX_copy(float sourceMAT[4][4],float targetMAT[4][4])
{
    int a,b;
    for(a=0; a<4; a++)
    {
        for(b=0; b<4; b++)
        {
            targetMAT[a][b]=sourceMAT[a][b];
        }
    }
}

void MATRIX_multiply(float MAT1[4][4],float MAT2[4][4],float newMAT[4][4])
{
    int a,b;
    for(a=0; a<4; a++)
    {
        for(b=0; b<4; b++)
        {
            newMAT[a][b]=MAT1[a][0]*MAT2[0][b]+MAT1[a][1]*MAT2[1][b]+\
                    MAT1[a][2]*MAT2[2][b]+MAT1[a][3]*MAT2[3][b];
        }
    }
}

void Translate3D(float MAT[4][4],int16_t tx,int16_t ty,int16_t tz)
{
    float lMAT[4][4];
    float tMAT[4][4];
    tMAT[0][0]=1;
    tMAT[0][1]=0;
    tMAT[0][2]=0;
    tMAT[0][3]=tx;	//	1 0 0 tx
    tMAT[1][0]=0;
    tMAT[1][1]=1;
    tMAT[1][2]=0;
    tMAT[1][3]=ty;	//	0 1 0 ty
    tMAT[2][0]=0;
    tMAT[2][1]=0;
    tMAT[2][2]=1;
    tMAT[2][3]=tz;	//	0 0 1 tz
    tMAT[3][0]=0;
    tMAT[3][1]=0;
    tMAT[3][2]=0;
    tMAT[3][3]=1;	//	0 0 0 1
    MATRIX_multiply(MAT,tMAT,lMAT); //相乘
    MATRIX_copy(lMAT,MAT);          //复制
}

void  Scale_3D(float MAT[4][4],float sx,float sy,float sz)
{
    float lMAT[4][4];
    float tMAT[4][4];
    tMAT[0][0]=sx;
    tMAT[0][1]=0;
    tMAT[0][2]=0;
    tMAT[0][3]=0;	//	sx0 0 0
    tMAT[1][0]=0;
    tMAT[1][1]=sy;
    tMAT[1][2]=0;
    tMAT[1][3]=0;	//	0 sy0 0
    tMAT[2][0]=0;
    tMAT[2][1]=0;
    tMAT[2][2]=sz;
    tMAT[2][3]=0;	//	0 0 sz0
    tMAT[3][0]=0;
    tMAT[3][1]=0;
    tMAT[3][2]=0;
    tMAT[3][3]=1;	//	0 0 0 1
    MATRIX_multiply(MAT,tMAT,lMAT);	//相乘
    MATRIX_copy(lMAT,MAT);          //复制
}

void Rotate_3D(float MAT[4][4],float ax,float ay,float az)
{
    float MAT1[4][4];
    float MAT2[4][4];
    float MATx[4][4];
    float MATy[4][4];
    float MATz[4][4];

    ax = D2RVAL * ax;	//角度转换为弧度量
    ay = D2RVAL * ay;
    az = D2RVAL * az;
    /*****************************绕x轴旋转********************************************/

    MATx[0][0]=1;
    MATx[0][1]=0;
    MATx[0][2]=0;
    MATx[0][3]=0;//1        0        0              0
    MATx[1][0]=0;
    MATx[1][1]=cos(ax);
    MATx[1][2]=-sin(ax);
    MATx[1][3]=0;//0        cos(ax)  -sin(ax)       0
    MATx[2][0]=0;
    MATx[2][1]=sin(ax);
    MATx[2][2]=cos(ax);
    MATx[2][3]=0;//0        sin(ax)   cos(ax)       0
    MATx[3][0]=0;
    MATx[3][1]=0;
    MATx[3][2]=0;
    MATx[3][3]=1;//0        0        0              1

    /*****************************绕y轴旋转********************************************/
    MATy[0][0]=cos(ay);
    MATy[0][1]=0;
    MATy[0][2]=sin(ay);
    MATy[0][3]=0;//cos(ay)  0     sin(ay)   0
    MATy[1][0]=0;
    MATy[1][1]=1;
    MATy[1][2]=0;
    MATy[1][3]=0;//0        1     0         0
    MATy[2][0]=-sin(ay);
    MATy[2][1]=0;
    MATy[2][2]=cos(ay);
    MATy[2][3]=0;//-sin(ay) 0     cos(ay)   0
    MATy[3][0]=0;
    MATy[3][1]=0;
    MATy[3][2]=0;
    MATy[3][3]=1;//0        0     0         1

    /*****************************绕z轴旋转********************************************/
    MATz[0][0]=cos(az);
    MATz[0][1]=-sin(az);
    MATz[0][2]=0;
    MATz[0][3]=0;//cos(az) -sin(az)  0       0
    MATz[1][0]=sin(az);
    MATz[1][1]=cos(az);
    MATz[1][2]=0;
    MATz[1][3]=0;//sin(az)  cos(az)  0       0
    MATz[2][0]=0;
    MATz[2][1]=0;
    MATz[2][2]=1;
    MATz[2][3]=0;//0       0         1       0
    MATz[3][0]=0;
    MATz[3][1]=0;
    MATz[3][2]=0;
    MATz[3][3]=1;//0       0         0       1

    MATRIX_multiply(MAT,MATx,MAT1);
    MATRIX_multiply(MAT1,MATy,MAT2);
    MATRIX_multiply(MAT2,MATz,MAT);
}

point_3d vector_matrix_MULTIPLY(point_3d Source,float MAT[4][4])
{
    point_3d Result;
    Result.x=Source.x*MAT[0][0]+Source.y*MAT[1][0]+Source.z*MAT[2][0]+MAT[3][0];
    Result.y=Source.x*MAT[0][1]+Source.y*MAT[1][1]+Source.z*MAT[2][1]+MAT[3][1];
    Result.z=Source.x*MAT[0][2]+Source.y*MAT[1][2]+Source.z*MAT[2][2]+MAT[3][2];
    return Result;
}

point_2d OrtProject(point_3d Space)
{
    point_2d Screen;
    Screen.x=(int16_t)Space.x;
    Screen.y=(int16_t)Space.y;
    return Screen;
}

#define FOCAL_DISTANCE main_cam_ang[3]	//视点到平面的距离

point_2d PerProject(point_3d Space,int16_t XO,int16_t YO)
{
    point_2d Screen;
    if(Space.z == 0)
    {
        Space.z = 1;		//被除数不能为零
    }
    Screen.x = (int16_t)(FOCAL_DISTANCE * Space.x /(Space.z + FOCAL_DISTANCE) + XO);
    Screen.y = (int16_t)(FOCAL_DISTANCE * Space.y /(Space.z + FOCAL_DISTANCE) + YO);
    return Screen;
}

void init_matrix(float x,float y,float z)
{
    Structure_3D(main_matrix);
    Scale_3D(main_matrix,1,1,1);
    Rotate_3D(main_matrix,x,y,z);
    //Translate3D(main_matrix,8,8,8);
}

void calc_main_point(int x,int y)
{
    int i;
    point_3d temp;
    point_2d res;
    for(i=0;i<LINE_NUM;i++)
    {
        temp = vector_matrix_MULTIPLY(main_point_3d[i],main_matrix);
        res = PerProject(temp,x,y);
        main_point_2d[i].x = res.x;
        main_point_2d[i].y = res.y;
        //printf("p%3d:%5.2f,%5.2f\n",i, main_point_2d[i].x, main_point_2d[i].y);
        if((i==(LINE_NUM - 1))&&(false))
        {
            printf("End Pos:X:%5.1f,Y:%5.1f,Z:%5.1f\n",main_point_3d[i].x,main_point_3d[i].y,main_point_3d[i].z);
        }
    }
    if(is_show_base_net)
    {
        for(i=0;i<BASE_NET_NUM * 2;i++)
        {
            temp = vector_matrix_MULTIPLY(base_net_point_3d[i],main_matrix);
            res = PerProject(temp,x,y);
            base_net_point_2d[i].x = res.x;
            base_net_point_2d[i].y = res.y;
            //printf("p%3d:%5.2f,%5.2f\n",i, main_point_2d[i].x, main_point_2d[i].y);
        }
    }
}

void init_robot_arm_base(point_3d *obj)
{
    #define NET_W BASE_W/(BASE_NET_NUM / 4)
    int i;
    for(i=0;i<BASE_NET_NUM * 2;i++)
    {
        if(i < (BASE_NET_NUM / 2))
        {
            if(i%2)
            {
                (obj + i)->x = NET_W * (((i % (BASE_NET_NUM / 2))/2) + 1);
                (obj + i)->y = - BASE_NET_NUM * NET_W / 4;
                (obj + i)->z = 0;
            }
            else
            {
                (obj + i)->x = NET_W * (((i % (BASE_NET_NUM / 2))/2) + 1);
                (obj + i)->y = BASE_NET_NUM * NET_W / 4;
                (obj + i)->z = 0;
            }
        }
        else if(i < BASE_NET_NUM)
        {
            if(i%2)
            {
                (obj + i)->x = - NET_W * (((i % (BASE_NET_NUM / 2))/2) + 1);
                (obj + i)->y = - BASE_NET_NUM * NET_W / 4;
                (obj + i)->z = 0;
            }
            else
            {
                (obj + i)->x = - NET_W * (((i % (BASE_NET_NUM / 2))/2) + 1);
                (obj + i)->y = BASE_NET_NUM * NET_W / 4;
                (obj + i)->z = 0;
            }
        }
        else if(i < (BASE_NET_NUM * 3 / 2))
        {
            if(i%2)
            {
                (obj + i)->x = BASE_NET_NUM * NET_W / 4;
                (obj + i)->y = NET_W * (((i % (BASE_NET_NUM / 2))/2) + 1);
                (obj + i)->z = 0;
            }
            else
            {
                (obj + i)->x = - BASE_NET_NUM * NET_W / 4;
                (obj + i)->y = NET_W * (((i % (BASE_NET_NUM / 2))/2) + 1);
                (obj + i)->z = 0;
            }
        }
        else if(i < (BASE_NET_NUM * 2))
        {
            if(i%2)
            {
                (obj + i)->x = BASE_NET_NUM * NET_W / 4;
                (obj + i)->y = - NET_W * (((i % (BASE_NET_NUM / 2))/2) + 1);
                (obj + i)->z = 0;
            }
            else
            {
                (obj + i)->x = - BASE_NET_NUM * NET_W / 4;
                (obj + i)->y = - NET_W * (((i % (BASE_NET_NUM / 2))/2) + 1);
                (obj + i)->z = 0;
            }
        }
    }
}
void init_robot_arm(point_3d *obj)
{
    //底座中心坐标
    (obj + 0)->x = 0;
    (obj + 0)->y = 0;
    (obj + 0)->z = 0;

    //底座中心坐标前
    (obj + 1)->x = 0;
    (obj + 1)->y = BASE_W;
    (obj + 1)->z = 0;

    //底座中心坐标后
    (obj + 2)->x = 0;
    (obj + 2)->y = -BASE_W;
    (obj + 2)->z = 0;

    //底座中心坐标左
    (obj + 3)->x = -BASE_W;
    (obj + 3)->y = 0;
    (obj + 3)->z = 0;

    //底座中心坐标右
    (obj + 4)->x = BASE_W;
    (obj + 4)->y = 0;
    (obj + 4)->z = 0;

    //底座上支撑
    (obj + 5)->x = 0;
    (obj + 5)->y = 0;
    (obj + 5)->z = BASE_H;

    //底座上支撑前支撑
    (obj + 6)->x = 0;
    (obj + 6)->y = BASE_R;
    (obj + 6)->z = BASE_H;

    //底座上支撑前支撑主臂
    (obj + 7)->x = 0;
    (obj + 7)->y = BASE_R;
    (obj + 7)->z = BASE_H + ARM_MAIN_LEN;

    //底座上支撑前支撑副臂
    (obj + 8)->x = 0;
    (obj + 8)->y = BASE_R + ARM_AID_LEN;
    (obj + 8)->z = BASE_H + ARM_MAIN_LEN;

    //底座上支撑前支撑副臂下支撑
    (obj + 9)->x = 0;
    (obj + 9)->y = BASE_R + ARM_AID_LEN;
    (obj + 9)->z = BASE_H + ARM_MAIN_LEN - HEAD_H;

    //底座上支撑前支撑副臂下支撑前支撑
    (obj + 10)->x = 0;
    (obj + 10)->y = BASE_R + ARM_AID_LEN + HEAD_R;
    (obj + 10)->z = BASE_H + ARM_MAIN_LEN - HEAD_H;
}

void calc_robot_arm(point_3d *obj)
{
    float len_t,len_x,len_y,angle;
    //底座上支撑前支撑
    (obj + 6)->x = BASE_R * cos(main_cam_ang[6] * D2RVAL);
    (obj + 6)->y = BASE_R * sin(main_cam_ang[6] * D2RVAL);
    (obj + 6)->z = BASE_H;

    len_x = ARM_MAIN_LEN * cos(main_cam_ang[7] * D2RVAL);
    len_y = ARM_MAIN_LEN * sin(main_cam_ang[7] * D2RVAL);
    len_t = len_x + BASE_R;
    //底座上支撑前支撑主臂
    (obj + 7)->x = len_t * cos(main_cam_ang[6] * D2RVAL);
    (obj + 7)->y = len_t * sin(main_cam_ang[6] * D2RVAL);
    (obj + 7)->z = (obj + 6)->z + len_y;

    angle = (main_cam_ang[7] + main_cam_ang[8] - 180);
    len_x += ARM_AID_LEN * cos(angle * D2RVAL);
    len_y += ARM_AID_LEN * sin(angle * D2RVAL);
    //len_t = sqrt(len_x * len_x + len_y * len_y);
    //底座上支撑前支撑副臂
    (obj + 8)->x = (len_x + BASE_R) * cos(main_cam_ang[6] * D2RVAL);
    (obj + 8)->y = (len_x + BASE_R) * sin(main_cam_ang[6] * D2RVAL);
    (obj + 8)->z = (obj + 6)->z + len_y;

    (obj + 9)->x = (obj + 8)->x;
    (obj + 9)->y = (obj + 8)->y;
    (obj + 9)->z = (obj + 8)->z - HEAD_H;

    (obj + 10)->x = (len_x + BASE_R + HEAD_R) * cos(main_cam_ang[6] * D2RVAL);
    (obj + 10)->y = (len_x + BASE_R + HEAD_R) * sin(main_cam_ang[6] * D2RVAL);
    (obj + 10)->z = (obj + 9)->z;
}

int pos_to_ang(robot_arm * obj)
{
    #define ARM_MAIN_LENPOW2 18225
    #define ARM_AID_LENPOW2  28900
    #define ARM_MAIN_AID_C2  45900

    float len_xy;
    float len_x,len_y,len_x2y,len_x2ypow;

    float ang_x2y;

    len_xy = sqrt(obj->x * obj->x + obj->y * obj->y);
    obj->angle_base = acos(obj->x / len_xy) * R2DVAL;

    len_x = len_xy - BASE_R - HEAD_R;
    len_y = obj->z + HEAD_H - BASE_H;

    len_x2ypow = len_x * len_x + len_y * len_y;
    len_x2y = sqrt(len_x2ypow);


    obj->angle_aid = acos((ARM_MAIN_LENPOW2 + ARM_AID_LENPOW2 - \
    len_x2ypow) / ARM_MAIN_AID_C2) * R2DVAL;

    ang_x2y = asin(len_y / len_x2y);

    if(len_x > 0)
    {
        obj->angle_main = (acos((ARM_MAIN_LENPOW2 + len_x2y * len_x2y - \
        ARM_AID_LENPOW2)/(2 * ARM_MAIN_LEN * len_x2y)) + ang_x2y) * R2DVAL;
    }
    else if(len_x < 0)
    {
        obj->angle_main = (acos((ARM_MAIN_LENPOW2 + len_x2y * len_x2y - \
        ARM_AID_LENPOW2)/(2 * ARM_MAIN_LEN * len_x2y)) - ang_x2y) * R2DVAL;
        if(obj->angle_main > 180) obj->angle_main -= 180;

        if(obj->angle_main < 0) obj->angle_main += 180;
    }
    return 0;
}

static lv_task_cb_t user_task_cb(lv_task_t * task)
{
    static int p;
    int i;
    p ++;

    init_matrix(main_cam_ang[0],main_cam_ang[1],main_cam_ang[2]);
    calc_robot_arm(main_point_3d);
    calc_main_point(main_cam_ang[4],main_cam_ang[5]);

    for(i=0;i<LINE_NUM;i++)
    {
        main_arm_line_points[i][0].x = main_point_2d[connect_arrays[i][0]].x;
        main_arm_line_points[i][0].y = main_point_2d[connect_arrays[i][0]].y;
        main_arm_line_points[i][1].x = main_point_2d[connect_arrays[i][1]].x;
        main_arm_line_points[i][1].y = main_point_2d[connect_arrays[i][1]].y;
        lv_line_set_points(main_arm_line[i],main_arm_line_points[i],2);
    }
    if(is_show_base_net)
    {
        for(i=0;i<BASE_NET_NUM;i++)
        {
            base_line_points[i][0].x = base_net_point_2d[i*2].x;
            base_line_points[i][0].y = base_net_point_2d[i*2].y;
            base_line_points[i][1].x = base_net_point_2d[i*2+1].x;
            base_line_points[i][1].y = base_net_point_2d[i*2+1].y;
            lv_line_set_points(base_arm_line[i],base_line_points[i],2);
        }
    }
}

static void setter_slider_change_event_cb(lv_obj_t * obj, lv_event_t event)
{
    int i;
    int res;
    if(event == LV_EVENT_VALUE_CHANGED)
    {
        for(i=0;i<MAX_VAL_SETTER;i++)
        {
            if(obj == main_slider_set[i].slider)
            {
                res = lv_slider_get_value(obj);
                lv_label_set_text_fmt(main_setter.slider[i].value,"%5d",res);

                if(i < 9)
                {
                    //视角变换需要特殊变换
                    if(i < 3)
                    {
                        int x,y,z;
                        x = (int)lv_slider_get_value(main_slider_set[0].slider);
                        y = (int)lv_slider_get_value(main_slider_set[1].slider);
                        z = (int)lv_slider_get_value(main_slider_set[2].slider);

//                        y -= 90;
//                        z += 90;
                        main_cam_ang[0] = x;
                        main_cam_ang[1] = y;
                        main_cam_ang[2] = z;
                    }
                    //视角距离、偏移变换
                    else if(i < 6)
                    {
                        main_cam_ang[i] = res;
                    }
                    //角度参数调整
                    else if(i < 9)
                    {
                        main_cam_ang[i] = res;
                    }
                }
                else if(i < 12)
                {
                    main_cam_ang[i] = res;

                    main_robot_arm.x = main_cam_ang[9];
                    main_robot_arm.y = main_cam_ang[10];
                    main_robot_arm.z = main_cam_ang[11];
                    pos_to_ang(&main_robot_arm);

                    main_cam_ang[6] = main_robot_arm.angle_base;
                    main_cam_ang[7] = main_robot_arm.angle_main;
                    main_cam_ang[8] = main_robot_arm.angle_aid;

                    lv_slider_set_value(main_slider_set[6].slider,main_cam_ang[6],LV_ANIM_ON);
                    lv_slider_set_value(main_slider_set[7].slider,main_cam_ang[7],LV_ANIM_ON);
                    lv_slider_set_value(main_slider_set[8].slider,main_cam_ang[8],LV_ANIM_ON);
                }
            }
        }
    }
}

//static void slider_change_event_cb(lv_obj_t * obj, lv_event_t event)
//{
//    int i;
//    if(event == LV_EVENT_VALUE_CHANGED)
//    {
//        for(i=0;i<3;i++)
//        {
//            main_cam_ang[i] = lv_slider_get_value(main_arm_slider[i]);
//        }
//    }
//}

void app_arm_create(lv_obj_t * parent)
{
    int i;

    color_tab[0] = LV_COLOR_RED;
    color_tab[1] = LV_COLOR_GREEN;
    color_tab[2] = LV_COLOR_BLUE;
    color_tab[3] = LV_COLOR_YELLOW;

//    main_cam_ang[0] = 128;
//    main_cam_ang[1] = 286;
//    main_cam_ang[2] = 218;
//    main_cam_ang[0] = 0;
//    main_cam_ang[1] = 270;
//    main_cam_ang[2] = 90;
//    main_cam_ang[3] = 5000;
//    main_cam_ang[4] = 132;
//    main_cam_ang[5] = 430;

//    main_cam_ang[6] = 90;
//    main_cam_ang[7] = 90;
//    main_cam_ang[8] = 90;

    main_cam_ang[0] = 106;
    main_cam_ang[1] = 289;
    main_cam_ang[2] = 196;
    main_cam_ang[3] = 5000;
    main_cam_ang[4] = 118;
    main_cam_ang[5] = 470;

    main_cam_ang[6] = 90;
    main_cam_ang[7] = 67;
    main_cam_ang[8] = 38;

    main_cam_ang[9] = 200;
    main_cam_ang[10] = 250;
    main_cam_ang[11] = 130;

    main_robot_arm.angle_base = main_cam_ang[6];
    main_robot_arm.angle_main = main_cam_ang[7];
    main_robot_arm.angle_aid = main_cam_ang[8];

    init_robot_arm(main_point_3d);
    if(is_show_base_net)
    {
        init_robot_arm_base(base_net_point_3d);
    }
    init_matrix(main_cam_ang[0],main_cam_ang[1],main_cam_ang[2]);
    calc_main_point(main_cam_ang[4],main_cam_ang[5]);

    main_robot_arm.x = main_point_3d[LINE_NUM-1].x;
    main_robot_arm.y = main_point_3d[LINE_NUM-1].y;
    main_robot_arm.z = main_point_3d[LINE_NUM-1].z;

    if(is_show_base_net)
    {
        lv_style_init(&base_line_style);
        lv_style_set_line_width(&base_line_style, LV_STATE_DEFAULT, 2);
        lv_style_set_line_color(&base_line_style, LV_STATE_DEFAULT, LV_COLOR_GRAY);
        lv_style_set_line_rounded(&base_line_style, LV_STATE_DEFAULT, true);

        for(i=0;i<BASE_NET_NUM;i++)
        {
            base_arm_line[i] = lv_line_create(parent,NULL);
            base_line_points[i][0].x = base_net_point_2d[i*2].x;
            base_line_points[i][0].y = base_net_point_2d[i*2].y;
            base_line_points[i][1].x = base_net_point_2d[i*2+1].x;
            base_line_points[i][1].y = base_net_point_2d[i*2+1].y;
            lv_line_set_points(base_arm_line[i],base_line_points[i],2);
            lv_obj_add_style(base_arm_line[i], LV_LINE_PART_MAIN, &base_line_style);
        }
    }

    for(i=0;i<LINE_NUM;i++)
    {
        main_arm_line[i] = lv_line_create(parent,NULL);
        main_arm_line_points[i][0].x = 0;
        main_arm_line_points[i][0].y = i * 10;
        main_arm_line_points[i][1].x = 80;
        main_arm_line_points[i][1].y = i * 10;
        lv_line_set_points(main_arm_line[i],main_arm_line_points[i],2);

        lv_style_init(&main_arm_line_style[i]);
        lv_style_set_line_width(&main_arm_line_style[i], LV_STATE_DEFAULT, 5);
        lv_style_set_line_color(&main_arm_line_style[i], LV_STATE_DEFAULT, color_tab[i%4]);
        lv_style_set_line_rounded(&main_arm_line_style[i], LV_STATE_DEFAULT, true);
        lv_obj_add_style(main_arm_line[i], LV_LINE_PART_MAIN, &main_arm_line_style[i]);

//        if(i < 3)
//        {
//            main_arm_slider[i] = lv_slider_create(parent,NULL);
//            lv_slider_set_type(main_arm_slider[i],LV_SLIDER_TYPE_NORMAL);
//            lv_slider_set_range(main_arm_slider[i],0,359);
//            lv_slider_set_value(main_arm_slider[i],main_cam_ang[i],LV_ANIM_OFF);
//            lv_obj_set_event_cb(main_arm_slider[i],slider_change_event_cb);

//            lv_obj_align(main_arm_slider[i],parent,LV_ALIGN_IN_BOTTOM_MID,0,- (2-i) * 50 - 20);
//        }
    }

    main_setter.w = 460;
    main_setter.h = 380;
    main_setter.list = lv_list_create(parent,NULL);
    lv_obj_set_size(main_setter.list,main_setter.w,main_setter.h);
    lv_obj_align(main_setter.list,parent,LV_ALIGN_IN_BOTTOM_MID,0,0);
    main_setter.chg_cb = setter_slider_change_event_cb;
    main_setter.slider = main_slider_set;

    lv_obj_set_style_local_bg_opa(main_setter.list,LV_LIST_PART_BG,LV_STATE_DEFAULT,LV_OPA_70);
    for(i=0;i<MAX_VAL_SETTER;i++)
    {
        lv_obj_t * list_btn;
        main_slider_set[i].title = setter_title[i];
        if(main_slider_set[i].title != "")
        {
            list_btn = lv_list_add_btn(main_setter.list,LV_SYMBOL_RIGHT,main_slider_set[i].title);
            lv_obj_set_style_local_bg_opa(list_btn,LV_LIST_PART_BG,LV_STATE_DEFAULT,LV_OPA_80);
        }
        list_btn = lv_list_add_btn(main_setter.list,NULL,NULL);
        lv_obj_set_style_local_bg_opa(list_btn,LV_LIST_PART_BG,LV_STATE_DEFAULT,LV_OPA_0);
        main_slider_set[i].name = lv_label_create(list_btn,NULL);
        lv_label_set_text(main_slider_set[i].name,setter_name[i]);
        main_slider_set[i].value = lv_label_create(list_btn,NULL);
        lv_label_set_text_fmt(main_slider_set[i].value,"%5d",(int)main_cam_ang[i]);
        main_slider_set[i].slider = lv_slider_create(list_btn,NULL);
        lv_slider_set_type(main_slider_set[i].slider,LV_SLIDER_TYPE_NORMAL);
        lv_slider_set_range(main_slider_set[i].slider,setter_range[i][0],setter_range[i][1]);
        lv_slider_set_value(main_slider_set[i].slider,main_cam_ang[i],LV_ANIM_ON);
        lv_obj_set_event_cb(main_slider_set[i].slider,main_setter.chg_cb);
        //lv_obj_align(main_slider_set[i].slider,main_slider_set[i].name,LV_ALIGN_OUT_BOTTOM_LEFT,0,5);
    }

    task = lv_task_create(user_task_cb, 20, LV_TASK_PRIO_MID, NULL);
}
