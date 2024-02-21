#include <ros/ros.h>
#include <sensor_msgs/LaserScan.h>
#include <std_msgs/Float32.h>
#include <my_robot_msgs/BumperbotFSM_msgs.h>
#include <math.h>


#define pi 3.1416
#define expanded_range 0.2
#define mid_range 0.14
#define rotate_range 0.11
#define side_points 46

my_robot_msgs::BumperbotFSM_msgs output_msg;
ros::Publisher pub;

void Handle_LidarData(std::vector<float> data){
    output_msg.isBackBlocked = false;
    output_msg.isHeadBlocked = false;
    output_msg.isRotateBlocked = false;
    output_msg.LeftPoint = 0;
    output_msg.RightPoint = 0;

    uint32_t temp_RP = 0,temp_LP = 0;
    //  ========= kiem tra cung tron phia tren robot =========
    for (uint16_t i = 0; i < 90-side_points; i++){
        if(std::isinf(data.at(i))) continue;

        // cung tro`n phia tren ben TRAI --- tia: 0->41
        if(data.at(i)<expanded_range) {
            temp_LP++;
            if(data.at(i)<mid_range) {
                output_msg.isHeadBlocked = true;
                if(data.at(i)< rotate_range) output_msg.isRotateBlocked = true;
            }
        }
        // cung tro`n phia tren ben PHAI --- tia: 302->359
        if(data.at(359-i)<expanded_range) {
            temp_RP++;
            if(data.at(359-i)<mid_range) {
                output_msg.isHeadBlocked = true;
                if(data.at(i)< rotate_range) output_msg.isRotateBlocked = true;
            }
        }
    }
    
    // mode expand area duoc bat khi RP = LP 

    // ========= kiem tra 2 canh ben cua robot ======== 
    // i = 90 - side_point;
    for (uint16_t i = 0; i <side_points ; i++){
        // canh ben trai
        if(data.at(89-i)< ((float)rotate_range/cos(i*pi/180))) {
            if(data.at(89-i)<expanded_range) temp_LP++;
            if(data.at(89-i)< rotate_range){
                output_msg.isRotateBlocked = true;
            }
        }
        if(data.at(90+i)< ((float)rotate_range/cos(i*pi/180))) {
            if(data.at(90+i)<expanded_range) temp_LP++;
            if(data.at(90+i)< rotate_range){
                output_msg.isRotateBlocked = true;
            }
        }
        
        // canh ben phai
        if(data.at(270+i) < ((float)rotate_range/cos(i*pi/180))) {
            if(data.at(270+i)<expanded_range) temp_RP++;
            if(data.at(270+i)< rotate_range){
                output_msg.isRotateBlocked = true;
            }
        }
        if(data.at(269-i) < ((float)rotate_range/cos(i*pi/180))) {
            if(data.at(269-i)<expanded_range) temp_RP++;
            if(data.at(269-i)< rotate_range){
                output_msg.isRotateBlocked = true;
            }
        }
    }

    // ========= kiem tra xoay o phia sau robot ==========
    for(uint16_t i= 90+side_points ;i < 269-side_points ;i++){
        if(data.at(i)< (mid_range-0.02)){
            output_msg.isBackBlocked = true;
            if(data.at(i)< rotate_range) {
                output_msg.isRotateBlocked = true;
                break;
            }
        }
    }

    ROS_INFO("-------------");
    ROS_INFO("So diem ben trai %d",temp_LP);
    ROS_INFO("So diem ben phai %d",temp_RP);
    if(output_msg.isHeadBlocked) {
        ROS_INFO("bi can phia truoc ");
        output_msg.LeftPoint = temp_LP;
        output_msg.RightPoint = temp_RP;
    }
    if(output_msg.isBackBlocked) {
        ROS_INFO("bi can phia sau ");
    }
    
    if(output_msg.isRotateBlocked) ROS_INFO("khong the xoay");
    pub.publish(output_msg);
}

void callback_receive_data(const sensor_msgs::LaserScan &msg){
    Handle_LidarData(msg.ranges);
}

int main(int argc, char ** argv){
    ros::init(argc,argv,"HandleLidar_Node");
    ros::NodeHandle nh;
    pub = nh.advertise<my_robot_msgs::BumperbotFSM_msgs>("/input_FSM",10);
    ros::Subscriber sub = nh.subscribe("/scan",
            1000,callback_receive_data);
    ros::spin();
}