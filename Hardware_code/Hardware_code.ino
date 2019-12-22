#include <SoftwareSerial.h>
#include "edp.h"
#define KEY  "mAyh0HZIlbQlaNegMWyqgbc7KVc="       //APIkey 
#define ID   "575232844"                          //设备ID
#define _baudrate   115200
#define WIFI_UART   dbgSerial
#define DBG_UART    Serial   //调试打印串口
#define lux_1       400     //设置光照下限
#define lux_2       800     //设置光照上限
#define angle_1     3       //设置光照高于上限时的舵机角度
#define angle_2     90      //设置光照处于中间值时舵机的角度
#define angle_3     161     //设置光照低于下限时舵机的角度
#define OPEN        90      //设置窗户打开的角度
#define CLOSE       45      
SoftwareSerial dbgSerial(2 , 3 ); // 软串口，调试打印
edp_pkt *pkt;
#include <Microduino_SHT2x.h>
#define  sensorPin_1  A0
#define INTERVAL_sensor 2000
unsigned long sensorlastTime = millis();
float sensor_lux, presensor_lux;                 //传感器光照
int servopinL = 9;               //定义控制窗帘的舵机L在D9接口
int servopinT = 8;               //定义控制窗户的舵机T在D8接口
/*
  doCmdOk
  发送命令至模块，从回复中获取期待的关键字
  keyword: 所期待的关键字
  成功找到关键字返回true，否则返回false
*/
bool doCmdOk(String data, char *keyword)
{
  bool result = false;
  if (data != "")   //对于tcp连接命令，直接等待第二次回复
  {
    WIFI_UART.println(data);  //发送AT指令
    DBG_UART.print("SEND: ");
    DBG_UART.println(data);
  }
  if (data == "AT")   //检查模块存在
    delay(2000);
  else
    while (!WIFI_UART.available());  // 等待模块回复

  delay(200);
  if (WIFI_UART.find(keyword))   //返回值判断
  {
    DBG_UART.println("do cmd OK");
    result = true;
  }
  else
  {
    DBG_UART.println("do cmd ERROR");
    result = false;
  }
  while (WIFI_UART.available()) WIFI_UART.read();   //清空串口接收缓存
  delay(500); //指令时间间隔
  return result;
}


void setup()
{
  DBG_UART.begin(115200);
  WIFI_UART.begin( 9600 );
  pinMode(servopinL, OUTPUT);
  pinMode(servopinT, OUTPUT);
  pinMode(sensorPin_1, INPUT);
  sensor_lux = analogRead(A0);
  presensor_lux = analogRead(A0);
  WIFI_UART.setTimeout(3000);    //设置find超时时间
  delay(3000);
  while (!doCmdOk("AT+CWMODE=3", "OK"));            //工作模式
  while (!doCmdOk("AT+CWJAP=\"iPhone7\",\"YjY20010806\"", "OK"));
  while (!doCmdOk("AT+CIPSTART=\"TCP\",\"183.230.40.39\",876", "CONNECT"));
  while (!doCmdOk("AT+CIPMODE=1", "OK"));           //透传模式
  while (!doCmdOk("AT+CIPSEND", ">"));              //开始发送*/
}
int sum = 0;      //设置中断判别开关1
int ban = 0;      //设置中断判别开关2
int n = 0;        //用来读取要设定的时间
int last;         //记录已经过去的秒数
void loop()
{
  static int edp_connect = 0;         //EDP连接的判断依据，1表示连接，0反之
  bool trigger = false;
  edp_pkt rcv_pkt;
  unsigned char pkt_type;
  int i, tmp;
  char num[10];

  /* EDP 连接 */
  //调试信息
  char edp_command[50];
  char edp_cmd_id[40];
  long id_len, cmd_len, rm_len;
  char datastr[20];
  char val[10];
  static char order = '0';
  if (sum == 0) {   //后续程序中若没有中断则执行以下程序，防止重复获得串口信息
    if (edp_connect == 0)
    {
      while (WIFI_UART.available()) WIFI_UART.read(); //清空串口接收缓存
      packetSend(packetConnect(ID, KEY));             //发送EPD连接包
      while (!WIFI_UART.available());                 //等待EDP连接应答
      if ((tmp = WIFI_UART.readBytes(rcv_pkt.data, sizeof(rcv_pkt.data))) > 0 )
      {
        rcvDebug(rcv_pkt.data, tmp);
        if (rcv_pkt.data[0] == 0x20 && rcv_pkt.data[2] == 0x00 && rcv_pkt.data[3] == 0x00)
        {
          edp_connect = 1;
          DBG_UART.println("EDP connected.");
        }
        else
        {
          edp_connect = 0;
          DBG_UART.println("EDP connect error.");
        }
      }
      packetClear(&rcv_pkt);
    }
    DBG_UART.print("edp:"); DBG_UART.println(edp_connect);
    DBG_UART.println(sum);DBG_UART.println(order);
    while (WIFI_UART.available())
    {
      readEdpPkt(&rcv_pkt);
      if (isEdpPkt(&rcv_pkt))
      {
        pkt_type = rcv_pkt.data[0];
        switch (pkt_type)
        {
          case CMDREQ:
            memset(edp_command, 0, sizeof(edp_command));
            memset(edp_cmd_id, 0, sizeof(edp_cmd_id));
            edpCommandReqParse(&rcv_pkt, edp_cmd_id, edp_command, &rm_len, &id_len, &cmd_len);
            DBG_UART.print("rm_len: ");
            DBG_UART.println(rm_len, DEC);
            delay(10);
            DBG_UART.print("id_len: ");
            DBG_UART.println(id_len, DEC);
            delay(10);
            DBG_UART.print("cmd_len: ");
            DBG_UART.println(cmd_len, DEC);
            delay(10);
            DBG_UART.print("id: ");
            DBG_UART.println(edp_cmd_id);
            delay(10);
            DBG_UART.print("cmd: ");
            DBG_UART.println(edp_command);
            order = edp_command[0];        //读取EDP命令字符串的第一位作为命令标识
            //数据处理与应用中EDP命令内容对应
            //本例中格式为  datastream:[1/0]
            sscanf(edp_command, "%[^:]:%s", datastr, val);


            packetSend(packetDataSaveTrans(NULL, datastr, val)); //将新数据值上传至数据流
            break;
          default:
            DBG_UART.print("unknown type: ");
            DBG_UART.println(pkt_type, HEX);
            edp_connect = 0;                 //保持EDP连接
            break;
        }
      }
      //delay(4);
    }
  }
  if (rcv_pkt.len > 0)
    packetClear(&rcv_pkt);
  delay(150);
  
  if (order == '3') {      //设置为可定时的程序
    DBG_UART.println("转换为设置模式 ");
    n=0;
    for (int i = 2; i < (int)(edp_command[1] - '0') + 2; i++) {
      n *= 10;
      n = n + (int)(edp_command[i] - '0');
    }      //从串口信息中读取设置的时间（以秒为单位）
    Serial.println(n);
    if (sum == 1)sum = 0;
    servoLCtrl(1000,presensor_lux);
    ban = 1;         //记录程序运行是否被中断，若被中断则不会将ban置为“0”
    for (int i = 0; i < n; i++) {
      Serial.println(i);
      last=i+1;
      if(i==n-1)sum=1;
      if ( edp_connect == 0)
      {
        while (WIFI_UART.available()) WIFI_UART.read(); //清空串口接收缓存
        packetSend(packetConnect(ID, KEY));             //发送EPD连接包
        while (!WIFI_UART.available());                 //等待EDP连接应答
        if ((tmp = WIFI_UART.readBytes(rcv_pkt.data, sizeof(rcv_pkt.data))) > 0 )
        {
          rcvDebug(rcv_pkt.data, tmp);
          if (rcv_pkt.data[0] == 0x20 && rcv_pkt.data[2] == 0x00 && rcv_pkt.data[3] == 0x00)
          {
            edp_connect = 1;
            DBG_UART.println("EDP connected.");
          }
          else
            DBG_UART.println("EDP connect error.");
        }
        packetClear(&rcv_pkt);
      }
      if (WIFI_UART.available()) {      //计时中收到指令，执行中断
        sum=1;        
        readEdpPkt(&rcv_pkt);
        if (isEdpPkt(&rcv_pkt))
        {
          pkt_type = rcv_pkt.data[0];
          switch (pkt_type)
          {
            case CMDREQ:
              memset(edp_command, 0, sizeof(edp_command));
              memset(edp_cmd_id, 0, sizeof(edp_cmd_id));
              edpCommandReqParse(&rcv_pkt, edp_cmd_id, edp_command, &rm_len, &id_len, &cmd_len);
              DBG_UART.print("rm_len: ");
              DBG_UART.println(rm_len, DEC);
              delay(10);
              DBG_UART.print("id_len: ");
              DBG_UART.println(id_len, DEC);
              delay(10);
              DBG_UART.print("cmd_len: ");
              DBG_UART.println(cmd_len, DEC);
              delay(10);
              DBG_UART.print("id: ");
              DBG_UART.println(edp_cmd_id);
              delay(10);
              DBG_UART.print("cmd: ");
              DBG_UART.println(edp_command);
              order = edp_command[0];
              //数据处理与应用中EDP命令内容对应
              //本例中格式为  datastream:[1/0]
              sscanf(edp_command, "%[^:]:%s", datastr, val);


              packetSend(packetDataSaveTrans(NULL, datastr, val)); //将新数据值上传至数据流
              break;
            default:
              DBG_UART.print("unknown type: ");
              DBG_UART.println(pkt_type, HEX);
              edp_connect = 0;     //保持EDP连接
              sum=6;               //进入长时间计时函数
              break;
          }
        }
  //将中断判别开关1置为“1”表示程序中断
             break; 
      }
      if (sum == 1){break;}
      delay(1000);
    }
    Serial.println("定时结束");
    if (sum == 1) {      //程序未被中断则执行
  Serial.println("sum:1");
      ban = 0;
      servoLCtrlSetting_1();
      delay(5000);
      delay(5000);
      sensor_lux = analogRead(A0);
      servoLCtrlSetting_2(sensor_lux);
      presensor_lux = sensor_lux;
      order = '0';      //将指令变更为“自动化”
    }
  }
  else if (order == '2')      //将窗户关闭 
  {     
    DBG_UART.println("执行mode_2关");
    servoTCtrlMode_2();
    order = '0';
  }
  else if (order == '1')      //将窗户打开
  {
    DBG_UART.println("执行mode_1开");
    servoTCtrlMode_1();
    order = '0';
  }
  else if (order == '0')      //设置为通过室外光照自动控制窗帘开合
  {
    if (sum != 0)sum = 0;
    DBG_UART.println("自动化");
    DBG_UART.println(sum);
    if (ban == 0) {
      sensor_lux = analogRead(A0);
      DBG_UART.print("lux"); DBG_UART.println(sensor_lux);
      servoLCtrl(sensor_lux, presensor_lux);
      presensor_lux = sensor_lux;
    }
    else if (ban == 1) {
      Serial.println("唤醒");
      sensor_lux = analogRead(A0);
      DBG_UART.print("lux"); DBG_UART.println(sensor_lux);
      servoLCtrl(sensor_lux, 1000);
      presensor_lux = sensor_lux;
      ban = 0;
    }
    //对定时模式下是否被中断进行分开处理，使舵机转动更加自然
    for (int i = 0; i < 5; i++) {      //使程序每五秒调节一次窗帘
      if ( edp_connect == 0)
      {
        while (WIFI_UART.available()) WIFI_UART.read(); //清空串口接收缓存
        packetSend(packetConnect(ID, KEY));             //发送EPD连接包
        while (!WIFI_UART.available());                 //等待EDP连接应答
        if ((tmp = WIFI_UART.readBytes(rcv_pkt.data, sizeof(rcv_pkt.data))) > 0 )
        {
          rcvDebug(rcv_pkt.data, tmp);
          if (rcv_pkt.data[0] == 0x20 && rcv_pkt.data[2] == 0x00 && rcv_pkt.data[3] == 0x00)
          {
            edp_connect = 1;
            DBG_UART.println("EDP connected.");
          }
          else
            DBG_UART.println("EDP connect error.");
        }
        packetClear(&rcv_pkt);
      }
      if (WIFI_UART.available()) {
        readEdpPkt(&rcv_pkt);
        if (isEdpPkt(&rcv_pkt))
        {
          pkt_type = rcv_pkt.data[0];
          switch (pkt_type)
          {
            case CMDREQ:
              memset(edp_command, 0, sizeof(edp_command));
              memset(edp_cmd_id, 0, sizeof(edp_cmd_id));
              edpCommandReqParse(&rcv_pkt, edp_cmd_id, edp_command, &rm_len, &id_len, &cmd_len);
              DBG_UART.print("rm_len: ");
              DBG_UART.println(rm_len, DEC);
              delay(10);
              DBG_UART.print("id_len: ");
              DBG_UART.println(id_len, DEC);
              delay(10);
              DBG_UART.print("cmd_len: ");
              DBG_UART.println(cmd_len, DEC);
              delay(10);
              DBG_UART.print("id: ");
              DBG_UART.println(edp_cmd_id);
              delay(10);
              DBG_UART.print("cmd: ");
              DBG_UART.println(edp_command);
              order = edp_command[0];
              //数据处理与应用中EDP命令内容对应
              //本例中格式为  datastream:[1/0]
              sscanf(edp_command, "%[^:]:%s", datastr, val);


              packetSend(packetDataSaveTrans(NULL, datastr, val)); //将新数据值上传至数据流
              break;
            default:
              DBG_UART.print("unknown type: ");
              DBG_UART.println(pkt_type, HEX);
              edp_connect = 0;                    //保持EDP连接
              break;
          }
        }
        sum = 1;
      }
      if (sum == 0)delay(1000);
      else break;
    }
  }
  if (sum == 6) {             //如果设置了较长时间
    Serial.println("真的结束了?");
    order='6';               //不存在的order
     for (int i = last+5; i < n; i++) {
      Serial.println(i);
      last=i+1;
      if(i==n-1){last=0;sum=5;}
      if ( edp_connect == 0)
      {
        while (WIFI_UART.available()) WIFI_UART.read(); //清空串口接收缓存
        packetSend(packetConnect(ID, KEY));             //发送EPD连接包
        while (!WIFI_UART.available());                 //等待EDP连接应答
        if ((tmp = WIFI_UART.readBytes(rcv_pkt.data, sizeof(rcv_pkt.data))) > 0 )
        {
          rcvDebug(rcv_pkt.data, tmp);
          if (rcv_pkt.data[0] == 0x20 && rcv_pkt.data[2] == 0x00 && rcv_pkt.data[3] == 0x00)
          {
            edp_connect = 1;
            DBG_UART.println("EDP connected.");
          }
          else
            DBG_UART.println("EDP connect error.");
        }
        packetClear(&rcv_pkt);
      }
      if (WIFI_UART.available()) {      //计时中收到指令，执行中断
        sum=1;        
        readEdpPkt(&rcv_pkt);
        if (isEdpPkt(&rcv_pkt))
        {
          pkt_type = rcv_pkt.data[0];
          switch (pkt_type)
          {
            case CMDREQ:
              memset(edp_command, 0, sizeof(edp_command));
              memset(edp_cmd_id, 0, sizeof(edp_cmd_id));
              edpCommandReqParse(&rcv_pkt, edp_cmd_id, edp_command, &rm_len, &id_len, &cmd_len);
              DBG_UART.print("rm_len: ");
              DBG_UART.println(rm_len, DEC);
              delay(10);
              DBG_UART.print("id_len: ");
              DBG_UART.println(id_len, DEC);
              delay(10);
              DBG_UART.print("cmd_len: ");
              DBG_UART.println(cmd_len, DEC);
              delay(10);
              DBG_UART.print("id: ");
              DBG_UART.println(edp_cmd_id);
              delay(10);
              DBG_UART.print("cmd: ");
              DBG_UART.println(edp_command);
              order = edp_command[0];
              //数据处理与应用中EDP命令内容对应
              //本例中格式为  datastream:[1/0]
              sscanf(edp_command, "%[^:]:%s", datastr, val);


              packetSend(packetDataSaveTrans(NULL, datastr, val)); //将新数据值上传至数据流
              break;
            default:
              DBG_UART.print("unknown type: ");
              DBG_UART.println(pkt_type, HEX);
              edp_connect = 0;                //保持EDP连接
              sum=6;
              break;
          }
        }
                                                    //将中断判别开关1置为“1”表示程序中断
             break;
      }
      if (sum == 1){break;}
      delay(1000);
    }
    }
if (sum == 5) {      //较长时间设定的程序未被中断则执行
  Serial.println("sum:5");
      ban = 0;
      servoLCtrlSetting_1();
      delay(5000);
      delay(5000);
      sensor_lux = analogRead(A0);
      servoLCtrlSetting_2(sensor_lux);
      presensor_lux = sensor_lux;
      order = '0';      //将指令变更为“自动化”
    }
}

/*
  readEdpPkt
  从串口缓存中读数据到接收缓存
*/
bool readEdpPkt(edp_pkt *p)
{
  int tmp;
  if ((tmp = WIFI_UART.readBytes(p->data + p->len, sizeof(p->data))) > 0 )
  {
    rcvDebug(p->data + p->len, tmp);
    p->len += tmp;
  }
  return true;
}

/*
  packetSend
  将待发数据发送至串口，并释放到动态分配的内存
*/
void packetSend(edp_pkt* pkt)
{
  if (pkt != NULL)
  {
    WIFI_UART.write(pkt->data, pkt->len);    //串口发送
    WIFI_UART.flush();
    free(pkt);              //回收内存
  }
}

void rcvDebug(unsigned char *rcv, int len)
{
  int i;

  DBG_UART.print("rcv len: ");
  DBG_UART.println(len, DEC);
  for (i = 0; i < len; i++)
  {
    DBG_UART.print(rcv[i], HEX);
    DBG_UART.print(" ");
  }
  DBG_UART.println("");
}

void servoLpulse(int angle)//定义一个脉冲函数
{
  int pulsewidth = (angle * 11) + 500; //将角度转化为500-2480的脉宽值
  digitalWrite(servopinL, HIGH);   //将舵机接口电平至高
  delayMicroseconds(pulsewidth);  //延时脉宽值的微秒数
  digitalWrite(servopinL, LOW);    //将舵机接口电平至低
  delayMicroseconds(20000 - pulsewidth);
}
void servoTpulse(int angle)//定义一个脉冲函数
{
  int pulsewidth = (angle * 11) + 500; //将角度转化为500-2480的脉宽值
  digitalWrite(servopinT, HIGH);   //将舵机接口电平至高
  delayMicroseconds(pulsewidth);  //延时脉宽值的微秒数
  digitalWrite(servopinT, LOW);    //将舵机接口电平至低
  delayMicroseconds(20000 - pulsewidth);
}
void servoLCtrl(float sl, float psl) {
  if (sl < lux_1) {
    if (psl >= lux_1 && psl <= lux_2) {
      for (int angle = angle_2; angle <= angle_3; angle++) {
        for (int n = 0; n < 3; n++) {
          servoLpulse(angle);
        }
      }
    }
    else if (psl > lux_2) {
      for (int angle = angle_1; angle <= angle_3; angle++) {
        for (int n = 0; n < 3; n++) {
          servoLpulse(angle);
        }
      }
    }
  }
  else if (sl >= lux_1 && sl <= lux_2) {
    if (psl < lux_1) {
      for (int angle = angle_3; angle >= angle_2; angle--) {
        for (int n = 0; n < 3; n++) {
          servoLpulse(angle);
        }
      }
    }
    else if (psl > lux_2) {
      for (int angle = angle_1; angle <= angle_2; angle++) {
        for (int n = 0; n < 3; n++) {
          servoLpulse(angle);
        }
      }
    }
  }
  else if (sl > lux_2) {
    if (psl < lux_1) {
      for (int angle = angle_3; angle >= angle_1; angle--) {
        for (int n = 0; n < 3; n++) {
          servoLpulse(angle);
        }
      }
    }
    else if (psl >= lux_1 && psl <= lux_2) {
      for (int angle = angle_2; angle >= angle_1; angle--) {
        for (int n = 0; n < 3; n++) {
          servoLpulse(angle);
        }
      }
    }
  }
}             
//通过读取光照控制窗帘开合的函数

void servoLCtrlSetting_1() {
  for (int angle = angle_1; angle <= angle_3; angle++) {
    for (int n = 0; n < 3; n++) {
      servoLpulse(angle);
    }
  }
}
//设置模式中定时结束时将窗帘拉开
void servoLCtrlSetting_2(int sl) {
  if (sl >= lux_1 && sl <= lux_2) {
    for (int angle = angle_3; angle >= angle_2; angle--) {
      for (int n = 0; n < 3; n++) {
        servoLpulse(angle);
      }
    }
  }
  else if (sl > lux_2) {
    for (int angle = angle_3; angle >= angle_1; angle--) {
      for (int n = 0; n < 3; n++) {
        servoLpulse(angle);
      }
    }
  }
}
//设置模式中将窗帘拉开后根据光照进行一次窗帘调节，使自动化时舵机转动更自然
void servoTCtrlMode_1() {
  for (int angle = CLOSE; angle <= OPEN; angle++) {
    for (int n = 0; n < 3; n++) {
      servoTpulse(angle);
    }
  }
}
//使窗户打开的函数
void servoTCtrlMode_2() {
  for (int angle = OPEN; angle >= CLOSE; angle--) {
    for (int n = 0; n < 3; n++) {
      servoTpulse(angle);
    }
  }
}
//使窗户关闭的函数

