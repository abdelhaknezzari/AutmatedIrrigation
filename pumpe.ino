
#include <Wire.h>
#include "RTClib.h"


#include <avr/sleep.h>
RTC_DS1307 RTC;

DateTime timeOld ;
enum PumpState {on,off};
PumpState currentState = off;
struct TimeStruct {
  int seconds;
  int minutes;
  int hours;
  int duration;
};


struct StateTime{
    int on;
    int off; 
};

StateTime stateTime;

TimeStruct pumpActivities[5];




void setupTimer1()
{
  int frequency = 1;
  cli();//stop interrupts
//set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A  = (unsigned long)((62500UL / frequency) - 1UL);//our clock runs at 62.5kHz, which is 1/62.5kHz = 16us
    
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
 sei();//allow interrupts
}

void setup () {
    Serial.begin(9600);
    Wire.begin();
    
    Serial.println("Initialisation complete.");
    delay(100); //Allow for serial print to complete.
 
   
    
    RTC.begin();
  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  
  stateTime.on   = 60;
  stateTime.off = 120;
  
  timeOld = RTC.now();
  
  
  setupTimer1();

  Serial.println("Interrup attached.");
  delay(200);
  
  
  
  pumpActivities[0].seconds  = 20;
  pumpActivities[0].minutes  = 20;
  pumpActivities[0].hours    = 17;
  pumpActivities[0].duration = 60;
}


int getTimeDiff( DateTime timeOld)
{
  
  
  DateTime timeNow = RTC.now(); 
    
  int diffSeconds ;
  int diffMinutes ;
  int diffHours ;
  int diffDays ;
  
  int value;


  int nowSeconds  = timeNow.second() ;
  int nowMinutes  = timeNow.minute();
  int nowHours    = timeNow.hour();
  int nowDays     = timeNow.day();


  int oldSeconds = timeOld.second() ;
  int oldMinutes = timeOld.minute() ;
  int oldHours   = timeOld.hour() ;
  int oldDays    = timeOld.day();

  if(nowSeconds >= oldSeconds )
  {
    
    diffSeconds =  nowSeconds - oldSeconds;
  } else {
    nowMinutes = nowMinutes - 1;
    nowSeconds  = nowSeconds + 60;
    diffSeconds  = nowSeconds - oldSeconds;  
  }
  
  
  if(nowMinutes >= oldMinutes )
  {
    
    diffMinutes =  nowMinutes - oldMinutes;
  } else {
    nowHours = nowHours - 1;
    nowMinutes  = nowMinutes + 60;
    diffMinutes  = nowMinutes - oldMinutes;  
  }
  
  
  value = diffSeconds + diffMinutes * 60;
  return value;
  
  

}

void dispTime(DateTime time)
{
    Serial.print(time.year(), DEC);
    Serial.print('/');
    Serial.print(time.month(), DEC);
    Serial.print('/');
    Serial.print(time.day(), DEC);
    Serial.print(' ');
    Serial.print(time.hour(), DEC);
    Serial.print(':');
    Serial.print(time.minute(), DEC);
    Serial.print(':');
    Serial.print(time.second(), DEC);
    Serial.println(); 
}


void pumpOn()
{

}

void pumpOff()
{

}


boolean isTimeOnFinshed( int timeDiff)
{
  if( timeDiff >= stateTime.on)
  {
    return true;
  } else {
    return false;
  }
}






boolean isTimeOffFinshed( int timeDiff )
{
  if( timeDiff >= stateTime.off)
  {
    return true;
  } else {
    return false;
  }
}


void goingToSleep()


{
  
    Serial.println("enterring sleep mode.");
   delay(200);
   
   
   sleep_enable();
   set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  // sleep_cpu();
 

  //sleep_mode();
  
  /* The program will continue from here. */
  
  /* First thing to do is disable sleep. */
  //sleep_disable(); 
  
  
  Serial.println("outging from sleep mode.");
  delay(200);
   
}


ISR(TIMER1_COMPA_vect){//timer1 interrupt 1Hz 
//generates pulse wave of frequency 1Hz/2 = 0.5kHz (takes two cycles for full wave- toggle high then toggle low)
   //

 // Serial.println("Timer1");

 //int timeDiff = getTimeDiff(timeOld);


   
}


void stateMachine()
{
   int timeDiff = getTimeDiff(timeOld);

//   Serial.println(timeDiff, DEC);
           
   switch(currentState)
   {
     case(on):
         pumpOn();
         if(isTimeOnFinshed(timeDiff))
         {
           currentState = off;
           timeOld = RTC.now();
           Serial.println("Off");
           Serial.print("Time Diff : ");
           Serial.println(timeDiff, DEC);
         }
         break;
     case(off):
        pumpOff();
        if(isTimeOffFinshed(timeDiff))
         {
           currentState = on;
           timeOld = RTC.now();
           Serial.println("On");
           Serial.print("Time Diff : ");
           Serial.println(timeDiff, DEC); 
            
         }   
        break;
   
   }; 
}


void loop () {
   //stateMachine();   
   
  
   //
   sleep_enable();
   set_sleep_mode(SLEEP_MODE_IDLE);
   sleep_mode();
   sleep_disable(); 
   stateMachine(); 
   //goingToSleep();
    
    
}
