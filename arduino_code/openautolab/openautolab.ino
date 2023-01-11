#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <HX711.h>
unsigned long int t0; // here time of start will be stored
unsigned long int secs; // here number of seconds on display will be stored
const byte motorplus =5; //positive pole of pump motor
const byte motorminus=6; //negative pole of pump motor
const byte valve1=11; //pins
const byte valve2=12; //of
const byte valve3=10; //valves
const byte valve4=9;
const byte valve5=7;
const byte valve6=8;
const byte servo=4; //servo pin
const byte scaleclk=2; //pins of
const byte scaledat=3; //scale
const byte buzzer=13; //buzzer pin
const byte displayclk=A4; //pins of
const byte displaydio=A5; //display
const byte button1=A0; //buttons
const byte button2=A1;
const byte button3=A2;
byte dvlpr=0; //developer index
byte k=0; //state index
byte container; //pin corresponding to current valve
unsigned long int airpump=10000UL; //number of milliseconds pumping without liquid tolerated   
bool error=false; //if something sent wrong machine beeps
Servo mixer;
HX711 scale;
LiquidCrystal_I2C lcd(0x27,16,2);
void agitation(float a, float b, float c, float d) {
unsigned long int init=1000.0 * a; //duration of one unit of first agitation, 1sec
unsigned long int intvl=1000.0 * b; //agitation every unit of time, 1sec
unsigned long int agit=1000.0 * c; //substequent agitations duration unit, 1sec
unsigned long int devt=60000.0 * d; //overall development time unit, 1min
t0=millis();
while ((millis()-t0)<devt) {
  if (((millis()-t0)%intvl)<agit || (millis()-t0)<init) {
    if (((millis()-t0)%2000UL)<1000UL) mixer.write(180);
    else mixer.write(0);
  }
  secs=(t0+devt-millis())/1000UL;
  //display.showNumberDecEx((secs/60UL)*100UL+(secs%60UL), 0b01000000, false);
}
}

void beep() {
  t0=millis();
  while (1) {
    if((millis()-t0)%1000UL<500UL) digitalWrite(buzzer,HIGH);
    else digitalWrite(buzzer,LOW);
    if(millis()-t0>5000) {digitalWrite(buzzer,LOW); break;} //here be time of beepeng on error
    if(digitalRead(button3)==LOW) {digitalWrite(buzzer,LOW); break;} //here be interrupt beeping and continuing
    }
}

void bip() {
  t0=millis();
  while (1) {
    if((millis()-t0)%500UL<250UL) digitalWrite(buzzer,HIGH);
    else digitalWrite(buzzer,LOW);
    if(millis()-t0>5000) {digitalWrite(buzzer,LOW); break;} //here be time of beepeng on error
    if(digitalRead(button3)==LOW) {digitalWrite(buzzer,LOW); break;} //here be interrupt beeping and continuing
    }
}

void intank(int tank) {
  switch(tank) {
    case 1:
     container=valve1;
     break;
    case 2:
     container=valve2;
     break;
    case 3:
     container=valve3;
     break;
    case 4:
     container=valve4;
     break;
    case 5:
     container=valve5;
     break;
  }
  //display.clear();
  int i=0;
  float measurements[10];
  scale.tare();
  digitalWrite(container,LOW);
  digitalWrite(motorminus,LOW);
  t0=millis();
  while (1) {
    delay(100);
    measurements[i]=scale.get_units();
    //display.showNumberDecEx((int) measurements[i], 0b00000000, false);
    if(measurements[i]>300) {error=false; break;}
    i=(i+1)%10;
    float maximum=measurements[0];
    float minimum=measurements[0];
    for(int j=0;j<10;j++) {
      if(measurements[j]<minimum) minimum=measurements[j];
      if(measurements[j]>maximum) maximum=measurements[j];
    }
    if(maximum-minimum<3.0 && millis()-t0>airpump) {error=true; break;}
  }
  digitalWrite(motorminus,HIGH);
//  delay(500);
  digitalWrite(container,HIGH);
//  delay(500);
}

void outtank(int tank) {
  switch(tank) {
    case 1:
     container=valve1;
     break;
    case 2:
     container=valve2;
     break;
    case 3:
     container=valve3;
     break;
    case 4:
     container=valve4;
     break;
    case 5:
     container=valve5;
     break;
  }
  //display.clear();
  int i=0;
  float measurements[10];
  scale.tare();
  digitalWrite(container,LOW);
  digitalWrite(motorplus,LOW);
  t0=millis();
  while (1) {
    delay(100);
    measurements[i]=scale.get_units();
    //display.showNumberDecEx((int) measurements[i], 0b00000000, false);
    i=(i+1)%10;
    float maximum=measurements[0];
    float minimum=measurements[0];
    for(int j=0;j<10;j++) {
      if(measurements[j]<minimum) minimum=measurements[j];
      if(measurements[j]>maximum) maximum=measurements[j];
    }
    if(maximum-minimum<3.0 && millis()-t0>airpump) break;
  }
  digitalWrite(motorplus,HIGH);
//  delay(500);
  digitalWrite(container,HIGH);
//  delay(500);
}

void stage(int in, int out, float a, float b, float c, float d) {
  intank(in);
  if(error) beep();
  agitation(a,b,c,d);
  outtank(out);
}
void develop() {
  switch (dvlpr){
    case 0: //r09 + foma 400
//              init int- agit devel
//              agit erv  durat time
      stage(1,5, 60,  30,  5,   11); //develop in rodinal 1+50
      stage(4,5, 30,  60,  10,   1); //in-between wash
      stage(2,2, 30,  30,  5,    6);  //fix
      stage(4,5,  5,  30,  5,    5);   //wash1
      stage(4,5,  5,  30,  5,    5);   //wash2
      stage(4,5,  5,  30,  5,    5);   //wash3
      break;
    case 1: //lqn
//              init int- agit devel
//              agit erv  durat time
      stage(1,1, 30,  60,  10,  9);   //develop in lqn
      stage(4,5, 30,  60,  10,  1);   //in-between wash
      stage(2,2, 30,  30,   5,  6);   //fix
      stage(4,5,  5,  30,   5,  5);   //wash1
      stage(4,5,  5,  30,   5,  5);   //wash2
      stage(4,5,  5,  30,   5,  5);   //wash3
      break;
    case 2: //tetenal 30
//              init int- agit devel
//              agit erv  durat time
      stage(4,5, 15,  15,   5,  5);   //pre-heat
      stage(1,1, 15,  15,   5,  9);   //colour developer
      stage(2,2, 15,  15,   5,  8);   //bleach fix
      stage(4,5, 15,  15,   5,  2);   //rinse
      stage(4,5, 15,  15,   5,  2);   //rinse
      stage(4,5, 15,  15,   5,  2);   //rinse
      stage(3,3, 15,  15,   5,  1);   //stab
      break;
    case 3: //tetenal 38
//              init int- agit devel
//              agit erv  durat time
      stage(4,5, 15,  15,   5,  5);   //pre-heat
      stage(1,1, 15,  15,   5,  3.5); //colour developer
      stage(2,2, 15,  15,   5,  6);   //bleach fix
      stage(4,5, 15,  15,   5,  1.5); //rinse
      stage(4,5, 15,  15,   5,  1.5); //rinse
      stage(3,3, 15,  15,   5,  1);   //stab
      break;
    case 4: //rollei+fuji super hr
    
//              init int- agit devel
//              agit erv  durat time
      stage(1,1, 10,  60,  5,   7);   //develop 
      stage(4,5, 30,  60,  10,  1);   //in-between wash
      stage(2,2, 30,  30,   5,  6);   //fix
      stage(4,5,  5,  30,   5,  5);   //wash1
      stage(4,5,  5,  30,   5,  5);   //wash2
      bip();                          //add wetting agent
      stage(4,5,  5,  30,   5,  5);   //wash3
      break;

    case 5: //r09+foma100
    
//              init int- agit devel
//              agit erv  durat time
      stage(1,5, 10,  60,  5,   9);   //develop 
      stage(4,5, 30,  60,  10,  1);   //in-between wash
      stage(2,2, 30,  30,   5,  6);   //fix
      stage(4,5,  5,  30,   5,  5);   //wash1
      stage(4,5,  5,  30,   5,  5);   //wash2
      bip();                          //add wetting agent
      stage(4,5,  5,  30,   5,  5);   //wash3
      break;

    case 6: //r09+fuji
    
//              init int- agit devel
//              agit erv  durat time
      stage(1,5, 10,  60,  5,   8);   //develop 
      stage(4,5, 30,  60,  10,  1);   //in-between wash
      stage(2,2, 30,  30,   5,  6);   //fix
      stage(4,5,  5,  30,   5,  5);   //wash1
      stage(4,5,  5,  30,   5,  5);   //wash2
      bip();                          //add wetting agent
      stage(4,5,  5,  30,   5,  5);   //wash3
      break;
}
}
void wait(float waittime) {
  unsigned long int devt=(int) (60000.0*waittime);
  t0=millis();
  while ((millis()-t0)<devt) {
    secs=(t0+devt-millis())/1000UL;
    //display.showNumberDecEx((secs/60UL)*100UL+(secs%60UL), 0b01000000, false);
  }
}

void setup() {
analogReference(INTERNAL);
pinMode(motorplus,OUTPUT);
pinMode(motorminus,OUTPUT);
pinMode(valve1,OUTPUT);
pinMode(valve2,OUTPUT);
pinMode(valve3,OUTPUT);
pinMode(valve4,OUTPUT);
pinMode(valve5,OUTPUT);
pinMode(valve6,OUTPUT);
pinMode(buzzer,OUTPUT);
pinMode(button1,INPUT_PULLUP);
pinMode(button2,INPUT_PULLUP);
pinMode(button3,INPUT_PULLUP);
digitalWrite(valve1,LOW);
digitalWrite(valve2,LOW);
digitalWrite(valve3,LOW);
digitalWrite(valve4,LOW);
digitalWrite(valve5,LOW);
digitalWrite(valve6,LOW);
digitalWrite(motorplus,LOW);
digitalWrite(motorminus,LOW);
digitalWrite(buzzer,LOW);
mixer.attach(servo);
//display.setBrightness(7);
//display.clear();
scale.begin(scaledat,scaleclk);
scale.set_scale(1850);
lcd.init();
lcd.backlight();
}
void loop() {
  switch(k){
    case 0: 
      //display.setSegments(tan1);
      lcd.setCursor(3, 0);
//      lcd.clear();
      lcd.print("tan1");
      if(digitalRead(button1)==LOW) {k=6; delay(200);}
      if(digitalRead(button2)==LOW) {k++; delay(200);}
      if(digitalRead(button3)==LOW) {
        digitalWrite(valve1,LOW);
        digitalWrite(motorplus,LOW);
//        digitalWrite(motorminus,LOW);
      }
      if(digitalRead(button3)==HIGH) {
        digitalWrite(motorplus,HIGH);
//        digitalWrite(motorminus,HIGH);
        digitalWrite(valve1,HIGH);
      }
      break;
    case 1:
      //display.setSegments(tan2);
      lcd.setCursor(3, 0);
//      lcd.clear();
      lcd.print("tan2");
      if(digitalRead(button1)==LOW) {k--; delay(200);}
      if(digitalRead(button2)==LOW) {k++; delay(200);}
      if(digitalRead(button3)==LOW) {
        digitalWrite(valve2,LOW);
        digitalWrite(motorplus,LOW);
//        digitalWrite(motorminus,LOW);
      }
      if(digitalRead(button3)==HIGH) {
        digitalWrite(motorplus,HIGH);
//        digitalWrite(motorminus,HIGH);
        digitalWrite(valve2,HIGH);
      }
      break;
    case 2: 
      //display.setSegments(tan3);
      lcd.setCursor(3, 0);
//      lcd.clear();
      lcd.print("tan3");
      if(digitalRead(button1)==LOW) {k--; delay(200);}
      if(digitalRead(button2)==LOW) {k++; delay(200);}
      if(digitalRead(button3)==LOW) {
        digitalWrite(valve3,LOW);
        digitalWrite(motorplus,LOW);
//        digitalWrite(motorminus,LOW);
      }
      if(digitalRead(button3)==HIGH) {
        digitalWrite(motorplus,HIGH);
//        digitalWrite(motorminus,HIGH);
        digitalWrite(valve3,HIGH);
      }
      break;
    case 3: 
      //display.setSegments(wash);
      lcd.setCursor(3, 0);
//      lcd.clear();
      lcd.print("wash");
      if(digitalRead(button1)==LOW) {k--; delay(200);}
      if(digitalRead(button2)==LOW) {k++; delay(200);}
      if(digitalRead(button3)==LOW) {
        stage(4,1,0,1,0,0);
        stage(4,2,0,1,0,0);
        stage(4,3,0,1,0,0);
        stage(1,5,0,1,0,0);
        stage(2,5,0,1,0,0);
        stage(3,5,0,1,0,0);
        beep();
      }
      break;
    case 4:
      //display.setSegments(dev);
      lcd.setCursor(3, 0);
//      lcd.clear();
      lcd.print("dev");
      if(digitalRead(button1)==LOW) {k--; delay(200);}
      if(digitalRead(button2)==LOW) {k++; delay(200);}
      if(digitalRead(button3)==LOW) {
        develop();
        beep();
        }
        break;
    case 5: 
      //display.setSegments(devh);
      lcd.setCursor(3, 0);
//      lcd.clear();
      lcd.print("devh");
      if(digitalRead(button1)==LOW) {k--; delay(200);}
      if(digitalRead(button2)==LOW) {k++; delay(200);}
      if(digitalRead(button3)==LOW) {
        wait(10.0);
        develop();
        beep();
      }      
      break;
    case 6: 
      //display.setSegments(heat);
      lcd.setCursor(3, 0);
//      lcd.clear();
      lcd.print("heat");
      if(digitalRead(button1)==LOW) {k--; delay(200);}
      if(digitalRead(button2)==LOW) {k=0; delay(200);}
      if(digitalRead(button3)==LOW) {
        beep();
      }
      break;
  }
}


