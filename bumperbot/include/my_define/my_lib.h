#include <ros/ros.h>


#define pi 3.1415926535
#define LIMIT_DIRECT_DISTANCE 0.21    // khoang cach duong trong dinh huong

#define WR 0.033                      // ban kinh banh xe 
#define LINE_VEL 2*pi                 // van toc goc banh xe dung cho viec di chuyen thang

struct Position{
    int32_t x;
    int32_t y;
};

struct Vector{
    int32_t x;
    int32_t y;
    int16_t theta;
};

