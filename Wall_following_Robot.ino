//Pin configurations
const int trig_side = 11;
const int echo_side = 10;
float distance_side ;
float time_side;

const int trig_front=6;
const int echo_front= 4;
float distance_front ;
float time_front;

const int input1 =9;
const int input2 = 2;
const int ena12 = 5;

const int input3 =7;
const int input4 = 8;
const int ena34 = 3;

//Prototypes
float getDisSide();
float getDisFront();

//Set up
void setup() {
  Serial.begin(9600);
  pinMode(trig_front, OUTPUT);
  pinMode(echo_front, INPUT);
  pinMode(trig_side, OUTPUT);
  pinMode(echo_side, INPUT);

  pinMode(ena12,OUTPUT);
  pinMode(input1,OUTPUT);
  pinMode(input2,OUTPUT);

  pinMode(ena34,OUTPUT);
  pinMode(input3,OUTPUT);
  pinMode(input4,OUTPUT);
 
  digitalWrite(input1,0);
  digitalWrite(input2,1);
  digitalWrite(input3,0);
  digitalWrite(input4,1);
//Timer set up
  cli();
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 3124;// = [(16*10^6) / (1/x *256) - 1] with x is the desired interrupt time (must be <65536), the timer is currently 0.05s
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 256 prescaler
  TCCR1B |= (1 << CS12);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  sei();
}

//PID parameters
const float kp = 11.2;
const float ki = 4.61;
const float kd = 0.42;

//Variables
const int setPoint = 15;
float w = 0, w1 = 0, e = 0, e1 = 0, e2 = 0;
const float T = 0.05;
const float R = 2.15;
const int L = 11.5;
const float V_robot = 17;
const int cycle = 100;
int flag_front = 0;

void loop() 
{ 
  
}

//Interrupt Service Routine - main
ISR (TIMER1_COMPA_vect) 
{
  if(flag_front == 1)
    {
      flag_front = 0;
      w = 0;
      w1 = 0; 
      e = 0; 
      e1 = 0; 
      e2 = 0;
    
    digitalWrite(input1,0);
    digitalWrite(input2,1);
  //TCCR1A = 0;// set entire TCCR1A register to 0
  //TCCR1B = 0;// same for TCCR1B
    TCNT1  = 0;
    OCR1A = 3124;//reset to original timer
    }
  getDisSide();
  getDisFront();
  if(distance_front < 13)
  {
    w = 10;
    digitalWrite(input1,1);
    digitalWrite(input2,0);
    flag_front = 1;
    //TCCR1A = 0;// set entire TCCR1A register to 0
    //TCCR1B = 0;// same for TCCR1B
    TCNT1  = 0;
    OCR1A = 18749;// = = [(16*10^6) / (1/x *256) - 1] with x is the desired interrupt time (must be <65536), time to turn left is currently 0.3s) 
  }
  else
  {
    w1 = w;
    e2 = e1;
    e1 = e;
    e = (setPoint - distance_side);
    w = w1 + kp*(e - e1) + ((ki*T)/2)*(e + e1) + (kd/T)*(e - 2*e1 + e2);
     if (w > 8)
    {
      w = 8;
    }
    if (w < -8)
    {
      w = -8;
    }
  }
  
    float w_r = (w*R/2 + V_robot * R/L)/(R*R/L);
    float w_l = (V_robot*R/L-w*R/2)/(R*R/L);

    float rpm_r = (60 * w_r)/(2*3.14); // m/s
    float rpm_l = (60 * w_l)/(2*3.14);

    float speed_r = (rpm_r / 360)*255;
    float speed_l = (rpm_l / 360)*255;
    if(flag_front == 1)
    {
      if (speed_l < 0) 
        {
          speed_l = -speed_l;
        }
    }
    else{
      if (speed_l < 0) 
        {
          speed_l = 0;
        }
    }

    if (speed_r < 0) 
    {
      speed_r = 0;
    }
    
    analogWrite(ena12,(int)speed_l);

    analogWrite(ena34,(int)speed_r);
}

//Function to get the Side_distance
float getDisSide()
{
  digitalWrite(trig_side,1);
  delayMicroseconds(10);
  digitalWrite(trig_side,0);

  time_side = pulseIn (echo_side, HIGH);
  distance_side = (time_side/2)*0.034;
if(distance_side > 50)
{
  distance_side = 50;
}
  return distance_side;
}

//Function to get the Front_distance
float getDisFront()
{
  digitalWrite(trig_front,1);
  delayMicroseconds(10);                      
  digitalWrite(trig_front,0);

  time_front = pulseIn (echo_front, HIGH);
  distance_front = (time_front/2)*0.034;

  return distance_front;
}
