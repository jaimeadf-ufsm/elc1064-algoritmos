//==================================================//
//    Multi-Platform Console (MPC) - v4.6.1 @ 2010  //
//     UFSM - Federal University of Santa Maria     //
//==================================================//

#ifndef __MPC_DEFINITIONS_H__
#define __MPC_DEFINITIONS_H__

static const int CHARACTER_WIDTH  = 8;
static const int CHARACTER_HEIGHT = 15;


// keyboard
static const int ACTIVE_SHIFT = 1;
static const int ACTIVE_CTRL  = 2;
static const int ACTIVE_ALT   = 4;

// graph size
static const int GRAPH_TINY   = 1;
static const int GRAPH_SMALL  = 2;
static const int GRAPH_MEDIUM = 3;
static const int GRAPH_LARGE  = 4;

// mpc fonts
static const int F_STD = 0;
static const int F_N   = 1;
static const int F_I   = 2;
static const int F_S   = 3;
static const int F_NS  = 4;
static const int F_SN  = 4;
static const int F_NI  = 5;
static const int F_IN  = 5;
static const int F_SI  = 6;
static const int F_IS  = 6;
static const int F_NSI = 7;
static const int F_NIS = 7;
static const int F_SIN = 7;
static const int F_SNI = 7;
static const int F_INS = 7;
static const int F_ISN = 7;

// mpc colors
#define BLACK     000, 000, 000
#define GREY_1    204, 204, 204
#define GREY_2    153, 153, 153
#define GREY_3    102, 102, 102
#define GREY_4    051, 051, 051
#define GREY_5    051, 051, 051
#define RED_1     255, 102, 102
#define RED_2     255, 051, 051
#define RED_3     255, 000, 000
#define RED_4     204, 000, 000
#define RED_5     153, 000, 000
#define GREEN_1   102, 255, 102
#define GREEN_2   000, 255, 051
#define GREEN_3   000, 204, 000
#define GREEN_4   000, 153, 000
#define GREEN_5   000, 102, 000
#define BLUE_1    051, 204, 255
#define BLUE_2    051, 153, 255
#define BLUE_3    000, 000, 255
#define BLUE_4    000, 000, 204
#define BLUE_5    000, 000, 153
#define CYAN_1    000, 050, 050
#define CYAN_2    000, 100, 100
#define CYAN_3    000, 150, 150
#define CYAN_4    000, 200, 200
#define CYAN_5    000, 255, 255
#define MAGENTA_1 050, 000, 050
#define MAGENTA_2 100, 000, 100
#define MAGENTA_3 150, 000, 150
#define MAGENTA_4 200, 000, 200
#define MAGENTA_5 255, 000, 255
#define YELLOW_1  255, 255, 204
#define YELLOW_2  255, 255, 153
#define YELLOW_3  255, 255, 000
#define YELLOW_4  255, 204, 000
#define YELLOW_5  255, 204, 000
#define ORANGE_1  255, 153, 000
#define ORANGE_2  255, 102, 000
#define GOLD      255, 204, 051
#define BROWN_1   153, 102, 000
#define BROWN_2   002, 051, 000
#define BROWN_3   051, 000, 000
#define PURPLE    102, 000, 153
#define WHITE     255, 255, 255
#define TRANSP    256, 256, 256

#endif // __MPC_DEFINITIONS_H__
