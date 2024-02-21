#!/usr/bin/env python3

import rospy
from sensor_msgs.msg import LaserScan

# # ranges: 0 -> 89 trai phia truoc
# # ranges: 90 -> 1

# # dinh dang msg.ranges hien tai float32[] --- https://docs.ros.org/en/noetic/api/sensor_msgs/html/msg/LaserScan.html
def Handle_Data(data):
    LP = RP = 0
    c1 = c2 = f = 0
    for i in range(0,58):
        if data[i] < 11: 
            c1 = 1
            LP = LP + 1
          
    
    

def callback_receive_data(msg):
    rospy.loginfo("Message receive: ")
    rospy.loginfo(msg.ranges)


if __name__ == '__main__':
    rospy.init_node('Handle_LidarSensor')

    sub = rospy.Subscriber("/scan",LaserScan,callback_receive_data)

    rospy.spin()