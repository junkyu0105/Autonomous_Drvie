#include <Servo.h>
Servo servo;

#define led A5
#define datap A2 // DS
#define latchp A3 // ST_CP
#define clockp A4 // SH_CP
#define trig_left A0
#define echo_left A1
#define trig_right 4
#define echo_right 10
#define servo_data 9

float  duration_l, distance_l, duration_r, distance_r;// Duration used to calculate distance

volatile int state;
volatile int master;
volatile int i;
volatile int j;
volatile int a;
volatile int b;

void setup() {                          // ↓ init setup ↓
  pinMode(datap, OUTPUT);               // analog(4) pin // shift regiser 14 pin 
  pinMode(latchp, OUTPUT);              // analog(3) pin // shift regiser 12 pin 
  pinMode(clockp, OUTPUT);              // analog(2) pin // shift regiser 11 pin 
  pinMode(2, INPUT_PULLUP);             // mode change button
  pinMode(3, INPUT_PULLUP);             // on/off button
  pinMode(5, OUTPUT);                   // dc motor foward
  pinMode(6, OUTPUT);                   // dc motor reverse
  pinMode(led, OUTPUT);                 // main LED(3mm)
  pinMode(trig_left, OUTPUT);           // digital(7) pin ultra_sonic send(left)디
  pinMode(echo_left, INPUT_PULLUP);     // digital(8) pin ultra_sonic reception(left)
  pinMode(trig_right, OUTPUT);          // digital(4) pin ultra_sonic send(right)
  pinMode(echo_right, INPUT_PULLUP);    // digital(10) pin ultra_sonic reception(right)
  servo.attach(servo_data);             // digital(9) pin servo_motor setup
  
  attachInterrupt(0, INT0_ISR, FALLING);  // mode change button interrupt setup
  attachInterrupt(1, INT1_ISR, FALLING);  // on/off button interrupt setup

  digitalWrite(trig_left, LOW);
  digitalWrite(trig_right, LOW);
  Serial.begin(9600);
  
  state=0;                                // mode change initial variable value    
  master=0;                               // on/off initial variable value
  i=0;
  a=0;
  b=0;                             
    
  for(j=0; j<8; j++) {
  digitalWrite(latchp, LOW);
  shiftOut(datap, clockp, MSBFIRST, 0<<j); 
  digitalWrite(latchp, HIGH); 
  }                                            
}                                          // ↑ init setup ↑

void INT0_ISR() {               // MODE CHANGE BUTTON INTERRUPT
  delayMicroseconds(2000);
  if(digitalRead(2) != LOW) return;   // button chattering
  if(state==0||state==4) {
    state=1;
  }
  if(state==2) {
    state=3;
  }
}

void INT1_ISR() {               // ON/OFF BUTTON INTERRUPT
  delayMicroseconds(2000);
  if(digitalRead(3) != LOW) return;
  if(master==0||master==4){
    master=1; 
  }
  if(master==2) {
    master=3;
  }
}

void shutdown_init() {
     for (j=1; j<4; j++) {
     digitalWrite(latchp, LOW);
     shiftOut(datap, clockp, MSBFIRST, 1<<j);
     digitalWrite(latchp, HIGH);
     delay(500);
     digitalWrite(latchp, LOW);
     shiftOut(datap, clockp, MSBFIRST, 0<<3);
     digitalWrite(latchp, HIGH);
    }
    servo.write(90);
    master=4;
    state=0;
    i=0;
    a=0;
    b=0;
    digitalWrite(5, LOW);
    digitalWrite(6, LOW);
}

void left_flicker() {                                 // left_flicker(yellow led)
  if(state==2) {                                      // SC
  digitalWrite(latchp, LOW);
  shiftOut(datap, clockp, MSBFIRST, 18);
  digitalWrite(latchp, HIGH);
  delay(100);
  digitalWrite(latchp, LOW);
  shiftOut(datap, clockp, MSBFIRST, 1<<1);
  digitalWrite(latchp, HIGH);
  }
  
  if (state==4) {                                    // OB
  digitalWrite(latchp, LOW);
  shiftOut(datap, clockp, MSBFIRST, 24);
  digitalWrite(latchp, HIGH);
  delay(100);
  digitalWrite(latchp, LOW);
  shiftOut(datap, clockp, MSBFIRST, 1<<3);
  digitalWrite(latchp, HIGH);
  }
}

void right_flicker() {                               // right_flicker(yellow led)
  if(state==2) {
  digitalWrite(latchp, LOW);
  shiftOut(datap, clockp, MSBFIRST, 34);
  digitalWrite(latchp, HIGH);
  delay(100);
  digitalWrite(latchp, LOW);
  shiftOut(datap, clockp, MSBFIRST, 1<<1);
  digitalWrite(latchp, HIGH);
 }
  
  if(state==4) {
  digitalWrite(latchp, LOW);
  shiftOut(datap, clockp, MSBFIRST, 40);
  digitalWrite(latchp, HIGH);
  delay(100);
  digitalWrite(latchp, LOW);
  shiftOut(datap, clockp, MSBFIRST, 1<<3);
  digitalWrite(latchp, HIGH);
  }
}

void break_light() {                                // break_light(red led)
  if(state==2) {
  digitalWrite(latchp, LOW);
  shiftOut(datap, clockp, MSBFIRST, 66);
  digitalWrite(latchp, HIGH);
  }
  if(state==4) {
  digitalWrite(latchp, LOW);
  shiftOut(datap, clockp, MSBFIRST, 72);
  digitalWrite(latchp, HIGH);
  }
}

void emergency() {                                // emergency flasher (left, right yellow led)
   digitalWrite(latchp, LOW);
   shiftOut(datap, clockp, MSBFIRST, 56);
   digitalWrite(latchp, HIGH);
   delay(300);
   digitalWrite(latchp, LOW);
   shiftOut(datap, clockp, MSBFIRST, 1<<3);
   digitalWrite(latchp, HIGH);
   delay(300);
   digitalWrite(latchp, LOW);
   shiftOut(datap, clockp, MSBFIRST, 56);
   digitalWrite(latchp, HIGH);
   delay(300);
   digitalWrite(latchp, LOW);
   shiftOut(datap, clockp, MSBFIRST, 1<<3);
   digitalWrite(latchp, HIGH);
}

void ultra_sonic_sensing() {                      // ultra_sonic sensor Trig, Echo
  digitalWrite(trig_left, LOW);
  delayMicroseconds(2);
  digitalWrite(trig_left, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_left, LOW);
  duration_l = pulseIn(echo_left, HIGH);
  
  digitalWrite(trig_right, LOW);
  delayMicroseconds(2);
  digitalWrite(trig_right, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_right, LOW);
  duration_r = pulseIn(echo_right, HIGH);

  distance_l = ((34000*duration_l)/1000000)/2;
  distance_r = ((34000*duration_r)/1000000)/2;

  if(distance_l>100) {
    distance_l=100;
  }
   if(distance_r>100) {
    distance_r=100;
  }
  if(distance_l<0) {
    distance_l=1;
  }
  if(distance_r<0) {
    distance_r=1;
  }
//  Serial.print("L : ");
//  Serial.println(distance_l);
//  Serial.print("R : ");
//  Serial.println(distance_r);
}

void normal_driving() {
  servo.write(88);
  delay(20);
  servo.write(92);
  delay(20);
  servo.write(90);
  delay(20);
  servo.write(94);
}

void Cruise_Control() {          // MODE CHANGE : 1 (White button 1 click)
  if(state==2) {                            
  ultra_sonic_sensing();
  normal_driving();
  digitalWrite(5, HIGH);

  if(distance_l >10 && distance_l<35 && distance_r>=40) {    /*15,25*/
    right_flicker();
    servo.write(72);
    delay(100);
  }
  if(distance_l>10 && distance_l<20 && distance_r>=40) {
    right_flicker();
    servo.write(65);
    delay(100);
  }

  if(distance_r >10 && distance_r<35 && distance_l>=40) {
    left_flicker();
    servo.write(104);
    delay(100);
    }
   if(distance_r >10 && distance_r<20 && distance_l>=40) {
    left_flicker();
    servo.write(112);
    delay(100);
    }
    
   if(distance_l<10 && distance_r<10) {
       break_light();
       digitalWrite(5, LOW);    
    }   
  }
  
}

void Obstacle_detection() {      // MODE CHANGE : 2 (White button 2 click)
   if(state==4) {
    if(b<5) {
    ultra_sonic_sensing();
    normal_driving();
   
    digitalWrite(5, HIGH);
     
    if(distance_l<20||distance_r<20) {
      distance_l, distance_r = 0;     // ultra_sonic sensing reset
      servo.write(90);
      delay(20);
      break_light();
      digitalWrite(5, LOW);
      delay(500);
      digitalWrite(6, HIGH);
      emergency();
      delay(500);
      digitalWrite(6, LOW);
      delay(20);
      ultra_sonic_sensing();
      delay(20);  
      b++;
      
    if(distance_l > distance_r) {
      right_flicker();
      servo.write(65);
      delay(20);
      a=1;
      digitalWrite(5, HIGH);
       }
    else if(distance_l < distance_r) {
      left_flicker();
      servo.write(112);
      delay(20);
      a=2;
      digitalWrite(5, HIGH); 
      }
    else if(distance_l == distance_r) {
      servo.write(65);
      delay(20);
      a=1;
      digitalWrite(5, HIGH);
      }
     }
    }
   
    if(a==1) {
      Serial.println("right handlering");
      for(int c=65; c<=90; c++) {
        servo.write(c);
        delay(50);
        a=0;
      }     
      digitalWrite(5, LOW); 
    }
    else if(a==2) {
      Serial.println("left handlering");
      for(int d=112; d>=87; d--) {
        servo.write(d);
        delay(50);
        a=0;
      }
      digitalWrite(5, LOW);
    }
    
  if(b==5) {                            // After 5th operation OB end
    digitalWrite(5, LOW);
    break_light();
     }
   }
}
   
void loop() {
  digitalWrite(led, HIGH);

  if(master==1||master==2) {          // System operation (Black button 1 click)
    if(i==0) {
     servo.write(90);
     digitalWrite(latchp, LOW);
     shiftOut(datap, clockp, MSBFIRST, 119); // rgb_green , yellow_led(left, right)x2, red_ledx2 Bright
     digitalWrite(latchp, HIGH);   
    }
  
    if(state==1 && i==1) {
     digitalWrite(latchp, LOW);
     shiftOut(datap, clockp, MSBFIRST, 1<<1);  // rgb_red 
     digitalWrite(latchp, HIGH);
     servo.write(90);
     digitalWrite(5, LOW);    
     digitalWrite(6, LOW);   
     state=2;
    }

     if(state==3 && i==1) {
      digitalWrite(latchp, LOW);
      shiftOut(datap, clockp, MSBFIRST, 1<<3); // rgb_blue 
      digitalWrite(latchp, HIGH);
      servo.write(90);
      digitalWrite(5, LOW);    
      digitalWrite(6, LOW);
      b=0;   
      state=4;
     }
      master=2;
      i=1;
    Cruise_Control();
    Obstacle_detection();
}

if(master==3) {             // System shutdown (Black button 2 click)
     shutdown_init();
     }
}  
