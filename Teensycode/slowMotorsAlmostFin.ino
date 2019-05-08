// set this to the hardware serial port you wish to use
//1.0001.5001.9001.0001.0001.000?
//pwm values.(1200-1700) low:245.76 middle: 307.2 high: 348.16
//#define HWSERIAL Serial1
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

float forward_back[6] = {-1, -1, 0, 1, 1, 0};
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
String str1 = "";
float motors[6]       = {0, 0, 0, 0, 0, 0};
float motorsSlow[6]   = {0, 0, 0, 0, 0, 0};
float strafe[6]       = {1, -1, 0, -1, 1, 0};
float turn[6]         = {1, -1, 0, 1, -1, 0};
float rollL[6]         = {0, 0, -1, 0, 0, -1};
float rollR[6]         = {0, 0, 1, 0, 0, 1};
float up_down[6]      = {0, 0, 1, 0, 0, -1};


//------------PCA VALUE CALCULATIONS----------------
//PCA VALUE = (micro-seconds you are trying to send)/(20,000 micro-seconds)*(4096)

float pwmHigh = 2000.0/20000*4096.0;
float pwmMid = (1500.0/20000*4096.0)-8.0;
float pwmLow = 1000.0/20000*4096.0;
float pwmRange = (pwmHigh - pwmLow)/2;
bool hasBeenArmed = true;
bool arming = false;
float armingCounter = 0;

//-------------------------------------------------

/////////
//for servos: a 1 degree increase is equal to a 5 micro-second pulse increase
/////////

void setup() {
  Serial.begin(9600);
//  HWSERIAL.begin(9600);
  pwm.begin();
  pwm.setPWMFreq(50);  // This is the maximum PWM frequency
  Wire.setClock(200000);

  //arm esc's
  //delay(5000);
  armESCs(pwmLow);
  delay(500);
  armESCs(pwmMid);

}

void armESCs(int x){
    for (int i = 1; i < 6; i++){pwm.setPWM(i,0, x);};
}
  
void loop() {


  //pwm.setPWM(8,0,2000);   //Servo marked #3 Spin left Right 
  //pwm.setPWM(9,0,pwmHigh);   //Servo marked #4 Open/Close arm movements
  //pwm.setPWM(10,0,pwmHigh);  //Servo marked #1. Posterior Left Side Parallel Servo
  //pwm.setPWM(11,0,pwmHigh);  //Servo marked #2. Posterior Right Side Parallel Servo (Unverified)

  
  int incomingByte;   
  if (Serial.available() > 0) {
    incomingByte = Serial.read(); ////STEP #1: READ INCOMING MESSAGE, BYTE BY BYTE 
    if (incomingByte == '?'){ //IF END OF MESSAGE
      float motors[6] = {0, 0, 0, 0, 0, 0}; //SET MOTOR VALUES TO INTIAL CONDITION OF 0
      
      
      String movement;
      //strafe first
      
      Serial.println(str1); //FEEDBACK TO THE JETSON TO SEE THE VALUES
      for (int i = 0; i < 6; i++){ //GO THROUGH ALL THE MOTOR WITH THE NEW VALUES GOT FROM THE CONTROLLER
        //programescs
        //motors[i] = motors[i] + (str1.substring(0,4).toFloat()-1);
        //strafe


        //calculating motor power from controller values 
        //string format: 1.0001.0001.0001.0001.0001?

        /*----------------PARSING THE MESSAGE AND ADDING THE VECTORS-----------*/
        
        motors[i] = motors[i] + strafe[i] * (str1.substring(0,4).toFloat()-1); //ADDING THE POWER OF THE MOTOR TO THE MOTOR VALUE
        //turn
        motors[i] = motors[i] + turn[i] * (str1.substring(5,9).toFloat()-1);
        //forward_back
        motors[i] = motors[i] + forward_back[i] * (str1.substring(10,14).toFloat()-1);
        //roll left
        motors[i] = motors[i] + rollL[i] * (str1.substring(15,19).toFloat()-1);
        //roll right
        motors[i] = motors[i] + rollR[i] * (str1.substring(20,24).toFloat()-1);
        //up down
        motors[i] = motors[i] + up_down[i] * (str1.substring(25,29).toFloat()-1);
        if (motors[i] < -1){motors[i]=-1;};
        if (motors[i] > 1){motors[i]=1;};

        /*---------------------------------------------------------------------*/

        
        //Test script
        //arrow keys for big claw
        if (str1.charAt(30) == '1'){//FORMAT FOR READING A SINGLE BUTTON PRESS
           pwm.setPWM(7,0,pwmHigh);
          //pwm.setPWM(10,0,1300);
        }
        else if(str1.charAt(31) == '1'){
         pwm.setPWM(7,0,pwmLow); 
        }
        else{
          pwm.setPWM(7,0,pwmMid);
        }
        //Test script
        //pwm.setPWM(11,0,int(1700));
        //pwm.setPWM(10,0,int(1300)); 
        /*
         
          Start of Manipulator All values are
          theoretical until verified with server

          *confirm DPad charval on all cases
        else if(str1.charAt(30)== '1') 
        {
          //Dpad Up--Confirm Bilat Movement
          pwm.setPWM(11,0,1700);//               MATH INSTEAD OF HARD SETTING VALUES: 1700/20000*4096
          pwm.setPWM(10,0,1300);
        } else if(str1.charAt(31)== '1') 
        {
          //Dpad Down--Confirm Bilat Movement
          pwm.setPWM(11,0,1300);
          pwm.setPWM(10,0,1700);
        }else if(str1.charAt(32)== '1') 
        {
          //Dpad Left Spin CW--Confirm intuitivness
          pwm.setPWM(8,0,1700);
        } else if(str1.charAt(33)== '1') 
        {
         //Dpad Right Spin CCW--Confirm intuitivness
          pwm.setPWM(8,0,1300);
        } else if(str1.charAt(36)== '1') 
        {
          *Add L Bumber to Socket_io_Server.js
          //L Bumber Close Claw--Confirm intuitivness
          pwm.setPWM(9,0,1300);
        } else if(str1.charAt(37)== '1') 
        {
          //R Bumper Open Claw--Confirm intuitivness
          pwm.setPWM(9,0,1700);
        } else {
          //If no press do nothing
          //Distal Servo (Spin 360)
           pwm.setPWM(8,0,int(pwmMid));
           //Anterior Servo (Open/Close Claw)
           pwm.setPWM(7,0,int(pwmMid));
           //Posterior Bilat Servos (Arm Up/Down) 
           pwm.setPWM(10,0,int(pwmMid));
           pwm.setPWM(11,0,int(pwmMid));
           //Keel ESC Open close Brushless
           pwm.setPWM(9,0,int(pwmMid));
        } */



        //if in arming mode---------------------------
        //INCRIMENTING TO VALUE FUNCTION:
        
        if ((motors[i] - motorsSlow[i]) > 0.1){//POSITIVE CHANGE
          motorsSlow[i] += 0.1;
        }
        else if((motors[i] - motorsSlow[i]) < -0.1){//NEGATIVE CHANGE
          motorsSlow[i] -= 0.1;
        }
        else{//IF THE SAME, DONT DO ANYTHING
          motorsSlow[i] = motors[i];
        }

        //-------------------------------------------

        if (hasBeenArmed) {
          if (i == 0){
            pwm.setPWM(6,0,int(motorsSlow[i]*pwmRange+pwmMid));//EDGE-CASE CATCH. MOTOR 0 SOLDERED TO CHANNEL 6. SPECIFICALLY FOR MOTOR 0.
          }
          else{
             pwm.setPWM(i,0,int(motorsSlow[i]*pwmRange+pwmMid));//FOR ALL THE REST Of THE MOTORS FOR SETTING ALL THE MOTOR VALUES
          }
        }
        Serial.print(int(motorsSlow[i]*pwmRange+pwmMid));  //print ranges to console
        Serial.print(" ");
      }
      Serial.println("");
      str1 = "";
      
      
    }
    else{ //ELSE WE HAVENT READ ALL THE BITS SO KEEP STORING THE BYTES AND MOVING TO THE END OF MESSAGE
       str1 += char(incomingByte);
    }
  }
}
