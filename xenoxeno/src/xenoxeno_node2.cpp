#include "ros/ros.h"
#include "std_msgs/String.h"
#include "sensor_msgs/Joy.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

//======================================Save=========================================
#include <fstream>
#include <string>

//======================================Xenomai======================================
#include <xenomai/alchemy/task.h>
#include <xenomai/alchemy/timer.h>

//======================================Dinamixel====================================
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <math.h>
#include "dynamixel_sdk/dynamixel_sdk.h"
#define ADDR_PRO_TORQUE_ENABLE          64                  // Control table address is different in Dynamixel model
#define ADDR_PRO_GOAL_POSITION          116
#define ADDR_PRO_PRESENT_POSITION       132
#define PROTOCOL_VERSION                2.0                 // See which protocol version is used in the Dynamixel
#define DXL_ID                          15                  // Dynamixel ID: 1
#define BAUDRATE                        1000000
#define DEVICENAME                      "/dev/ttyUSB0"
#define TORQUE_ENABLE                   1                   // Value for enabling the torque
#define TORQUE_DISABLE                  0                   // Value for disabling the torque
#define DXL_MINIMUM_POSITION_VALUE      0                   // Dynamixel will rotate between this value
#define DXL_MAXIMUM_POSITION_VALUE      4096                // and this value (note that the Dynamixel would not move when the position value is out of movable range. Check e-manual about the range of the Dynamixel you use.)
#define DXL_MOVING_STATUS_THRESHOLD     20                  // Dynamixel moving status threshold

//======================================Xenomai======================================
#define TASK_PRIORITY 99 /*The priority of the task can have a value from 1 to 99. In this case, the priority is set to 99.*/
#define TASK_MODE 0 /*The task mode indicates the mode of the task, usually with values like TASK_MODE_NORMAL, TASK_MODE_RT. In this case, the mode is set to 0, which corresponds to TASK_MODE_NORMAL.*/
#define TASK_STACK_SIZE 0 /*Defining the size of stack memory to be used by a task. It is set to 0 because Xenomai automatically determines the stack size.*/

//=================================================================================================================
dynamixel::PortHandler *portHandler = dynamixel::PortHandler::getPortHandler(DEVICENAME);
dynamixel::PacketHandler *packetHandler = dynamixel::PacketHandler::getPacketHandler(PROTOCOL_VERSION);

int dxl_comm_result = COMM_TX_FAIL;             // Communication result
uint8_t dxl_error = 0;                          // Dynamixel error
int32_t dxl_present_position = 0;               // Present position


//=================================================================================================================
RT_TASK task_1;

RTIME task_time[1000][2] = {};
int total_thread = 2;
double max_cnt_1= 2000;
bool task_run = true;
int goal_pos = 0;
double cnt_1 =0;

//======================================function======================================
void save();
void Dxl_control(void *arg);
void Load();
void init_Dxl();
int cos_wave(int present_pos,int end_pos);
void Run_Dxl(int finish_pos);

using namespace std;

sensor_msgs::Joy joy_data;


void joyCallback(const sensor_msgs::Joy::ConstPtr& msg)
{
  ROS_INFO("connected");
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "xenoxeno_node");

  portHandler->openPort();
  portHandler->setBaudRate(BAUDRATE);
  dxl_comm_result = packetHandler->write1ByteTxRx(portHandler, DXL_ID, ADDR_PRO_TORQUE_ENABLE, TORQUE_ENABLE, &dxl_error);
  packetHandler->getTxRxResult(dxl_comm_result); //Torque on

  rt_task_create(&task_1, "xeno_task_1", TASK_STACK_SIZE, TASK_PRIORITY, TASK_MODE);

  Load();
  rt_task_start(&task_1, &Dxl_control, NULL);

  while (ros::ok())
    {
    ros::spinOnce();
    }

//    save();

    dxl_comm_result = packetHandler->write1ByteTxRx(portHandler, DXL_ID, ADDR_PRO_TORQUE_ENABLE, TORQUE_DISABLE, &dxl_error);
    packetHandler->getTxRxResult(dxl_comm_result);

    // Close port
    portHandler->closePort();

  return 0;
}


void Dxl_control(void *arg){

  ros::NodeHandle nh;
  rt_task_set_periodic(NULL, TM_NOW, 10000000); // 10 ms
  ros::Publisher chatter_pub_1 = nh.advertise<std_msgs::String>("chatter_1", 10);
  ros::Subscriber sub = nh.subscribe("joy", 1000, joyCallback);
  while (task_run)
  {
    Run_Dxl(3000);
//    Run_Dxl_by_Joy();


//  if (cnt_1 == 5000) {
//    task_run = false;
//    ROS_INFO("======================================================");
//    ROS_INFO("======================================================");
//  }
//  task_time[cnt_1][0] = rt_timer_read();

//  cnt_1 ++;

  rt_task_wait_period(NULL);
}

}

//void save()
//{
//    ofstream file;
//    file.open("/home/nuc2/catkin_ws/test_save/time.txt",ios_base::app);
//     for(int i=2;i<=max_cnt_1_1;i++)
//        {
//           for (int j = 0;j<=total_thread;j++)
//          {
//             ROS_INFO("Saving_%d,%d",i,j);
//            file <<(task_time[i][j] - task_time[i-1][j])/1000000.0<<", ";
//          }
//          file << "\n";
//        }
//        cout<<"save success"<<endl;

//    file.close();
//}

void Run_Dxl(int finish_pos)
{

    dxl_comm_result = packetHandler->read4ByteTxRx(portHandler, DXL_ID, ADDR_PRO_PRESENT_POSITION, (uint32_t*)&dxl_present_position, &dxl_error); // 현재 위치가 저장된 주소값을 알려주는 부분
    packetHandler->getTxRxResult(dxl_comm_result); //현재 위치를 저장하는 부분

    printf("present_pos = %d\n",dxl_present_position);

    goal_pos = cos_wave(dxl_present_position,finish_pos);

    dxl_comm_result = packetHandler->write4ByteTxRx(portHandler, DXL_ID, ADDR_PRO_GOAL_POSITION, goal_pos, &dxl_error); // 목표값 지정
    packetHandler->getTxRxResult(dxl_comm_result); // 목표값으로 이동 시키는 부분

    cnt_1++;
    if (abs(dxl_present_position - goal_pos) < 5){
      return;
    }

}

void init_Dxl()
{
  int init_pos = 0;

while(1){
  printf("loading...\n present_pos = %d\n",dxl_present_position);
    dxl_comm_result = packetHandler->read4ByteTxRx(portHandler, DXL_ID, ADDR_PRO_PRESENT_POSITION, (uint32_t*)&dxl_present_position, &dxl_error); // 현재 위치가 저장된 주소값을 알려주는 부분
    packetHandler->getTxRxResult(dxl_comm_result);

    goal_pos = cos_wave(dxl_present_position,init_pos);

    dxl_comm_result = packetHandler->write4ByteTxRx(portHandler, DXL_ID, ADDR_PRO_GOAL_POSITION, goal_pos, &dxl_error); // 목표값 지정
    packetHandler->getTxRxResult(dxl_comm_result);

    cnt_1 ++;
    if(abs(dxl_present_position - init_pos) < 15){
      printf("loading complete! now pos : %d\n",dxl_present_position);
      break;
    }

}
      cnt_1 = 0;
}

int cos_wave(int present_pos,int end_pos){

  int goal=((end_pos - present_pos)/2)*(1-cos(3.14*(cnt_1/max_cnt_1)))+present_pos;
  return goal;
}

void Load(){
  init_Dxl();
}

//void Run_Dxl_by_Joy(const sensor_msgs::Joy::ConstPtr& msg){
//  int finish_by_joy = 0;
//  Run_Dxl(finish_by_joy);
//}

