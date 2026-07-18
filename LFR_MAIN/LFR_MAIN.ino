#include <QTRSensors.h>

QTRSensors qtr;
uint16_t sensorValues[6];
//MOTOR PART
int motor_speed_right;
int motor_speed_left;
int motor_outpin_right;
int motor_outpin_left;
int control_right1;
int control_right2
int control_left1;
int control_left2;
int standby;
int motor_pid_speed;
//Equation PART
int p,i,d;
int error=0,last_error=0;
int pterm,iterm,dterm;
//Sensor PART
int calibrated[8];
long int sum[]={0,0,0,0,0,0,0,0};


void setup(){
  qtr.setTypeAnalog();
  qtr.setSensorPins((const uint8_t[]){A0,A1,A2,A3,A4,A5}, 6);
  qtr.setEmitterPin(2);

  Serial.begin(9600);
  pinMode(motor_outpin_right,OUTPUT);
  pinMode(motor_outpin_left,OUTPUT);
  calibrate();
}

void loop(){
  fetch_sensors();
  check_output();
} 

void fetch_sensors(){
  qtr.read(sensorValues);
  for(int i=0;i<6;i++){
    calibrated[i] = sensorValues[i]-sum[i];
  }
}

void check_output(){
  for(int i=0;i<6;i++){
    Serial.print(calibrated[i]);
    Serial.print("\t");
  }
  Serial.println();
  delay(250);
}

void calibrate(){
  for(int j=1;j<=500;j++){
  qtr.read(sensorValues);
  for(int i=0;i<6;i++){
    sum[i]+=sensorValues[i];
  }
  }
  for(int i=0;i<6;i++){
    sum[i]/=500;
  }
  delay(2000);
}

void motordriving(){
  motor_speed_right = base_speed + motor_pid_speed;
  motor_speed_left = base_speed - motor_pid_speed;
}
void move(){
  digitalWrite(standby,HIGH);
  digitalWrite(control_left1,HIGH);
  digitalWrite(control_left2,LOW);
  digitalWrite(control_right1,HIGH);
  digitalWrite(control_right2,LOW);
}
void stall(){
  digitalWrite(standby,LOW);
}
void refresh_PID(){
  pterm = error;
  iterm += error;
  dterm = error - last_error;
  motor_pid_speed = (p*pterm)+(i*iterm)+(d*dterm);
}