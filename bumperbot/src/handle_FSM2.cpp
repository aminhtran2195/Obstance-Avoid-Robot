#include <ros/ros.h>
#include <my_robot_msgs/BumperbotFSM_msgs.h>
#include <std_msgs/Int8.h>
#include <std_msgs/Float32.h>

#define STOP 0
#define GO_FORWARD 1
#define TURNLEFT 2
#define TURNRIGHT 3
#define GO_BACK 4

float obs_distance;
int8_t direction;

int8_t state = STOP;
ros::Publisher pub;
ros::Publisher pub2;
my_robot_msgs::BumperbotFSM_msgs data_msg;

std_msgs::Int8 msg_out;

bool change_stt = false;
void FSM_DEBUG();
void blocked_FSM(my_robot_msgs::BumperbotFSM_msgs msg){
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

void callback_receive_Direct(const std_msgs::Int8 &msg){
    direction = msg.data;
}
void callback_receive_OBSDistance(const std_msgs::Float32 &msg){
    obs_distance = msg.data;
}
void callback_receive_data(const my_robot_msgs::BumperbotFSM_msgs &msg){
    data_msg = msg;
}

int main(int argc, char ** argv){
    ros::init(argc,argv,"HandleFSM_Node");
    ros::NodeHandle nh;
    ros::Subscriber sub = nh.subscribe("/direction",
            10,callback_receive_Direct);
    ros::Subscriber sub2 = nh.subscribe("/obs_distance",
            10,callback_receive_OBSDistance);
    ros::Subscriber sub3 = nh.subscribe("/input_FSM",
            10,callback_receive_data);
    pub = nh.advertise<std_msgs::Int8>("/wheel_control",10);
    pub2 = nh.advertise<std_msgs::Int8>("/check",10);
    while(ros::ok()){
        ros::spinOnce();
        if(data_msg.isHeadBlocked || data_msg.isRotateBlocked){
            blocked_FSM(data_msg);
        }
        else{
            switch (state)
            {
            case GO_FORWARD:
                if(obs_distance != 999){
                        ROS_INFO("gap vat can\n");
                        state = STOP;
                        msg_out.data = 0;
                        pub.publish(msg_out);
                        msg_out.data = 1;
                        pub2.publish(msg_out);
                        direction = 2;
                }
                break;
            case TURNLEFT:
                if(obs_distance == 999) {
                    ROS_INFO("Di thang\n");
                    state = GO_FORWARD;
                    msg_out.data = 1;
                    pub.publish(msg_out);
                }
                break;
            case TURNRIGHT:
                if(obs_distance == 999) {
                    ROS_INFO("Di thang\n");
                    state = GO_FORWARD;
                    msg_out.data = 1;
                    pub.publish(msg_out);
                }
                break;
            case STOP:
                if(obs_distance == 999) {
                    ROS_INFO("Di thang\n");
                    state = GO_FORWARD;
                    msg_out.data = 1;
                    pub.publish(msg_out);
                }
                else{
                    if(direction == 0 ) {
                        ROS_INFO("Re phai\n");
                        state = TURNRIGHT;
                        msg_out.data = 3;
                        pub.publish(msg_out);
                        direction == 2; 
                    }
                    else if (direction == 1){
                        ROS_INFO("Re trai\n");
                        state = 2;
                        msg_out.data = state;
                        pub.publish(msg_out);
                        direction == 2; 
                    }
                }  
                break;
            default:
                break;
            }
        }
        
    }
  
}