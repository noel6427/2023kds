#include "ros/ros.h"
#include "std_msgs/String.h"
#include <xenomai/alchemy/task.h>
#include <xenomai/alchemy/timer.h>
#include <iostream>
#include <fstream>
#include <string>

#define TASK_PRIORITY 99 /*The priority of the task can have a value from 1 to 99. In this case, the priority is set to 99.*/
#define TASK_MODE 0 /*The task mode indicates the mode of the task, usually with values like TASK_MODE_NORMAL, TASK_MODE_RT. In this case, the mode is set to 0, which corresponds to TASK_MODE_NORMAL.*/
#define TASK_STACK_SIZE 0 /*Defining the size of stack memory to be used by a task. It is set to 0 because Xenomai automatically determines the stack size.*/

RT_TASK task_1;
RT_TASK task_2;
RT_TASK task_3;
bool task_run = true;

using namespace std;

RTIME task_time[1000][2] = {};

int total_thread = 2;
int max_cnt = 700;
int cnt_1 =0;
int cnt_2 =0;
int cnt_3 =0;

void save();
void pub_thread_1(void *arg);
void pub_thread_2(void *arg);
void pub_thread_3(void *arg);

int main(int argc, char **argv)
{
  ros::init(argc, argv, "xenoxeno_node");
  ros::NodeHandle nh;

  rt_task_create(&task_1, "xeno_task_1", TASK_STACK_SIZE, TASK_PRIORITY, TASK_MODE);
  rt_task_create(&task_2, "xeno_task_2", TASK_STACK_SIZE, TASK_PRIORITY, TASK_MODE);
  rt_task_create(&task_3, "xeno_task_3", TASK_STACK_SIZE, TASK_PRIORITY, TASK_MODE);
  rt_task_start(&task_1, &pub_thread_1, NULL);
  rt_task_start(&task_2, &pub_thread_2, NULL);
  rt_task_start(&task_3, &pub_thread_3, NULL);

  while (ros::ok())
    {
    ros::spinOnce();
    }

    save();

//  Wait for each task to finish
//  rt_task_join(&task_1);
//  rt_task_join(&task_2);
//  rt_task_join(&task_3);

//  rt_task_delete(&task_1);
//  rt_task_delete(&task_2);
//  rt_task_delete(&task_3);



  return 0;
}

void pub_thread_1(void *arg){

  ros::NodeHandle nh;
  rt_task_set_periodic(NULL, TM_NOW, 10000000); // 10 ms
  ros::Publisher chatter_pub_1 = nh.advertise<std_msgs::String>("chatter_1", 10);

  while (task_run)
  {
    std_msgs::String msg;
    msg.data = "hello xenomai";

    task_time[cnt_1][0] = rt_timer_read();

    ROS_INFO("I heard: [%s] [%lld]", msg.data.c_str(),task_time[cnt_1][0]/1000000);

    chatter_pub_1.publish(msg);
    cnt_1 ++;

    rt_task_wait_period(NULL);
  }
}

void pub_thread_2(void *arg){

  ros::NodeHandle nh;
  rt_task_set_periodic(NULL, TM_NOW, 10000000); // 10 ms
  ros::Publisher chatter_pub_2 = nh.advertise<std_msgs::String>("chatter_2", 10);

  while (task_run)
  {
    std_msgs::String msg;
    msg.data = "hello xenomai";

    task_time[cnt_2][1] = rt_timer_read();

    ROS_INFO("I heard: [%s] [%lld]", msg.data.c_str(),task_time[cnt_2][1]/1000000);

    chatter_pub_2.publish(msg);

    cnt_2 ++;

    rt_task_wait_period(NULL);
  }
}

void pub_thread_3(void *arg){

  ros::NodeHandle nh;
  rt_task_set_periodic(NULL, TM_NOW, 10000000); // 10 ms
  ros::Publisher chatter_pub_3 = nh.advertise<std_msgs::String>("chatter_3", 10);

  while (task_run)
  {
    std_msgs::String msg;
    msg.data = "hello xenomai";

    task_time[cnt_3][2] = rt_timer_read();

    ROS_INFO("I heard: [%s] [%lld]", msg.data.c_str(),task_time[cnt_3][2]/1000000);

    chatter_pub_3.publish(msg);
    ROS_INFO("%d",cnt_3);
    if (cnt_3 == max_cnt) {
      task_run = false;
      ROS_INFO("======================================================");
      ROS_INFO("======================================================");
    }
    cnt_3 ++;
    rt_task_wait_period(NULL);
  }
}

// Check if the time interval is close to the set value, make time.txt on catkin_ws
void save()
{
    ofstream file;
    file.open("/home/nuc2/catkin_ws/test_save/time.txt",ios_base::app);
     for(int i=2;i<=max_cnt;i++)
        {
           for (int j = 0;j<=total_thread;j++)
          {￣￣
             ROS_INFO("Saving_%d,%d",i,j);
            file <<(task_time[i][j] - task_time[i-1][j])/1000000.0<<", ";
          }
          file << "\n";
        }
        cout<<"save success"<<endl;

    file.close();
}
