#include <ArduinoHardware.h>
#include <ros.h> 
#include <geometry_msgs/Twist.h> 
#include <std_msgs/Float32.h> 
#include <Encoder.h>

Encoder myEncL(2, 27);
Encoder myEncR(19, 23);

long oldPositionL  = 0;   // ตัวแปลที่ใช้สำหรับเก็บค่า encL
long oldPositionR  = 0;   // ตัวแปลที่ใช้สำหรับเก็บค่า encR
ros::NodeHandle nh;     // ตัวจัดการ node ใร ros โดยมีชื่อว่า nh


geometry_msgs::Twist msg;     // ประเภทของข้อความที่ใช้
std_msgs::Float32 encL_msg;   // รับค่า encL
std_msgs::Float32 encR_msg;   // รับค่า encR

ros::Publisher EncL("Enc_L", &encL_msg);    // ส่งค่า public encoder ที่ชื่อTopic Enc_L จากมอเตอร์ไปที่ ros โดยข้อความเป็นประเภท  &encL_msg
ros::Publisher EncR("Enc_R", &encR_msg);    // ส่งค่า public encoder ที่ชื่อTopic Enc_R จากมอเตอร์ไปที่ ros โดยข้อความเป็นประเภท  &encL_msg

  long newPositionL;
  long newPositionR;
    
void roverCallBack(const geometry_msgs::Twist& cmd_vel)
{

  double x = cmd_vel.linear.x;
        double z = cmd_vel.angular.z;

  double moveL = x+(z/2);   // การกดความเร็วล้อซ้าย เช่น u,l ซ้ายกับขวาจะไม่เท่ากัน
  double moveR = x-(z/2);

  
if (moveL>0.0){
     analogWrite(5,max(min(moveL*100,255),85)); // ค่ารเิ่มต้นให้มอเตอร์หมุน และค่าสูงสุด ***analog Arduino เขียนค่าได้สูงสุด 255
    //  analogWrite(5,255);  
      digitalWrite(30,0);digitalWrite(32,1);
    }else if (moveL<0.0){
  analogWrite(5,max(min(abs(moveL)*100,255),85));
 //       analogWrite(5,255);  
        digitalWrite(30,1);digitalWrite(32,0);
    }else{ 
  analogWrite(5,0);
        digitalWrite(30,0);digitalWrite(32,0);
  }

if (moveR>0.0){
      analogWrite(7,max(min(moveR*100,255),85));
      //analogWrite(7,255);  
      digitalWrite(22,0);digitalWrite(24,1);
    }else if (moveR<0.0){
  analogWrite(7,max(min(abs(moveR)*100,255),85));
        //analogWrite(7,255);   
        digitalWrite(22,1);digitalWrite(24,0);
    }else{ 
  analogWrite(7,0);
        digitalWrite(22,0);digitalWrite(24,0);
        }
}
ros::Subscriber <geometry_msgs::Twist> Motor("/cmd_vel", roverCallBack);

void setup()
{
  pinMode(5,OUTPUT);  pinMode(32,OUTPUT); pinMode(30,OUTPUT);
  pinMode(7,OUTPUT);  pinMode(22,OUTPUT); pinMode(24,OUTPUT);
  nh.initNode();
  nh.subscribe(Motor);    // subscribe ตัวมอเตอร์ 
  nh.advertise(EncL);     // ประกาศ EncL บน master
  nh.advertise(EncR);
} 

void loop()
{
    newPositionL = myEncL.read();
  newPositionR = myEncR.read()*-1;
  
  if (newPositionL != oldPositionL) {
    oldPositionL = newPositionL;
     encL_msg.data = newPositionL;
  }
  if (newPositionR != oldPositionR) {
    oldPositionR = newPositionR;
     encR_msg.data = newPositionR;
  }
  if((newPositionL>1000000)||(newPositionR>1000000)||(newPositionL<-1000000)||(newPositionR<-1000000)){   // เก็บค่า newPosition ไว้ได้ที่ 1,000,000 พอเกินก็เซตเป็น 0
  myEncL.write(0);
  myEncR.write(0);
  }
  EncL.publish( &encL_msg );
  EncR.publish( &encR_msg );
  nh.spinOnce();    // เรียกทำซ้ำ
  delay(100);
}
