#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>

#include <MeMegaPi.h>


//Encoder Motor
MeEncoderOnBoard Encoder_1(SLOT1);
MeEncoderOnBoard Encoder_2(SLOT2);
MeEncoderOnBoard Encoder_3(SLOT3);
//MeEncoderOnBoard Encoder_4(SLOT4);

MeMegaPiDCMotor gripper(1);
//MeDCMotor gripper(12);
int grip_duration = 100;
unsigned long target_time = millis();
bool pending_time = false;
bool is_moveto = false;
unsigned long gripper_target = millis();
bool gripper_pending = false;
bool gripper_isopen = true;

bool is_time_out() {
  if (pending_time) {
    if (millis() > target_time) {
      pending_time = false;
      Serial.println("encoder ended");
      return true;
    }
    if (millis() + 5000 < target_time) {
      target_time = millis() + 500;
    }
    return false;
  }
  return true;
}

bool is_gripper_timeout() {
  if (gripper_pending) {
    if (millis() > gripper_target) {
      gripper_pending = false;
      Serial.println("gripper ended");
      return true;
    }
    if (millis() + 5000 < gripper_target) {
      gripper_target = millis() + 500;
    }
    return false;
  }
  return true;
}

bool set_time_out(float seconds) {
  Serial.println("---------set_time_out---------------");
  Serial.println(millis());
  pending_time = true;
  target_time = millis() + seconds * 1000;
  Serial.println(target_time);
}

bool set_gripper_timeout(float seconds) {
  Serial.println("---------gripper_set_timeout---------------");
  Serial.println(millis());
  gripper_pending = true;
  gripper_target = millis() + seconds * 1000;
  Serial.println(gripper_target);
}

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
/*
void isr_process_encoder4(void)
{
      if(digitalRead(Encoder_4.getPortB()) == 0){
            Encoder_4.pulsePosMinus();
      }else{
            Encoder_4.pulsePosPlus();
      }
}
*/
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
    attachInterrupt(Encoder_1.getIntNum(), isr_process_encoder1, RISING);
    Encoder_1.setPulse(8);
    Encoder_1.setRatio(46.67);
    Encoder_1.setPosPid(1.8,0,1.2);
    Encoder_1.setSpeedPid(0.18,0,0);  
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
}

int encoder_1_angle = 0;
int encoder_2_angle = 0;
int encoder_3_angle = 0;
int encoder_1_speed = 6;
int encoder_2_speed = 0;
int encoder_3_speed = 0;
int state = 0;

void set_state(int next_state) {
  if (state == next_state) {
    return;
  }
  
  switch (next_state) {
    // hot sauce
    case 1:
      encoder_1_angle = 60;
      encoder_1_speed = 20;
      encoder_2_speed = 0;
      Serial.println("state 1");
      if (state != 1) {
        go_down(&Encoder_1, -30, 2);
      } else {
        if (!pending_time) {
          set_gripper_timeout(500);
          if (gripper_isopen) {
            gripper.run(255);
            gripper_isopen = false;
          } else {
            gripper.run(-200);
            gripper_isopen = true;
          }
        }
      }
      
      break;
    // cheese powder
    case 2:
      encoder_1_angle = 0;
      encoder_1_speed = 20;
      encoder_2_speed = 60;
      if (state == 1) {
        if (!gripper_isopen) {
          set_gripper_timeout(500);
          gripper.run(-200);
          gripper_isopen = true;
        }
      }
      move_to(&Encoder_1, 0, 30);
      break;
    default:
      encoder_1_angle = 0;
      encoder_1_speed = 20;
      encoder_2_speed = 0;
      if (state == 1) {
        if (!gripper_isopen) {
          set_gripper_timeout(500);
          gripper.run(-200);
          gripper_isopen = true;
        }
      }
      move_to(&Encoder_1, 0, 30);
      break;
  }
  state = next_state;
}

void go_down(MeEncoderOnBoard* encoder, int move_speed, int wait_time) {
  set_time_out(wait_time);
  encoder->runSpeed(move_speed);
}

void move_to(MeEncoderOnBoard* encoder, int angle, int move_speed) {
  int curr_pos = encoder->getCurPos();
  if (curr_pos != angle) {
    if (abs(angle - curr_pos) > 0 && abs(angle - curr_pos) < 10) {
      int buffer_angle = curr_pos < angle ? 8 : -8;
      encoder->moveTo(angle + buffer_angle, move_speed);
      is_moveto = false;
    } else {
      encoder->moveTo(angle, move_speed);
    }
  } else {
    is_moveto = false;
  }
}

int get_state(int tilt_angle) {
  if (tilt_angle > 20) {
    return 1;
  } else if (tilt_angle < -20) {
    return 2;
  }
  return 0;
}

void loop(){
  /*
   * Encoder_1: Chili Sauce Motor
   * Encoder_2: Cheese banging Motor
   */
  
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    if (input[0] >= 'a' && input[0] <= 'z') {
      // DEBUG
      angle = input.substring(1).toInt();
      switch(input[0]) {
        case 'a':
          if (angle == NULL) {
            set_state(1); 
          } else {
            set_time_out(3);
            Encoder_1.runSpeed(angle);
          }
          break;
        case 'b':
          if (angle == NULL) {
            set_state(2);
          } else {
            set_gripper_timeout(500);
            angle = angle > 255 ? 255 : angle;
            angle = angle < -255 ? -255 : angle;
            gripper.run(angle);
          }
          break;
        case 'c':
          set_state(0);
          break;
        case 'd':
          encoder_1_angle += angle;
          encoder_1_speed = 20;
          is_moveto = true;
          break;
        case 'e':
          set_time_out(1.2);
          Encoder_3.runSpeed(angle);
          break;
        case 'f':
          encoder_3_angle += angle;
          encoder_3_speed = 60;
          break;
        case 'g':
          encoder_2_speed = angle;
          break;
      }
    } else {
      // RELEASE
      if (ultrasonic_8.distanceCm() < 10) {
        angle = input.toInt();
        int next_state = get_state(angle);
        set_state(next_state);
      } else {
        set_state(0);
      }
    }
  }
  
  if (is_time_out()) {
    Encoder_1.runSpeed(0);
    Encoder_3.runSpeed(0);
    //gripper.run(0);
  }
  if(is_gripper_timeout()) {
    gripper.run(0);
  }

  if (is_moveto) {
    move_to(&Encoder_1, encoder_1_angle, encoder_1_speed);
  }
  //move_to(&Encoder_1, encoder_1_angle, encoder_1_speed);
  Encoder_2.runSpeed(encoder_2_speed);
  //move_to(&Encoder_3, encoder_3_angle, encoder_3_speed);
  _loop();
}

void _delay(float seconds){
    long endTime = millis() + seconds * 1000;
    while(millis() < endTime)
      _loop();
}

void _loop(){
    Encoder_1.loop();
    Encoder_2.loop();
    Encoder_3.loop();
}
