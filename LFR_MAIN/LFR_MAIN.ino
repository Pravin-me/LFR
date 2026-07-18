#include <QTRSensors.h>

QTRSensors qtr;
uint16_t sensorValues[6];
//MOTOR PART
int motor_speed_right;
int motor_speed_left;
int motor_outpin_right=5;
int motor_outpin_left=6;
int control_right1=3;
int control_right2=4;
int control_left1=7;
int control_left2=2;
int standby=8;
int base_speed=200;
int motor_pid_speed=0;
//Equation PART
int p,i,d;
int error=0,last_error=0;
int pterm,iterm,dterm;
//Sensor PART
int calibrated[6];
long int sum[]={0,0,0,0,0,0};


void setup(){
  qtr.setTypeAnalog();
  qtr.setSensorPins((const uint8_t[]){A0,A1,A2,A3,A4,A5}, 6);
  qtr.setEmitterPin(2);

  //analogWriteResolution()

  Serial.begin(9600);
  pinMode(control_right1,OUTPUT);
  pinMode(control_right2,OUTPUT);
  pinMode(control_left1,OUTPUT);
  pinMode(control_left2,OUTPUT);
  pinMode(standby,OUTPUT);  
  pinMode(motor_outpin_right,OUTPUT);
  pinMode(motor_outpin_left,OUTPUT);
  calibrate();
}

void loop(){
  fetch_sensors();
  check_output();
// move();
// delay(2000);
// stall();
// delay(2000);
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
  motordriving();
  digitalWrite(standby,HIGH);
  digitalWrite(control_left1,HIGH);
  digitalWrite(control_left2,LOW);
  digitalWrite(control_right1,HIGH);
  digitalWrite(control_right2,LOW);
  analogWrite(motor_outpin_right,motor_speed_right);
  analogWrite(motor_outpin_left,motor_speed_left);
}
void stall(){
  digitalWrite(standby,LOW);
}
void refresh_PID(){
  pterm = error;
  iterm += error;
  dterm = error - last_error;
  motor_pid_speed = (p*pterm)+(i*iterm)+(d*dterm);
  //FOR TESTING
  motor_pid_speed = 0;
}