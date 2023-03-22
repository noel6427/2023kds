#ifndef CAN_H
#define CAN_H


#include "ros/ros.h"
#include "std_msgs/String.h"
#include "std_msgs/Int8.h"

#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <fcntl.h>


#define PI 3.14159265

/***************RPM*************/
#define DPS2RPM       0.166666
#define RPM2DSP       6.0000024
/*******************************/

/***************ENO************/
#define DEG2LSP       100
#define ENCODER_RANGE 32767      // 16bit encoder // +-32767 // 0~65535 // 16383
#define DEG2ENC       91.0194   // 65536 / 360(deg)=182.0444 // 16383/360=45.50833
#define ENC2DEG       0.010987  // 360/32767=0.010987 // 360 / 65536=0.005493   // 360/16383=0.002197
#define DEG2RAD       0.017453   // PI / 180
#define RAD2DEG       57.29578   // 180 / PI
/*******************************/


/**************************Motor Command**********************/
#define SPEED_CLOESED_LOOP_CMMD                               0XA2

#define POSITION_CLOSED_LOOP_CMMD                             0xA4

/**************************************************************/


using namespace std;

typedef unsigned char  BYTE;  //8bit
typedef unsigned short BYTE2; //16bit
typedef unsigned int BYTE4;   //32bit


enum Bit_rate {
  _10k = 0,
  _20k,
  _50k,
  _100k,
  _125k,
  _250k,
  _500k,
  _750k,
  _1M
};

class CAN
{
  private:
    int soc;
    int read_can_port;
    string port_name;        //socket CAN port name. such as "can0"
    string device_name;  //Port of USB_to_CAN device port such as "ttyACM0". Can find it in "/dev" directory.

  public :
    CAN(string port_name_, string device_name_);
    ~CAN();

    int16_t Enco_Data_01;
    double Present_Posi = 0;
    double Goal_Posi = 0.0 * DEG2RAD;

    int open_port(const char *port);
    int send_port(struct can_frame *frame);
    int close_port();

    void baud_n_openport(int bit_rate_mode);
    void set_can_frame(struct can_frame &canframe, int CAN_id, u_int8_t CAN_data_len, bool is_ext_mode);
    bool read_CAN(struct can_frame &recv_frame);
    void CAN_write(struct can_frame &frame, BYTE data_array[]);


    void joint_RPM_Ctrl(int32_t rpm);
    void Position_GoV3(int RMD_id, double t , int a);
    void Read_RMD_Data();
    void Set_ZeroPos_1();
};



#endif // CAN_H
