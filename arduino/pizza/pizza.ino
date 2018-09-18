#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>

#include <MeMegaPi.h>

//Encoder Motor
MeEncoderOnBoard Encoder_1(SLOT1);
MeEncoderOnBoard Encoder_2(SLOT2);
MeEncoderOnBoard Encoder_3(SLOT3);
MeEncoderOnBoard Encoder_4(SLOT4);

MeMegaPiDCMotor gripper(12);
int grip_duration = 100;

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
int angle;
MeSerial se;
MeUltrasonicSensor ultrasonic_8(8);

void setup(){
    //Set Pwm 8KHz
    TCCR1A = _BV(WGM10);
    TCCR1B = _BV(CS11) | _BV(WGM12);
    TCCR2A = _BV(WGM21) | _BV(WGM20);
    TCCR2B = _BV(CS21);
    
    Serial.begin(115200);
    Serial2.begin(9600);
    attachInterrupt(Encoder_2.getIntNum(), isr_process_encoder2, RISING);
    Encoder_2.setPulse(8);
    Encoder_2.setRatio(46.67);
    Encoder_2.setPosPid(1.8,0,1.2);
    Encoder_2.setSpeedPid(0.18,0,0);
    attachInterrupt(Encoder_3.getIntNum(), isr_process_encoder3, RISING);
    Encoder_3.setPulse(8);
    Encoder_3.setRatio(46.67);
    Encoder_3.setPosPid(1.8,0,1.2);
    Encoder_3.setSpeedPid(0.18,0,0);
    attachInterrupt(Encoder_1.getIntNum(), isr_process_encoder1, RISING);
    Encoder_1.setPulse(8);
    Encoder_1.setRatio(46.67);
    Encoder_1.setPosPid(1.8,0,1.2);
    Encoder_1.setSpeedPid(0.18,0,0);    
}

int encoder_1_angle = 0;
int encoder_2_angle = 0;
int encoder_3_angle = 0;
int encoder_1_speed = 8;
int encoder_2_speed = 0;
int encoder_3_speed = 30;

void set_state_1() {
  encoder_1_angle = 60;
  encoder_1_speed = 8;
  encoder_3_speed = 30;
  encoder_2_speed = 0;
  encoder_3_angle = 0;
}
void set_state_2() {
  encoder_1_angle = 0;
  encoder_1_speed = 8;
  encoder_3_speed = 30;
  encoder_2_speed = 150;
  encoder_3_angle = 200;
}
void set_state_0() {
  encoder_1_angle = 0;
  encoder_1_speed = 8;
  encoder_3_speed = 30;
  encoder_2_speed = 0;
  encoder_3_angle = 0;
}

void move_to(MeEncoderOnBoard* encoder, int angle, int move_speed) {
  int curr_pos = encoder->getCurPos();
  if (curr_pos != angle) {
    if (abs(angle - curr_pos) > 0 && abs(angle - curr_pos) < 10) {
      int buffer_angle = curr_pos < angle ? 8 : -8;
      encoder->moveTo(angle + buffer_angle, move_speed);
    } else {
      encoder->moveTo(angle, move_speed);  
    }
  }
}

void loop(){
  /*
   * Encoder_1: Chili Sauce Motor
   * Encoder_2: Cheese banging Motor
   * Encoder_3: Cheese lid opening Motor
   */
   
  if ((ultrasonic_8.distanceCm()) < 10) {
    if(Serial.available()){
        String temp = Serial.readStringUntil('\n');
        angle = temp.toInt();
        if(angle > 25) {
          set_state_1();
        }
        else if (angle < -25) {
          set_state_2();
        }
        else {
          Serial.write("init");
          set_state_0();
        }
    }
  }
  else {
    if(Serial.available()){
        String temp = Serial.readStringUntil('\n');
        if(temp != NULL) {
          angle = temp.substring(1).toInt();
          if(temp[0] == 'a') {
            encoder_1_angle += angle;
            encoder_1_speed = 8;
          }
          else if(temp[0] == 'b') {
            encoder_3_angle += angle;
            encoder_3_speed = 30;
            Serial.println(encoder_3_angle);
          } 
          else if (temp[0] == 'c') {
            set_state_0();
          }
        }
    }
  }

  move_to(&Encoder_1, encoder_1_angle, encoder_1_speed);
  Encoder_2.runSpeed(encoder_2_speed);
  move_to(&Encoder_3, encoder_3_angle, encoder_3_speed);
  move_gripper();
  _loop();
}

void move_gripper() {
  
  if (encoder_1_speed == 0) {
    grip_duration = 100;
    return;
  }
  
  _delay(1);
  int dir = encoder_1_speed > 0 ? 1 : -1;
  while (grip_duration-- > 0) {
    gripper.run(100 * dir);
  }
}

void _delay(float seconds){
    long endTime = millis() + seconds * 1000;
    while(millis() < endTime)
      _loop();
}

void _loop(){
    Encoder_2.loop();
    
    Encoder_3.loop();
    
    Encoder_1.loop();
}
