#include "canman/cancan.h"

CAN::CAN(string port_name_, string device_name_) // class? 생성자 함수 객체를 정해줌?
{
  this->port_name = port_name_; //this pointer 객체 자신을 지향하는?
  this->device_name = device_name_;
}

CAN::~CAN(){  // 소멸자 함수
  close_port();
}
int CAN::open_port(const char *port)
{
    struct ifreq ifr;
    struct sockaddr_can addr;
    printf("open_port_start\n");
    /* open socket */
    soc = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if(soc < 0){
        return (-1);
    }

    addr.can_family = AF_CAN;
    strcpy(ifr.ifr_name, port);

    if (ioctl(soc, SIOCGIFINDEX, &ifr) < 0){
        return (-1);
    }

    addr.can_ifindex = ifr.ifr_ifindex;
    fcntl(soc, F_SETFL, O_NONBLOCK);

    if (bind(soc, (struct sockaddr *)&addr, sizeof(addr)) < 0){
        return (-1);
    }

    return 0;
}
int CAN::send_port(struct can_frame *frame) // port에 보내주는거
{
    int retval;
    retval = write(soc, frame, sizeof(struct can_frame));
    if (retval != sizeof(struct can_frame)){
      return (-1);
    }
    else{
      return (0);
    }
}
int CAN::close_port() //캔통신을 끊어준다
{
    close(soc);
    return 0;
}
void CAN::baud_n_openport(int bit_rate_mode){
  /*
    * (can bit-rate)
    * -s0 : 10k(bps)
    * -s1 : 20k
    * -s2 : 50k
    * -s3 : 100k
    * -s4 : 125k
    * -s5 : 250k
    * -s6 : 500k
    * -s7 : 750k
    * -s8 : 1M
    */

  /*
   * Note
   * You have to replace '111111' to your linux Password.
   */

    string command_0 = "echo '111111' | sudo -S slcand -o -c -s0 /dev/"+ this->device_name + " " + this->port_name + " && sudo ifconfig "+this->port_name+" up && sudo ifconfig "+this->port_name+" txqueuelen 1000";
    string command_1 = "echo '111111' | sudo -S slcand -o -c -s1 /dev/"+ this->device_name + " " + this->port_name + " && sudo ifconfig "+this->port_name+" up && sudo ifconfig "+this->port_name+" txqueuelen 1000";//"sudo slcand -o -c -s1 /dev/"+ port_name + " can1 && sudo ifconfig can1 up && sudo ifconfig can1 txqueuelen 1000";
    string command_2 = "echo '111111' | sudo -S slcand -o -c -s2 /dev/"+ this->device_name + " " + this->port_name + " && sudo ifconfig "+this->port_name+" up && sudo ifconfig "+this->port_name+" txqueuelen 1000";
    string command_3 = "echo '111111' | sudo -S slcand -o -c -s3 /dev/"+ this->device_name + " " + this->port_name + " && sudo ifconfig "+this->port_name+" up && sudo ifconfig "+this->port_name+" txqueuelen 1000";
    string command_4 = "echo '111111' | sudo -S slcand -o -c -s4 /dev/"+ this->device_name + " " + this->port_name + " && sudo ifconfig "+this->port_name+" up && sudo ifconfig "+this->port_name+" txqueuelen 1000";
    string command_5 = "echo '111111' | sudo -S slcand -o -c -s5 /dev/"+ this->device_name + " " + this->port_name + " && sudo ifconfig "+this->port_name+" up && sudo ifconfig "+this->port_name+" txqueuelen 1000";
    string command_6 = "echo '111111' | sudo -S slcand -o -c -s6 /dev/"+ this->device_name + " " + this->port_name + " && sudo ifconfig "+this->port_name+" up && sudo ifconfig "+this->port_name+" txqueuelen 1000";
    string command_7 = "echo '111111' | sudo -S slcand -o -c -s7 /dev/"+ this->device_name + " " + this->port_name + " && sudo ifconfig "+this->port_name+" up && sudo ifconfig "+this->port_name+" txqueuelen 1000";
    string command_8 = "echo '111111' | sudo -S slcand -o -c -s8 /dev/"+ this->device_name + " " + this->port_name + " && sudo ifconfig "+this->port_name+" up && sudo ifconfig "+this->port_name+" txqueuelen 1000";

    string command = command_8; //초기화
    string bps_s = "10k"; //초기화

    switch (bit_rate_mode)
    {
    case 0:
      command = command_0;
      bps_s = "10k";
      break;
    case 1:
      command = command_1;
      bps_s = "20k";
      break;
    case 2:
      command = command_2;
      bps_s = "50k";
      break;
    case 3:
    command = command_3;
    bps_s = "100k";
    break;
    case 4:
      command = command_4;
      bps_s = "125k";
      break;
    case 5:
      command = command_5;
      bps_s = "250k";
      break;
    case 6:
      command = command_6;
      bps_s = "500k";
      break;
    case 7:
      command = command_7;
      bps_s = "750k";
      break;
    case 8:
      command = command_8;
      bps_s = "1M";
      break;

    default:
      command = command_5;
      bps_s = "10k";
      break;
    }

    const char *c = command.c_str();
    const char *c2 = bps_s.c_str();

    int i;
    int count = 0;

  /*  만약 코드가 에러가 뜰 경우 다음의 for문을 확인 로스런했을 때 비밀번호 관련 문제 또는 포트 이름이 안맞을 때 이 for문을 주석처리를 해제하면 에러가 안뜸.
      for(i=0;i<5;i++){
      if(system(c) == 0){       //터미널에 명령 전달
        ROS_INFO("Set bit_rate %s",c2); //-s5
        ROS_INFO("%s",c);
        break;
      }
      else{
        count ++;
      }
    }
  */
    if(count>4){
      ROS_ERROR("CAN init Setting Failed!!!");
    }


    const char *port_name_s = this->port_name.c_str();

    for(i=0;i<5;i++){

      if( open_port(port_name_s) == -1 ){            //port_s : can0, can1 etc...
        ROS_WARN("load_Failed");
      }
      else{
        ROS_INFO("succed_to_load_CAN");
        break;
      }
    }
}
void CAN::set_can_frame(struct can_frame &canframe, int CAN_id, u_int8_t CAN_data_len, bool is_ext_mode) //구조체, 이름, 캔아이디 등의 데이터를 보내주는 함수, 데이터가 얼만지 ~ 설정해주고 아이디가 뭔지~
//read에는 안씀!
{
  canframe.can_id = CAN_id;
  canframe.can_dlc = CAN_data_len;
 //printf("[%X]\n", canframe.can_id);

  if(is_ext_mode){
    canframe.can_id |= CAN_EFF_FLAG;    //extended CAN mode FLAG
  }
}
bool CAN::read_CAN(struct can_frame &recv_frame) // 버퍼값을 읽어오기위한 함수, read 함수
{
  struct can_frame frame_rd;
  int result = read(soc, &frame_rd, sizeof(struct can_frame));
  if(result<1){
      //ROS_WARN("CAN_read_failed");
      return false;
  }
  recv_frame = frame_rd;
  return true;
}
void CAN::CAN_write(struct can_frame &frame, BYTE data_array[]){    // 데이터를 보내주는 함수

  memcpy(frame.data, data_array, 8); //copy (data_array)->(frame)

  if(send_port(&frame) == -1) {
    ROS_WARN("CAN_write_fuction error\n");
  }
  /*else {
    printf("succedd_to_CAN_write\n");
  }*/


}
void CAN::joint_RPM_Ctrl(int32_t rpm){
  int32_t speed = rpm  * DEG2LSP;// * RPM2DSP;

  struct can_frame rx_frame1;
  set_can_frame(rx_frame1, 0x141, 8, false);

  BYTE can_arr1[8]={0, };

  can_arr1[0] = SPEED_CLOESED_LOOP_CMMD;  //0xA2
  can_arr1[1] = 0x00;
  can_arr1[2] = 0x00; // 형변환 개념 ~
  can_arr1[3] = 0x00;
  can_arr1[4] = (uint8_t)(speed);
  can_arr1[5] = (uint8_t)(speed>>8);
  can_arr1[6] = (uint8_t)(speed>>16);
  can_arr1[7] = (uint8_t)(speed>>24);

  CAN_write(rx_frame1, can_arr1); // 데이터를 보낸다
}
void CAN::Position_GoV3(int RMD_id, double t , int a /*,int32_t Pos_Deg*/)
{
  if(t >= 1 /*|| abs(a-Enco_Data_01)<=2*/){
    return;
  }
  int32_t Pos_Deg = Enco_Data_01 + (a - Enco_Data_01)*0.5*(1.0-cos(PI*(t)));
  int32_t angleControl = Pos_Deg * DEG2LSP;
  // 0.01degree/LSB ex)36000=360° 1:9 *9

  struct can_frame rx_frame2;
  set_can_frame(rx_frame2, RMD_id, 8, false); //rmd == standard CAN mode FLAG

  uint16_t maxSpeed = 100; //1350; rpm value

  BYTE can_arr2[8]={0, };
  can_arr2[0] = POSITION_CLOSED_LOOP_CMMD; //0xA4
  can_arr2[1] = 0x00;
  can_arr2[2] = (uint8_t)(maxSpeed);
  can_arr2[3] = (uint8_t)(maxSpeed>>8);
  can_arr2[4] = (uint8_t)(angleControl);
  can_arr2[5] = (uint8_t)(angleControl>>8);
  can_arr2[6] = (uint8_t)(angleControl>>16);
  can_arr2[7] = (uint8_t)(angleControl>>24);

  CAN_write(rx_frame2, can_arr2);
}




void CAN::Read_RMD_Data() { //double    버퍼값을 ??
  struct can_frame recv_frame;

  read_CAN(recv_frame);

  //printf("recv_canframe = %x\n",recv_frame.can_id);

  if(recv_frame.can_id == 0x241) {
    Enco_Data_01 = (int16_t)recv_frame.data[6]+((int16_t)recv_frame.data[7]<<8); // 형변환
    //printf("0x141_RMD_Present_Posi_Read   ");
    printf("01_Enco_Data = [%d]\n", Enco_Data_01);

//    printf("\n");
//    printf("can_id     : %x\n",recv_frame.can_id);
//    printf("can_length : %d\n",recv_frame.can_dlc);
//    printf("can_data   :\n");
//    printf("[0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,0x%X]\n",
//           recv_frame.data[0],
//           recv_frame.data[1],
//           recv_frame.data[2],
//           recv_frame.data[3],
//           recv_frame.data[4],
//           recv_frame.data[5],
//           recv_frame.data[6],
//           recv_frame.data[7] );
//    printf("\n");
  }



  //return Enco_Data_02;
}
void CAN::Set_ZeroPos_1()
{
  struct can_frame rx_frame4;
  set_can_frame(rx_frame4, 0x141, 8, false);

  BYTE can_arr4[8]={0, };
  can_arr4[0] = 0x64; //CRRT_MULTI_TURN_POS_OF_ENCODER_TO_ROM_AS_ZERO_CMMD; //
  can_arr4[1] = 0x00;
  can_arr4[2] = 0x00;
  can_arr4[3] = 0x00;
  can_arr4[4] = 0x00;
  can_arr4[5] = 0x00;
  can_arr4[6] = 0x00;
  can_arr4[7] = 0x00;

  CAN_write(rx_frame4, can_arr4);
}
