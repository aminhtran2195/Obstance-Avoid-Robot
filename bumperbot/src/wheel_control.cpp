#include <ros/ros.h>
#include <std_msgs/Int8.h>
#include <std_msgs/Float64.h>

#define pi 3.1459
#define STOP 0
#define GO_FORWARD 1
#define TURNLEFT 2
#define TURNRIGHT 3
#define GO_BACK 4

float line_vel = pi*2; // 4.823 vong` tren giay
float rotate_vel = pi*2;

int8_t state = 0;
ros::Publisher right_pub;
ros::Publisher left_pub;
std_msgs::Float64 msg_out;
std_msgs::Int8 robot_stt_msg;


void callback_receive_data(const std_msgs::Int8 &msg){
    ROS_INFO("receive msg: %d",msg.data);
    state = msg.data;
    switch (state)
    {
    case GO_FORWARD:
        msg_out.data = line_vel;
        right_pub.publish(msg_out);
        left_pub.publish(msg_out);
        break;
    case STOP:
        msg_out.data = 0;
        right_pub.publish(msg_out);
        left_pub.publish(msg_out);
        break;
    case TURNLEFT:
        msg_out.data = rotate_vel;
        right_pub.publish(msg_out);
        msg_out.data = 0 - rotate_vel;
        left_pub.publish(msg_out);
        break;
    case TURNRIGHT:
        msg_out.data = 0 - rotate_vel;
        right_pub.publish(msg_out);
        msg_out.data = rotate_vel;
        left_pub.publish(msg_out);
        break;
    case GO_BACK:
        msg_out.data = 3.14 - line_vel;
        right_pub.publish(msg_out);
        left_pub.publish(msg_out);
        break;
    default:
        break;
    }
}

int main(int argc, char ** argv){
    ros::init(argc,argv,"wheelcontrol_Node");
    ros::NodeHandle nh;
    ros::Subscriber sub = nh.subscribe("/wheel_control",
            10,callback_receive_data);
    right_pub = nh.advertise<std_msgs::Float64>("/wheel_right_controller/command",10);
    left_pub = nh.advertise<std_msgs::Float64>("/wheel_left_controller/command",10);
    ros::spin();
}
