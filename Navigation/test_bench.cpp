/*
This is a test bench to ensure that all features of the navigation node are working properly. 

*/

#include <Arduino.h>

int Timer1 = 1000;
int Timer2 = 3000;    //Timer value in microseconds

void test1(){
  while(1){
    // digitalWrite(13, HIGH);
    // delayMicroseconds(1000);
    // digitalWrite(13, LOW);
    // delayMicroseconds(1000);
    GPIOC_PTOR = 1<<5;
    delayMicroseconds(500);
    GPIOC_PTOR = 1<<5;
    delayMicroseconds(Timer1);
    GPIOC_PTOR = 1<<7;
    delayMicroseconds(500);
    GPIOC_PTOR = 1<<7;
    delayMicroseconds(Timer2);
  }  
}

void test2(){
  while(1)
  {
    // GPIOC_PTOR = 1<<5;
    // delayMicroseconds(Timer1);
    // GPIOC_PTOR = 1<<7;
    // delayMicroseconds(Timer2);
    GPIOC_PTOR = 1<<5;
    delayMicroseconds(500);
    GPIOC_PTOR = 1<<5;
    delayMicroseconds(Timer1);
    GPIOC_PTOR = 1<<7;
    delayMicroseconds(500);
    GPIOC_PTOR = 1<<7;
    delayMicroseconds(Timer2+1000);
  }// Both detect first strip but timer2 expires and neither detects second strip
}

void test3()
{
  while(1)
  {
    GPIOC_PTOR = 1<<5;
    delayMicroseconds(500);
    GPIOC_PTOR = 1<<5;
    delayMicroseconds(Timer1);
    GPIOC_PTOR = 1<<7;
    delayMicroseconds(500);
    GPIOC_PTOR = 1<<7;
    delayMicroseconds(Timer2+2000);
  } //Same as previous except after timer expires, |accelerometer - RR| > 150 
}

void test4()
{
  
}

int main(){
 // pinMode(PIN_D6, OUTPUT);       // LED
 // pinMode(PIN_D7, INPUT_PULLUP); // Pushbutton 
     pinMode(13, OUTPUT); 
     pinMode(12, OUTPUT);
     pinMode(11, OUTPUT);
  while(1){
   // digitalWrite(PIN_D6, LOW);

  }
  return 1;
}

/*
This is a test bench to ensure that all features of the navigation node are working properly. 
*/

// #include <Arduino.h>
// #include <FlexCAN.h>
// /*
// #ifndef __MK66FX1M0__
//   #error "Teensy 3.6 with dual CAN bus is required to run this example"
// #endif
// */
// static CAN_message_t msg;

// //Test all state change for navigation node
// /*
//     case 0: {         //Power On; Low Voltage Electronics powered on
//     //xTaskNotifyGive(PKTHDLE1);
//     system_state = 0;
//     break;    
//     }
//     case 1: {         //Idle; from Power node (High Voltage stuff is on)
//       system_state = 1; 
//     break;    
//     }
//     case 2: {         //Ready; from Comm Node (signal that Health Check is Cool)
//       system_state = 2;
//       break;
//     }
//     case 3: {         //Awaiting Pusher Attachment; from Comm Node (signal from UI to launch Pod)
//       system_state = 3;
//       break;
//     }
//     case 4: {         //Pushing
//       //report Error
//       break;
//     }
//     case 5: {         //Levitation & Breaking;          
//       //report Error
//       break;
//     }
//     case 6: xTaskNotifyGive(PKTHDLE7);break;

// */

// int stateChangeTest{
//   /*
//   0  : Initialisation happens in navigation node and state 0 is reported on serial monitor
// State0  

//   0-1: Power node reports high voltage battery. Fake CAN packet and see CAN ack on logic analyser and state change on Serial monitor.

// State1

//   1-2: Comm sends successful health status CAN packet. See CAN ack on logic analyser and response on Serial monitor. 
// State2

//   2-3: Comm sends CAN pod start signal. See CAN ack on logic analyser and response on Serial monitor. 
// State3

//   3-4: Fake accelerometer reading to 0.1 m/s^2
// State4
//   Fake accelerometer reading and see actuator engaging packet on CAN and data packet on serial

//   4-5: Fake sensor stripe value = 6
// State5
// a. Test normal braking PID
// b. Test normal braking PID with pitch> 1.3
// c. Test normal braking and LTS = 11mm
// d. Velocity error> 65 units
// e. Test normal braking and yaw > 0.5
  
//   5-6: Fake velocity values
// State6

// Other states later





// */
//   return 1;
// }

// int Test1{
// //Communication and UI

//   //1,Idle - Ready,,,Successful Health Check,Health Check button on the
//   //UI,Fake the values into the health check such that one of the parameters
//   //are not in order. The UI should display that the health check has failed
//   //and should deactivate the button to start the pod. Run this test for all
//   //the parameters that are being tested. Then run the test making sure that
//   //all parameters are in order. The UI should display that the check
//   //completed successfully and should activate the pod start button.
  
//   return 1;
// }

// int Test2{
// // Check state change on navigation by faking state change: Ready - Awaiting Pusher... Comm sends 
//   msg.ext = 0;
//   msg.id = 0x100;
//   msg.len = 1;

//   //2,Ready - Awaiting Pusher,,,Pod start command from UI,UI,Connect
//   //the test MCU to the main MCU and successfully finish the health
//   //check. Press the Pod start on the UI and monitor the change in
//   //state of Pod as displayed on the UI.
//   return 1;
// }

// int Test3{

// // Fake acceleration value to the navigation node and ensure state change.

//   //3,Awaiting Pusher - Pushing,,,Acceleration > 0.01
//   //m/s^2,Accelerometer/RR Sensor readings,A test bench written on a
//   //different MCU fakes the accelerometer reading provided by the
//   //sensor and the corresponding state change is observed on the
//   //Serial Monitor and on the UI.
//   return 1;
// }

// int Test4{


//   //4,Acceleration on wheels - Retraction of Wheels,,,Less than 4
//   //sec left for disengaging pusher,Accelerometer/RR Sensor readings
//   //and a timer,"The software takes in the fake values for position
//   //and velocity at discrete intervals in time from a test
//   //microcontroller . When less than 4 sec are left to reach 1600
//   //feet (Pusher Detachment), the levitation wheels start to
//   //retract."
//   return 1;
// }

// int Test5{
//   //5,Pushing - Levitation & Braking,,,Retroreflective Strip Count
//   //is 16 ,Retroreflective Sensor,Test: SSH into the pod network and
//   //send a fake CAN packet that changes the state in each of the
//   //nodes on the Pod to the Pushing state. We then change the
//   //retroreflective strip count to 16 and see that the pod state
//   //changes to levitation/braking.
//   return 1;
// }

// int Test6{
//   //6,Levitation & Braking - Descent & Retraction of
//   //Brakes,,,Velocity < 25 m/s,Accelerometer/RR sensor readings,"SSH
//   //into the pod network and send a fake CAN packet that changes the
//   //state in each of the nodes on the Pod to the ‘Levitation &
//   //Braking’ state. The software takes in fake values for position
//   //and velocity at discrete intervals in time from a test MCU. As
//   //soon as the velocity falls below 25 m/s, the UI displays the
//   //state as ‘Descent & Retraction of Brakes’ and the brakes start
//   //to disengage (which can be seen either visually or on the UI
//   //using the data from the Laser Triangulation Sensors)."
//   return 1;
// }

// int Test7{
//   //7,Normal Braking - Excessive Pitch,,,Pitch > 1.3 degrees,IMU
//   //(Gyroscope),"SSH into the pod network and send a fake CAN packet
//   //that changes the state in each of the nodes on the Pod to the
//   //‘Levitation & Braking’ state. The software takes in fake values
//   //for Yaw, Pitch, Roll at discrete intervals in time from a test
//   //MCU. We run the PID algorithm using fake position and velocity
//   //values and observe that brakes are engaging as expected. As soon
//   //as the pitch goes beyond 1.3 degrees, the actuation of brake
//   //pads halts from 0.5 seconds (which can be seen either visually
//   //or on the UI using the data from the Laser Triangulation
//   //Sensors)."
//   return 1;
// }

// int Test8{
//   //8,Rolling on wheels - Low Speed Drive,,,Velocity < 2 m/s and
//   //Current position more than 100 feet from the end of
//   //tube,Accelerometer/RR Sensor,"SSH into the pod network and send
//   //a fake CAN packet that changes the state in each of the nodes on
//   //the Pod to the ‘Rolling on Wheels’ state. The software takes in
//   //the fake values for position and velocity. When the value of
//   //velocity is less than 2 m/s and position is more than 100 feet,
//   //low speed drive is engaged."
//   return 1;
// }

// int Test9{
//   //9,Low Speed Drive - Pod Stop,,,Velocity == 0,Accelerometer/RR
//   //Sensor,"SSH into the pod network and send a fake CAN packet that
//   //changes the state in each of the nodes on the Pod to the ‘Low
//   //Speed Drive’ state. The software takes in the fake values for
//   //position and velocity. When the value of velocity is less than 2
//   //m/s and position is more than 100 feet, low speed drive is
//   //engaged."
//   return 1;
// }

// int Test10{
//   //10,Rolling on wheels - Pod Stop,,,Velocity == 0,Accelerometer/RR
//   //Sensor,"SSH into the pod network and send a fake CAN packet that
//   //changes the state in each of the nodes on the Pod to the
//   //‘Rolling on Wheels’ state. The software takes in the fake values
//   //for position and velocity. When the value of velocity is less
//   //than 2 m/s and position is less than 100 feet, the pod continues
//   //to roll on wheels till the velocity becomes 0."
//   return 1;
// }

// int Test11{
//   //11,Rolling on wheels - Emergency Braking,,,Velocity > 2 m/s and
//   //Current position less than 100 feet from the end of
//   //tube,Accelerometer/RR Sensor,"SSH into the pod network and send
//   //a fake CAN packet that changes the state in each of the nodes on
//   //the Pod to the ‘Rolling on Wheels’ state. The software takes in
//   //the fake values for position and velocity. When the value of
//   //velocity is more than 2 m/s and position is less than 100 feet,
//   //the pod applies emergency brakes to being it to a halt."
//   return 1;
// }

// int Test12{
//   //12,Sensor Failure - Emergency Braking,,,,,"For any state in the
//   //system, fake sensor readings beyong the acceptable values in
//   //that range. The pod would deploy emergency braking and the same
//   //will be seen in the UI."
//   return 1;
// }

// int Test13{
//   //13,Power Failure - Emergency Braking,,,,,Turn on the pod and
//   //disconnect the power source. The pod would deploy emergency
//   //brakes and will be reflected in the UI as well. Repeat the test
//   //with the pusher switch closed. Emergency brakes will not be
//   //deployed.
//   return 1;
// }

// int Test14{ 
//   //14,Pod to Base Station Communication channel breaks,,,3 Health
//   //packet not acknowledged on the Hyperloop network,Beagle Bone
//   //connected on the Pod network,Ensure that the pod is actively
//   //able to communicate with the base station. Make sure the pusher
//   //contact switch is turned off. Then turn off the base station.
//   //The pod should deploy emergency brakes immediately.

//   return 1;
// }

// int Test15{
//   //15,Inactive network node (Power node),,,3 Health packet not
//   //acknowledged on the Pod network,Power node connect to the Pod
//   //network,Turn on the low power electronics. The communication
//   //node starts to broadcast health packet over the pod network.
//   //Power off the power node after that. The pod should
//   //automatically deploy emergency brakes.
//   return 1;
// }

// int Test16{
//   //16,Inactive network node (Any node other than Power
//   //node),,,Beagle Bone does not receive an acknowledgement for the
//   //health packets over the CAN network and declares an
//   //emergency,Beagle Bone connected on the Pod network,Turn on the
//   //low power electronics. The communication node starts to
//   //broadcast health packet over the pod network. Power off one of
//   //the nodes after that. The pod should automatically deploy
//   //emergency brakes.
//   return 1;
// }

// int Test17{
//   //17,,,,Emergency Braking command from UI,Base Station UI,Press
//   //the Emergency stop button from the UI. The Pod should
//   //immediately deploy emergency brakes if the Pusher contact switch
//   //is turned off.
//   return 1;
// }

// int Test18{
//   //18,Normal Braking - Emergency Braking (Excessive Yaw),,,Yaw >
//   //0.5 degrees,IMU,Turn on the low power electronics and rotate the
//   //IMU by more than 0.5 degrees. The pod would deploy emergency
//   //brakes.
//   return 1;
// }

// int Test19{
//   //19,,1,Both the sensors read the same strip withint a certain
//   //time interval and update the position and velocity value
//   //accordingly,RRS2 reads stripe,Retro Reflective Sensor 2,Connect
//   //the test MCU to the main MCU and raise interrupts on the pins
//   //connected to the Retro Reflective Sensor 1 & 2 with a certain
//   //time. The updated position variable should be displayed on the
//   //Base Station
//   return 1;
// }

// int Test20{
//   //20,,2,"Both the sensors read the same strip withint a certain
//   //time interval and update the position and velocity value
//   //accordingly. However timer expires before any of the sensors can
//   //trigger an interrupt again. After the timer has expired, S2
//   //raises an interrupt. Here the difference in position reading
//   //from the accelerometer and the RR sensor is less than 150 feet
//   //",Timer T1 expires and RRS2 detects a strip after that,Timer T1
//   //and Retro Reflective Sensor 2,"Connect the test MCU to the main
//   //MCU and raise interrupts on the pins connected to the Retro
//   //Reflective Sensor 1 & 2 within a certain time. Do not raise any
//   //interrupt till a certain time elapses. Raise an interrupt on the
//   //pin connected to RR Sensor 2. Make sure the difference in the
//   //position value due to the accelerometer and the RR strip
//   //position value are less than 150 feet. The updated position
//   //variable should be displayed on the Base Station, along with an
//   //error saying that 'Sensor 1 missed strip'"
//   return 1;
// }

// int Test21{
//   //21,,3,"Both the sensors read the same strip withint a certain
//   //time interval and update the position and velocity value
//   //accordingly. However timer expires before any of the sensors can
//   //trigger an interrupt again. After the timer has expired, S2
//   //raises an interrupt. Here the difference in position reading
//   //from the accelerometer and the RR sensor is greater than 150
//   //feet ",Timer T1 expires and RRS2 detects a strip after
//   //that,Timer T1 and Retro Reflective Sensor 2,Connect the test MCU
//   //to the main MCU and raise interrupts on the pins connected to
//   //the Retro Reflective Sensor 1 & 2 within a certain time. Do not
//   //raise any interrupt till a certain time elapses. Raise an
//   //interrupt on the pin connected to RR Sensor 2. Make sure the
//   //difference in the position value due to the accelerometer and
//   //the RR strip position value are greater than 150 feet. An
//   //emergency braking command should be issued on the CAN network
//   //which can be read using the Communication node or a Serial
//   //Monitor
//   return 1;
// }

// int Test22{
//   //22,,4,"Both the sensors read the same strip withint a certain
//   //time interval and update the position and velocity value
//   //accordingly. However timer expires before any of the sensors can
//   //trigger an interrupt again. After the timer has expired, S1
//   //raises an interrupt. Here the difference in position reading
//   //from the accelerometer and the RR sensor is greater than 150
//   //feet ",Timer T1 expires and RRS1 detects a strip after
//   //that,Timer T1 and Retro Reflective Sensor 1,Connect the test MCU
//   //to the main MCU and raise interrupts on the pins connected to
//   //the Retro Reflective Sensor 1 & 2 within a certain time. Do not
//   //raise any interrupt till a certain time elapses. Raise an
//   //interrupt on the pin connected to RR Sensor 1. Make sure the
//   //difference in the position value due to the accelerometer and
//   //the RR strip position value are greater than 150 feet. An
//   //emergency braking command should be issued on the CAN network
//   //which can be read using the Communication node or a Serial
//   //Monitor
//   return 1;
// }

// int Test23{
//   //23,,5,"Both the sensors read the same strip withint a certain
//   //time interval and update the position and velocity value
//   //accordingly. However timer expires before any of the sensors can
//   //trigger an interrupt again. After the timer has expired, the
//   //difference in position reading from the accelerometer and the RR
//   //sensor is greater than 150 feet ",RRS1 and RRS2 raise an
//   //interrupt butTimer1 expires after that. And the position due to
//   //acceleration is 150 feet greater than the last stripe
//   //encountered,"Timer T1, Retro Reflective Sensor 1, Retro
//   //Reflective Sensor 2",Connect the test MCU to the main MCU and
//   //raise interrupts on the pins connected to the Retro Reflective
//   //Sensor 1 & 2 within a certain time. Do not raise any interrupt
//   //till a certain time elapses. Make sure the difference in the
//   //position value due to the accelerometer and the RR stripe
//   //position value is more than 150 feet. The system would go into
//   //an emergency state and brakes would be engaged
//   return 1;
// }

// int Test24{
//   //24,,6,"Both the sensors read the same strip withint a certain
//   //time interval and update the position and velocity value
//   //accordingly. However timer expires before any of the sensors can
//   //trigger an interrupt again. After the timer has expired, S1
//   //raises an interrupt. Here the difference in position reading
//   //from the accelerometer and the RR sensor is less than 150 feet.
//   //Timer 2 expires (ie. The second sensor missed the strip)",Timer
//   //T1 expires and RRS1 detects a strip after that. T2 expires,Timer
//   //T1 and Retro Reflective Sensor 1,Connect the test MCU to the
//   //main MCU and raise interrupts on the pins connected to the Retro
//   //Reflective Sensor 1 & 2 within a certain time. Do not raise any
//   //interrupt till a certain time elapses. Raise an interrupt on the
//   //pin connected to RR Sensor 1. Make sure the difference in the
//   //position value due to the accelerometer and the RR strip
//   //position value are less than 150 feet. Raise an interrupt on the
//   //pin connected to RR Sensor 2. The position value should be
//   //updated on the UI.
//   return 1;
// }

// int Test25{
//   //25,,7,"Both the sensors read the same strip withint a certain
//   //time interval and update the position and velocity value
//   //accordingly. However timer expires before any of the sensors can
//   //trigger an interrupt again. After the timer has expired, S2
//   //raises an interrupt. Here the difference in position reading
//   //from the accelerometer and the RR sensor is less than 150 feet.
//   //Sensor 2 triggers an interrupt after that.",Timer T1 expires and
//   //RRS1 detects a strip after that. RRS2 reads the strip,Timer T1
//   //and Retro Reflective Sensor 1,Connect the test MCU to the main
//   //MCU and raise interrupts on the pins connected to the Retro
//   //Reflective Sensor 1 & 2 within a certain time. Do not raise any
//   //interrupt till a certain time elapses. Raise an interrupt on the
//   //pin connected to RR Sensor 1. Make sure the difference in the
//   //position value due to the accelerometer and the RR strip
//   //position value are less than 150 feet. Raise should an interrupt
//   //on the pin connected to RR Sensor 2. The position value should
//   //be updated on the UI.
//   return 1;
// }

// int Test26{
//   //26,,8,Both the sensors read the same stripe withint a certain
//   //time interval and update the position and velocity value
//   //accordingly. RRS2 reads the next stripe(RRS1 missed a
//   //stripe),RRS1 and RRS2 detect an interrupt within a certain time
//   //interval. RRS2 detects another stripe,Retro Reflective Sensor 1
//   //and Retro Reflective Sensor 2,Connect the test MCU to the main
//   //MCU and raise interrupts on the pins connected to the Retro
//   //Reflective Sensor 1 & 2 within a certain time. Raise another
//   //interrupt on the pin connected to Retro Reflective Sensor 2. The
//   //position value should be pdated on the UI and error should be
//   //displayed on the UI saying 'Sensor 1 missed stripe'.
//   return 1;
// }

// int Test27{
//   //27,,9,Both the sensors read the same stripe withint a certain
//   //time interval and update the position and velocity value
//   //accordingly. RRS2 reads the next stripe(RRS1 missed a stripe).
//   //RRS1 reads the next stripe and then RRS2 reads the stripe after
//   //that,RRS1 and RRS2 raise an interrupt within a certain time
//   //interval. RRS2 detects another stripe. RRS1 and RRS2 detect
//   //another stripe within a certain time interval.,Retro Reflective
//   //Sensor 1 and Retro Reflective Sensor 2,Connect the test MCU to
//   //the main MCU and raise interrupts on the pins connected to the
//   //Retro Reflective Sensor 1 & 2 within a certain time. Raise
//   //another interrupt on the pin connected to Retro Reflective
//   //Sensor 2. The position value should be updated on the UI and
//   //error should be displayed on the UI saying 'Sensor 1 missed
//   //stripe'. The value of the position should be updated again
//   return 1;
// }

// int Test28{
//   //28,,10,Both the sensors read the same stripe withint a certain
//   //time interval and update the position and velocity value
//   //accordingly. RRS2 reads the next stripe(RRS1 missed a stripe).
//   //No sensors detects any strip till the timer expires,RRS1 and
//   //RRS2 raise an interrupt within a certain time interval. RRS2
//   //detects another stripe. Timer T1 expires,Retro Reflective Sensor
//   //1 and Retro Reflective Sensor 2 and Timer T1,"Connect the test
//   //MCU to the main MCU and raise interrupts on the pins connected
//   //to the Retro Reflective Sensor 1 & 2 within a certain time.
//   //Raise another interrupt on the pin connected to Retro Reflective
//   //Sensor 2. The position value should be pdated on the UI and
//   //error should be displayed on the UI saying 'Sensor 1 missed
//   //stripe'. After this the results for transition 2,3,4,5,6,7 hold"
//   return 1;
// }

// int Test29{
//   //29,,11,Both the sensors read the same stripe withint a certain
//   //time interval and update the position and velocity value
//   //accordingly. RRS2 reads the next stripe(RRS1 missed a stripe)
//   //and the strip after that. (Sensor 1 is down) ,RRS1 and RRS2
//   //raise an interrupt within a certain time interval. RRS2 detects
//   //another stripe. RRS2 detect another stripe within a certain time
//   //interval.,Retro Reflective Sensor 1 and Retro Reflective Sensor
//   //2,"Connect the test MCU to the main MCU and raise interrupts on
//   //the pins connected to the Retro Reflective Sensor 1 & 2 within a
//   //certain time. Raise another interrupt on the pin connected to
//   //Retro Reflective Sensor 2. The position value should be updated
//   //on the UI and error should be displayed on the UI saying 'Sensor
//   //1 missed stripe'. The value of the position should be updated
//   //again. RRS2 is encountered again, the error is displayed on the
//   //UI and pos value is updated"
//   return 1;
// }

// int Test30{
//   //30,,12,Both the sensors read the same stripe within a certain
//   //time interval and update the position and velocity value
//   //accordingly. RRS2 reads the next stripe(RRS1 missed a stripe)
//   //and the strip after that. (Sensor 1 is down). The system would
//   //detect an emergency scenario.,RRS1 and RRS2 raise an interrupt
//   //within a certain time interval. RRS2 detects another stripe.
//   //RRS2 detect another stripe within a certain time interval.,Retro
//   //Reflective Sensor 1 and Retro Reflective Sensor 2,"Connect the
//   //test MCU to the main MCU and raise interrupts on the pins
//   //connected to the Retro Reflective Sensor 1 & 2 within a certain
//   //time. Raise another interrupt on the pin connected to Retro
//   //Reflective Sensor 2. The position value should be updated on the
//   //UI and error should be displayed on the UI saying 'Sensor 1
//   //missed stripe'. The value of the position should be updated
//   //again. RRS2 is encountered again, the error is displayed on the
//   //UI and position value is updated. Emergency brakes are engaged"
//   return 1;
// }

// int Test31{
//   //31,,13,Sensor 1 reads a strip and the timer expires before
//   //Sensor 2 can read the strip (Sensor 2 missed the strip),RRS1
//   //raises an interrupt and then T2 expires,Retro Reflective Sensor
//   //1 and Timer T2,Connect the test MCU to the main MCU and raise
//   //interrupt on the pin connected to Retro Reflective Sensor 1.
//   //Then wait for the timer to expire. The UI should display the
//   //error 'RR Sensor 2 missed stripe' and the position value should
//   //be updated on the UI
//   return 1;
// }

// int Test32{
//   //32,,14,Sensor 1 reads a strip and the timer expires before
//   //Sensor 2 can read the strip (Sensor 2 missed the strip). Sensor
//   //1 reads another strip,RRS1 raises an interrupt and then T2
//   //expires. RRS1 raises an interrupt again,Retro Reflective Sensor
//   //1 and Timer T2,Connect the test MCU to the main MCU and raise
//   //interrupt on the pin connected to Retro Reflective Sensor 1.
//   //Then wait for the timer to expire. The UI should display the
//   //error 'RR Sensor 2 missed stripe' and the position value should
//   //be updated on the UI. Transition 1 can be used to validate this
//   //further
//   return 1;
// }

// int Test33{
//   //33,,15,Sensor 1 reads a strip and the timer expires before
//   //Sensor 2 can read the strip (Sensor 2 missed the strip). Sensor
//   //2 reads a strip after that.,RRS1 raises an interrupt and then T2
//   //expires. RRS2 raises an interrupt after that. ,Retro Reflective
//   //Sensor 1 & 2 and Timer T2 ,Connect the test MCU to the main MCU
//   //and raise interrupt on the pin connected to Retro Reflective
//   //Sensor 1. Then wait for the timer to expire. The UI should
//   //display the error 'RR Sensor 2 missed stripe' and the position
//   //value should be updated on the UI. After that the UI should
//   //display 'RR Sensor 1 missed stripe' and the position value
//   //should be updated.
//   return 1;
// }

// int Test34{
//   //34,,16,Sensor 1 reads a strip and the timer expires before
//   //Sensor 2 can read the strip (Sensor 2 missed the strip). Sensor
//   //1 again reads a strip and timer expires before Sensor 2 reads
//   //the strip. ,RRS1 raises an interrupt and then T2 expires. RRS1
//   //again raises an interrupt and T2 expires.,Retro Reflective
//   //Sensor 1 and Timer T2,"Connect the test MCU to the main MCU and
//   //raise interrupt on the pin connected to Retro Reflective Sensor
//   //1. Then wait for the timer to expire. The UI should display the
//   //error 'RR Sensor 2 missed stripe' and the position value should
//   //be updated on the UI. Wait for the process to repeat again.
//   //After the second timer expires, the system declares an emergency
//   //and brakes actuate. "
//   return 1;
// }

// int Test35{
//   //35,,17,Sensor 1 reads a stripe. Then Timer 2 expires and after
//   //that Timer 1 expires. ,RRS1 raises an interrupt and then T2
//   //expires. After that T1 expires. ,"Retro Reflective Sensor 1,
//   //Timer T1 and T2","Connect the test MCU to the main MCU and raise
//   //interrupt on the pin connected to Retro Reflective Sensor 1.
//   //Then wait for the timer to expire. The UI should display the
//   //error 'RR Sensor 2 missed stripe' and the position value should
//   //be updated on the UI. After that wait for Timer 1 to expire. We
//   //can follow the results of transition 2,3,4,5,6,7, from here. "
//   return 1;
// }


// int main(){
//   Can0.begin(500000);  
//     pinMode(13, OUTPUT);


//   return 1;
// }

// //Serial Number,State Transition,Transition Number (as on the statechart),State Transition Explained,Transition Trigger,Trigger Source,Transition Test