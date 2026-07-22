uint16_t sensorValues[6];
int button = 13;
//MOTOR PART
int motor_speed_right;
int motor_speed_left;
int motor_outpin_right=5;
int motor_outpin_left=6;
int control_right1=3;
int control_right2=4;
int control_left1=7;
int control_left2=9;
int standby=8;
int base_speed=175;
int motor_pid_speed=0;
//Equation PART
int p=50,i=0,d=0;
float error=0,last_error=0;
int pterm,iterm,dterm;
//Sensor PART
int calibrated[6];
long int sum[]={0,0,0,0,0,0};


void setup(){
  //Inputs for sensor
  pinMode(A0,INPUT);
  pinMode(A1,INPUT);
  pinMode(A2,INPUT);
  pinMode(A3,INPUT);
  pinMode(A4,INPUT);
  pinMode(A5,INPUT);
  //analogWriteResolution()

  Serial.begin(9600);
  pinMode(control_right1,OUTPUT);
  pinMode(control_right2,OUTPUT);
  pinMode(control_left1,OUTPUT);
  pinMode(control_left2,OUTPUT);
  pinMode(standby,OUTPUT);  
  pinMode(motor_outpin_right,OUTPUT);
  pinMode(motor_outpin_left,OUTPUT);
  pinMode(button,INPUT_PULLUP);
  while(digitalRead(button));
  calibrate();
  while(digitalRead(button));
}

void loop(){
  fetch_sensors();
  refresh_PID();
  //move();
  check_output();
} 

void fetch_sensors(){
  sensorValues[0] = analogRead(A0);
  sensorValues[1] = analogRead(A1);
  sensorValues[2] = analogRead(A2);
  sensorValues[3] = analogRead(A3);
  sensorValues[4] = analogRead(A4);
  sensorValues[5] = analogRead(A5);
  for(int i=0;i<6;i++){
    calibrated[i] = sensorValues[i]-sum[i];
  }
}

void check_output(){
  calc_error();
  for(int i=0;i<6;i++){
    Serial.print(calibrated[i]);
    Serial.print("\t");
  }
  Serial.println();
  Serial.print(error);
  Serial.println();
  delay(250);
}

void calibrate(){
  fetch_sensors();
  //digitalWrite(LED,LOW);
  delay(1000);
  //digitalWrite(LED,HIGH);
  for(int j=1;j<=2000;j++){

  for(int i=0;i<6;i++){
    sum[i]+=sensorValues[i];
  }
  }
  for(int i=0;i<6;i++){
    sum[i]/=2000;
  }
  // digitalWrite(LED,LOW);
  // delay(200);
  // digitalWrite(LED,HIGH);
  // delay(200);
  // digitalWrite(LED,LOW);
  // delay(200);
  // digitalWrite(LED,HIGH);
}
void new_calibration(){
  int highest[] = {0,0,0,0,0},lowest[]= {0,0,0,0,0};
  digitalWrite(standby,HIGH);
  digitalWrite(control_left1,LOW);
  digitalWrite(control_left2,HIGH);
  digitalWrite(control_right1,HIGH);
  digitalWrite(control_right2,LOW);
  analogWrite(motor_outpin_right,220);
  analogWrite(motor_outpin_left,220);
  for(int i=0;i<2000;i++){
    fetch_sensors();
    if(sensorValues[i]>highest[i]){
      highest[i] = sensorValues[i];
    }
    else if(sensorValues[i]<lowest[i]){
      lowest[i] = sensorValues[i];
    }
  }
  for(int i=0;i<5;i++){
    calibrated[i] = (highest[i]+lowest[i])/2;
  }

}

void motordriving(){
  motor_speed_left = base_speed - motor_pid_speed;
  motor_speed_right = base_speed + motor_pid_speed;
}
void move(){
  motordriving();
  digitalWrite(standby,HIGH);
  digitalWrite(control_left1,LOW);
  digitalWrite(control_left2,HIGH);
  digitalWrite(control_right1,LOW);
  digitalWrite(control_right2,HIGH);
  analogWrite(motor_outpin_right,motor_speed_right);
  analogWrite(motor_outpin_left,motor_speed_left);
}
void stall(){
  digitalWrite(standby,LOW);
}
void refresh_PID(){
  calc_error();
  pterm = error;
  iterm += error;
  dterm = error - last_error;
  motor_pid_speed = (p*pterm)+(i*iterm)+(d*dterm);
  last_error = error;
}
void calc_error(){
 //FOR 6 sensors active
 // error = sensorValues[2]-sensorValues[3]+2*(sensorValues[1]-sensorValues[4])+4*(sensorValues[0]-sensorValues[5]);
 //FOR 5 sensors active
 error = sensorValues[3]+2*(sensorValues[4]-sensorValues[2])+4*(sensorValues[0]-sensorValues[0]);
 error = error/10;
}