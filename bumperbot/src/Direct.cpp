#include <ros/ros.h>
#include <std_msgs/Int8.h>
#include <std_msgs/Float64.h>
#include <sensor_msgs/LaserScan.h>
#include <std_msgs/Float32.h>
#include <my_robot_msgs/BumperbotFSM_msgs.h>
#include <math.h>

ros::Publisher pub;
std_msgs::Int8 msg_direct;
sensor_msgs::LaserScan lidar_data;
#define pi 3.1415926535
#define CHECK_RADIUS 0.4
volatile bool check = 0;


void ChooseDirection2(const sensor_msgs::LaserScan &msg){
    float arr_center[270] = {0};
    bool avaiable = false;
    float lim_rad = CHECK_RADIUS;
    uint8_t loop_count = 0;
    uint8_t angular =0;
    bool positive = false;
    bool enable = false;
    //size_t omega = 0;
    //int8_t direction = 0;
    // dua toa do tu lidar vao mang
    for(size_t i = 0; i < 135;i++){
        arr_center[i] = msg.ranges.at(134 - i); // index cua tia thang theo phuong x la 180  
    }
    
    for(size_t i = 0; i<135;i++ ){
        arr_center[i+135] = msg.ranges.at(359-i);
    }

    uint16_t left_cnt =0, right_cnt = 0;
    while(!enable){
        left_cnt =0;
        right_cnt = 0;
        if(lim_rad > 0.11) lim_rad = lim_rad-loop_count*0.02;
        int16_t srtPoint = -1, endPoint = -1;
        angular = 2* (uint8_t)(asin(0.11/lim_rad)*180/pi);
        for(size_t i=0;i<135;i++){
            if(arr_center[i]> lim_rad){
                positive = true;
                for(size_t j=0;j<angular;j++){
                    if(arr_center[i+j]<lim_rad) {
                        positive = false;
                        i = i+j+1;
                        left_cnt += j;
                        break;
                    }

                }
                if(positive){
                    enable = true;
                    left_cnt++;
                }
            }
        }
        for(size_t i=135;i<270-angular;i++){
            if(arr_center[i]> lim_rad){
                positive = true;
                for(size_t j=0;j<angular;j++){
                    if(arr_center[i+j]<lim_rad) {
                        positive = false;
                        i = i+j+1;
                        right_cnt +=j;
                        break;
                    }

                }
                if(positive){
                    enable = true;
                    right_cnt++;
                }
            }
        }


    }
    int8_t data = 0;
    if(right_cnt > left_cnt) {
      ROS_INFO("ben phai\n");
        data = 0;
       msg_direct.data = data;
       pub.publish(msg_direct);
    }
    else {
        ROS_INFO("Ben trai\n");
        data = 1;
       msg_direct.data = data;
        msg_direct.data = 1;
        pub.publish(msg_direct);
       
    }

}


void callback_receive_request(const std_msgs::Int8 &msg){
    check = msg.data;
}

void callback_receive_Lidardata(const sensor_msgs::LaserScan &msg){
    lidar_data = msg;
}

int main(int argc, char ** argv){
    ros::init(argc,argv,"Direct_Node");
    ros::NodeHandle nh;

    ros::Subscriber sub = nh.subscribe("/scan",
            1000,callback_receive_Lidardata);
    ros::Subscriber sub2 = nh.subscribe("/check",
            1000,callback_receive_request);
    pub = nh.advertise<std_msgs::Int8>("/direction",10);
    ros::Rate rate(5);

    while (ros::ok())
    {   
        ros::spinOnce();
        if(check == 1){
          ChooseDirection2(lidar_data);
          check = 0;
        }
        rate.sleep();
        /* code */
    }

}