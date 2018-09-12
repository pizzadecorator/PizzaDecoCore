#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>

#include <MeMegaPi.h>


//Encoder Motor
MeEncoderOnBoard Encoder_1(SLOT1);
MeEncoderOnBoard Encoder_2(SLOT2);
MeEncoderOnBoard Encoder_3(SLOT3);
MeEncoderOnBoard Encoder_4(SLOT4);

int default_angle1 = 0;
int default_angle2 = 0;
int default_angle3 = 0;

void isr_process_encoder1(void)
{
      if(digitalRead(Encoder_1.getPortB()) == 0){
            Encoder_1.pulsePosMinus();
      }else{
            Encoder_1.pulsePosPlus();
      }
}

void isr_process_encoder2(void)
{
      if(digitalRead(Encoder_2.getPortB()) == 0){
            Encoder_2.pulsePosMinus();
      }else{
            Encoder_2.pulsePosPlus();
      }
}

void isr_process_encoder3(void)
{
      if(digitalRead(Encoder_3.getPortB()) == 0){
            Encoder_3.pulsePosMinus();
      }else{
            Encoder_3.pulsePosPlus();
      }
}

void isr_process_encoder4(void)
{
      if(digitalRead(Encoder_4.getPortB()) == 0){
            Encoder_4.pulsePosMinus();
      }else{
            Encoder_4.pulsePosPlus();
      }
}

void move(int direction, int speed)
{
      int leftSpeed = 0;
      int rightSpeed = 0;
      if(direction == 1){
            leftSpeed = -speed;
            rightSpeed = speed;
      }else if(direction == 2){
            leftSpeed = speed;
            rightSpeed = -speed;
      }else if(direction == 3){
            leftSpeed = speed;
            rightSpeed = speed;
      }else if(direction == 4){
            leftSpeed = -speed;
            rightSpeed = -speed;
      }
      Encoder_1.setTarPWM(rightSpeed);
      Encoder_2.setTarPWM(leftSpeed);
}
void moveDegrees(int direction,long degrees, int speed_temp)
{
      speed_temp = abs(speed_temp);
      if(direction == 1)
      {
            Encoder_1.move(degrees,(float)speed_temp);
            Encoder_2.move(-degrees,(float)speed_temp);
      }
      else if(direction == 2)
      {
            Encoder_1.move(-degrees,(float)speed_temp);
            Encoder_2.move(degrees,(float)speed_temp);
      }
      else if(direction == 3)
      {
            Encoder_1.move(degrees,(float)speed_temp);
            Encoder_2.move(degrees,(float)speed_temp);
      }
      else if(direction == 4)
      {
            Encoder_1.move(-degrees,(float)speed_temp);
            Encoder_2.move(-degrees,(float)speed_temp);
      }
    
}

double angle_rad = PI/180.0;
double angle_deg = 180.0/PI;
int angle_head;
MeSerial se;
MeUltrasonicSensor ultrasonic_8(8);



void setup(){
    //Set Pwm 8KHz
    TCCR1A = _BV(WGM10);
    TCCR1B = _BV(CS11) | _BV(WGM12);
    TCCR2A = _BV(WGM21) | _BV(WGM20);
    TCCR2B = _BV(CS21);
    
    Serial.begin(115200);
    Serial2.begin(115200);
    attachInterrupt(Encoder_2.getIntNum(), isr_process_encoder2, RISING);
    Encoder_2.setPulse(8);
    Encoder_2.setRatio(46.67);
    Encoder_2.setPosPid(1.8,0,1.2);
    Encoder_2.setSpeedPid(0.18,0,0);
    default_angle2 = Encoder_2.getCurPos();
    attachInterrupt(Encoder_3.getIntNum(), isr_process_encoder3, RISING);
    Encoder_3.setPulse(8);
    Encoder_3.setRatio(46.67);
    Encoder_3.setPosPid(1.8,0,1.2);
    Encoder_3.setSpeedPid(0.18,0,0);
    default_angle3 = Encoder_3.getCurPos();
    attachInterrupt(Encoder_1.getIntNum(), isr_process_encoder1, RISING);
    Encoder_1.setPulse(8);
    Encoder_1.setRatio(46.67);
    Encoder_1.setPosPid(1.8,0,1.2);
    Encoder_1.setSpeedPid(0.18,0,0);
    default_angle1 = Encoder_1.getCurPos();
    
}

void chili_activate() {
  //have to decide angle and speed
  Encoder_1.move(60, 30);
}
void chili_deactivate() {
  Encoder_1.move(-60, 30);
}

void cheese_activate() {
  Encoder_2.runSpeed(120);
  Encoder_3.move(90, 30);
}
void cheese_deactivate() {
  Encoder_2.runSpeed(0);
  Encoder_3.move(-90, 30);
}

void set_motor_default() {
  if (flag == 1) {
    chili_deactivate();
  }
  else if(flag == 2) {
    cheese_deactivate();
  }
  else {
    //stop everything
    Encoder_1.runSpeed(0);
    Encoder_2.runSpeed(0);
    Encoder_3.runSpeed(0);
  }
  flag = 3;
}

int flag = 0;
/*
 * 0 : default
 * 1 : chili
 * 2 : cheese
 * 3 : default angle manipulation 
 */

void loop(){
  Serial.println(Encoder_3.getCurPos());
  /*
   * Encoder_1: Chili Sauce Motor
   * Encoder_2: Cheese banging Motor
   * Encoder_3: Cheese lid opening Motor
   */
   
  if ((ultrasonic_8.distanceCm()) < 10) {
    if(Serial.available()){
        String temp = Serial.readStringUntil('\n');
        angle_head = temp.toInt();
        if(angle > 25) {
          //chili
          if (flag == 1) {
            //nothing to do 
          }
          else if (flag == 2) {
            chili_activate();
            cheese_deactivate(); 
          }
          else {
            //flag 0 and 3: just activate chili
            chili_activate();
          }
          flag = 1;
        }
        else if (angle < -25) {
          //cheese
          if (flag == 1) {
            chili_deactivate();
            cheese_activate();
          }
          else if (flag == 2) {
            //continue
          }
          else {
            cheese_activate();
          }
          flag = 2;
        }
        else {
          set_motor_default();  
        }
    }
  }
  else { 
    if(Serial.available() {
      //default angle manipulation 
      String order = Serial.readStringUntil('\n');
      if(order == null) {
        
      }
      else {
        if(order[0] == 'a') {
          angle = order.substring(1).toInt();
          flag = 3;
          Encder_1.move();
        }
        else if (order[1] == 'b') {
          angle = order.substring(1).toInt();
          flag = 3;
          Encoder_3.move();
        }
        else {
          //do nothing
        }
      }
      
    }
    else {
      //default
      set_motor_default();
    }
  }
  _loop();
}

void _delay(float seconds){
    long endTime = millis() + seconds * 1000;
    while(millis() < endTime)_loop();
}

void _loop(){
    Encoder_2.loop();
    
    Encoder_3.loop();
    
    Encoder_1.loop();
}

