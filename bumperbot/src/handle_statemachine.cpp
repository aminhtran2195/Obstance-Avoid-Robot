#include <ros/ros.h>
#include <my_robot_msgs/BumperbotFSM_msgs.h>
#include <std_msgs/Int8.h>

#define STOP 0
#define GO_FORWARD 1
#define TURNLEFT 2
#define TURNRIGHT 3
#define GO_BACK 4

bool direct = true;

ros::Publisher pub;
int8_t state = STOP;

std_msgs::Int8 msg_out;

bool change_stt = false;


void FSM_DEBUG(){
    switch (state)
    {
    case GO_FORWARD:
        ROS_INFO("DI ve phia truoc");
        break;
    case TURNLEFT:
        ROS_INFO("Xoay trai");
        break;
    case TURNRIGHT:
        ROS_INFO("Xoay phai");
        break;
    case GO_BACK:
        ROS_INFO("Di ve phia sau");
        break;
    case STOP:
        ROS_INFO("Dung lai");
        break;
    default:
        break;
    }
}


void callback_receive_data(const my_robot_msgs::BumperbotFSM_msgs &msg){
    change_stt = false;
    switch (state)
    {
        case GO_FORWARD:
            if(msg.isHeadBlocked){
                if(!msg.isRotateBlocked){
                    if(msg.LeftPoint<= msg.RightPoint)
                        state = TURNLEFT;
                    else
                        state = TURNRIGHT;
                }
                else{
                    if(!msg.isBackBlocked)
                        state = GO_BACK;
                    else 
                        state = STOP;
                }
                change_stt = true;
            }
            break;

        case TURNLEFT:
            if(!msg.isHeadBlocked){
                state = GO_FORWARD;
                change_stt = true;
            }
            else{
                if(msg.isRotateBlocked){
                    if(!msg.isBackBlocked)
                        state = GO_BACK;
                    else
                        state = STOP;
                    change_stt = true;
                }
            }
            break;

        case TURNRIGHT:
            if(!msg.isHeadBlocked){
                state = GO_FORWARD;
                change_stt = true;
            }
            else{
                if(msg.isRotateBlocked){
                    if(!msg.isBackBlocked)
                        state = GO_BACK;
                    else
                        state = STOP;
                    change_stt = true;
                }
            }
            break;

        case GO_BACK:
            if(!msg.isHeadBlocked){
                state = GO_FORWARD;
                change_stt = true;
            }
            else{
                if(!msg.isRotateBlocked){
                    if(msg.LeftPoint<=msg.RightPoint)
                        state = TURNLEFT;
                    else 
                        state = TURNRIGHT;
                    change_stt = true;
                }
                else if (msg.isBackBlocked){
                    state = STOP;
                    change_stt = true;
                }
            }
            break;

        case STOP:
            if(!msg.isHeadBlocked){
                state = GO_FORWARD;
                change_stt = true;
            }
            else {
                if(!msg.isRotateBlocked){
                    if(msg.LeftPoint <= msg.RightPoint)
                        state = TURNLEFT;
                    else
                        state = TURNRIGHT;
                    change_stt = true;
                }
                else if (!msg.isBackBlocked){ 
                    state = GO_BACK;
                    change_stt = true;
                }
            }
            break;

        default:
            break;
    }
    if(change_stt) {
        msg_out.data = state;
        //ROS_INFO("Topic out: %d",msg_out.data);
        pub.publish(msg_out);
        FSM_DEBUG();
    }
}

int main(int argc, char ** argv){
    ros::init(argc,argv,"HandleFSM_Node");
    ros::NodeHandle nh;
    ros::Subscriber sub = nh.subscribe("/input_FSM",
            1000,callback_receive_data);
    pub = nh.advertise<std_msgs::Int8>("/wheel_control",10);
    ros::spin();
}
