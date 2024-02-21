#include <ros/ros.h>
#include <std_msgs/Int8.h>
#include <std_msgs/Float64.h>
#include <sensor_msgs/LaserScan.h>
#include <std_msgs/Float32.h>
#include <my_robot_msgs/BumperbotFSM_msgs.h>
#include <math.h>

#define pi 3.1415926535
#define WR 0.033
#define WS 0.172
#define ANG_VEL 2*pi
#define LINE_VEL 2*pi
#define DIRECT_DISTANCE 0.18
#define CHECK_RADIUS 0.5

ros::Publisher pub;
std_msgs::Float32 msg_front;

float angular_vel_R = 0, angular_vel_L = 0;
float angular_vel= 0;

float arr_center[360] = {0};

/*!
    @brief ham kiem tra phia truoc co bi dung vat can hay khong
    @param arr_c mang gom 180 phan tu, chua khoang cach cac vat can phia truoc robot
    @return khoang cach toi vat can gan nhat hoac 999 neu khong gap vat can

*/
float Check_Front(float * arr_c, float limit_distance){
    float phi = 0; // goc tao boi OP va Ox 
    float xP_L,xP_R;
    float obs_distance = 999;
    // kiem tra cac diem ben trai - phai robot
    for (uint8_t i = 0;i <90;i++){
         // chuyen do qua radian
        phi = i*pi/180;
        xP_L = arr_c[89-i]*cos(phi);
        xP_R = arr_c[90+i]*cos(phi);
        if(arr_c[89-i]*sin(phi)<0.11 && xP_L<limit_distance) if(xP_L < obs_distance) obs_distance = xP_L; 
        if(arr_c[90+i]*sin(phi)<0.11 && xP_R<limit_distance) if(xP_R < obs_distance) obs_distance = xP_R;
    }
    return obs_distance;
}


void GetLidarData(const sensor_msgs::LaserScan &msg){
    
    
    float arr_front[180] = {0};
    // dua toa do tu lidar vao mang
    for(size_t i = 0; i < 180;i++){
        arr_center[i] = msg.ranges.at(179 - i); // index cua tia thang theo phuong x la 180  
    }
    
    for(size_t i = 0; i<180;i++ ){
        arr_center[i+180] = msg.ranges.at(359-i);
    }

    for(size_t i = 0; i<180;i++ ){
        arr_front[i] = arr_center[i+90]; 
    }

    //*** Kiem tra xe co tiep tuc di thang duoc hay khong ***//
    msg_front.data = Check_Front(arr_front,DIRECT_DISTANCE) ;
    ROS_INFO("Khoang cach: %f \n",msg_front.data);
    pub.publish(msg_front);
    
    //***

}

void callback_receive_Lidardata(const sensor_msgs::LaserScan &msg){
    GetLidarData(msg);
}


// void callback_timer(const ros::TimerEvent &event){
//     theta = angular_vel*1/1000+theta;   
// }

// void callback_timer2(const ros::TimerEvent &event){
//     ROS_INFO("--------------------------------\n");
//     ROS_INFO("goc: %f \n",theta*180/pi);
// }


int main(int argc, char ** argv){
    ros::init(argc,argv,"CheckFront_Node");
    ros::NodeHandle nh;

    ros::Subscriber sub = nh.subscribe("/scan",
            1000,callback_receive_Lidardata);
    pub = nh.advertise<std_msgs::Float32>("/obs_distance",10);
 
    ros::spin();

}