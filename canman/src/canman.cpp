#include "ros/ros.h"
#include "std_msgs/String.h"
#include <unistd.h>
#include "canman/cancan.h"
#include "std_msgs/Int32.h"
#include "std_msgs/Float32.h"

double rmd_Present_Posi_01 = 0;

CAN _can("can0", "ttyACM0");

int main(int argc, char **argv)
{
  ros::init(argc, argv, "canman");
  ros::NodeHandle nh;

  ros::Publisher chatter_pub = nh.advertise<std_msgs::Float32>("chatter", 1000);

  _can.baud_n_openport(_1M);

  struct can_frame rx_frame;

  _can.joint_RPM_Ctrl(0);
  usleep(2000);

  _can.Set_ZeroPos_1();

  _can.Read_RMD_Data();
  ros::Rate loop_rate(100);
  int a=0;
  double t=0;
  double T =3;
  double dt = 0.01;

  printf("원하는 위치를 입력해주세요 :");
  scanf("%d",&a);
  printf("\n");

  while (ros::ok())
  {
      _can.Position_GoV3(0x141,t/T,a); // Runnig
      _can.Read_RMD_Data();  // encoder /present pos

     t =t+dt;

     std_msgs::Float32 msg;
     msg.data = a;
     chatter_pub.publish(msg);
     ros::spinOnce();
     loop_rate.sleep();
  }

  _can.close_port();

  return 0;
}


