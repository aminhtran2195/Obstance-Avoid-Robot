
import rospy
from std_msgs.msg import String

def callback_receive_data(msg):
    rospy.loginfo("Message receive: ")
    rospy.loginfo(msg)

if __name__ == '__main__':
    rospy.init_node('Handle_LidarSensor')

    sub = rospy.Subscriber("/scan",String,callback_receive_data)

    rospy.spin()