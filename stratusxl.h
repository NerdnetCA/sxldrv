
#ifndef STRATUSXL_H
#define STRATUSXL_H

#include <unistd.h>
#include <stdlib.h>

#include <sys/types.h>

//#define SXL_NORTH_BIT   4
#define SXL_SOUTH_BIT   0
#define SXL_EAST_BIT    1
//#define SXL_WEST_BIT    3
#define SXL_LB_BIT      6
#define SXL_RB_BIT      7

//--------------------------------------------
// Swap the west and north bits to work around a bug that
// is causing them to be assigned incorrectly.
#define SXL_NORTH_BIT   3
#define SXL_WEST_BIT    4
// -------------------------------------------

#define SXL_START_BIT   3
#define SXL_THUMBL_BIT  5
#define SXL_THUMBR_BIT  6

#define SXL_BACK_BIT    0
#define SXL_HOME_BIT    1

#define SXL_LEFT_BIT    0
#define SXL_RIGHT_BIT   1
#define SXL_MIDDLE_BIT  2

#define SXL_NORTH_MASK  (1<<SXL_NORTH_BIT)
#define SXL_SOUTH_MASK  (1<<SXL_SOUTH_BIT)
#define SXL_EAST_MASK  (1<<SXL_EAST_BIT)
#define SXL_WEST_MASK  (1<<SXL_WEST_BIT)
#define SXL_RB_MASK  (1<<SXL_RB_BIT)
#define SXL_LB_MASK  (1<<SXL_LB_BIT)

#define SXL_START_MASK  (1<<SXL_START_BIT)
#define SXL_THUMBL_MASK  (1<<SXL_THUMBL_BIT)
#define SXL_THUMBR_MASK  (1<<SXL_THUMBR_BIT)

#define SXL_BACK_MASK  (1<<SXL_BACK_BIT)
#define SXL_HOME_MASK  (1<<SXL_HOME_BIT)

#define SXL_LEFT_MASK  (1<<SXL_LEFT_BIT)
#define SXL_RIGHT_MASK  (1<<SXL_RIGHT_BIT)
#define SXL_MIDDLE_MASK  (1<<SXL_MIDDLE_BIT)

#define SXLR1_HAT_BYTE          1
#define SXLR1_BTNBLOCK1_BYTE    2
#define SXLR1_BTNBLOCK2_BYTE    3
#define SXLR1_AXISX_LO_BYTE     4
#define SXLR1_AXISX_HI_BYTE     5
#define SXLR1_AXISY_LO_BYTE     6
#define SXLR1_AXISY_HI_BYTE     7
#define SXLR1_AXISX2_LO_BYTE    8
#define SXLR1_AXISX2_HI_BYTE    9
#define SXLR1_AXISY2_LO_BYTE    10
#define SXLR1_AXISY2_HI_BYTE    11
#define SXLR1_AXISTR_LO_BYTE    12
#define SXLR1_AXISTR_HI_BYTE    13
#define SXLR1_AXISTL_LO_BYTE    14
#define SXLR1_AXISTL_HI_BYTE    15

#define SXLR2_BUTTONS_BYTE      1

#define SXL_REPORT_BUFFER_SIZE   50

#define TRIGGER_THRESH          1300


struct sxl_state {
    unsigned int north : 1;
    unsigned int south : 1;
    unsigned int east : 1;
    unsigned int west : 1;
    
    unsigned int rb : 1;
    unsigned int lb : 1;
    unsigned int thumbr : 1;
    unsigned int thumbl : 1;

    unsigned int start : 1;
    unsigned int back : 1;
    unsigned int home : 1;
    
    int16_t dpad_x; // -1 to 1 rest 0
    int16_t dpad_y; // -1 to 1 rest 0
    
    int16_t axis_x;
    int16_t axis_y;
    int16_t axis_x2;
    int16_t axis_y2;
    
    int16_t axis_lt;
    int16_t axis_rt;
};

struct sxl_report2 {
    u_int8_t repnum;
    u_int8_t back : 1;
    u_int8_t home : 1;
    u_int8_t none : 6;
    //u_int8_t keys;
};
struct sxl_report3 {
    u_int8_t repnum;
    u_int8_t buttonmap;
    int8_t rel_x;
    int8_t rel_y;
    int8_t rel_wheel;
};
struct sxl_report1 {
    u_int8_t repnum;
    u_int8_t hat;
    u_int8_t south : 1;
    u_int8_t east : 1;
    u_int8_t none1 : 1;
    u_int8_t west : 1;
    u_int8_t north : 1;
    u_int8_t none2 : 1;
    u_int8_t lb : 1;
    u_int8_t rb : 1;
    u_int8_t none3 : 3;
    u_int8_t start : 1;
    u_int8_t none4 : 1;
    u_int8_t thumbl : 1;
    u_int8_t thumbr : 1;
    u_int8_t none5 : 1;
    
    //u_int16_t buttonmap;
    int16_t axis_x1;
    int16_t axis_y1;
    int16_t axis_x2;
    int16_t axis_y2;
    int16_t trig_right;
    int16_t trig_left;
};

typedef struct sxl_state SXL_STATE;

int sxl_update(SXL_STATE* sxl, u_int8_t* buffer, int evfd);

#endif

