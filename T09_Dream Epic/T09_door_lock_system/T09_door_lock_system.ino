#include <LiquidCrystal.h>
#include <Keypad.h>
#include <Servo.h>
#include <SoftwareSerial.h>

#define LED pins
#define redLED 5
#define greenLED 4
#define buzzer 3
#define motor 6

boolean inReset = false; //set true if an interrupt occur. Then main loop start from the beginning. 

String id1="#001";
String id2="#002";
String id3="#003";
String enteredID="";
String enteredPW="";
int lck=0;

const int buttonPin1 = 2; 

const byte ROWS = 4; 
const byte COLS = 3; 

char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {10, 9, 8, 7}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {11, 12, 13}; //connect to the column pinouts of the keypad

Keypad digitalPad= Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS); 
LiquidCrystal lcd(A5,A4,A3,A2,A1,A0);
Servo lock;
SoftwareSerial sms(0,1);

void setup(){
  lcd.begin(16, 2);

  lock.attach(motor);
  lock.write(lck);

  sms.begin(9600);   //Module baude rate
  Serial.begin(9600); 

  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(buttonPin1, INPUT);

  attachInterrupt(0, killunlock, CHANGE);
}

  
void loop(){
  
  int i=0,passwordCheck=0;

  if(inReset){ 
    lcd.clear();
    inReset=!inReset;
    lcd.setCursor(1,0);
    lcd.print("Button pressed");
    lcd.setCursor(1,1);
    lcd.print("door unlocked");
    delay(2000);
    lcd.clear();
     }
    
  lcd.setCursor(4,0);
  lcd.print("Welcome!");
  lcd.setCursor(0, 1);
  lcd.print("  Enter your ID");
  
  while(i<=3 && !inReset){// loop 3 times until system freezes from wrong ID and start again from 0
    
    if(i>0 && i<3){
      
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(" Incorrect ID!");
      digitalWrite(redLED, HIGH);
      delay(500);
      digitalWrite(redLED, LOW);
      lcd.setCursor(0, 1);
      lcd.print(" Enter ID again");
    }
              
    if(i!=3){enteredID=getId();}
            
    if(enteredID==id1 || enteredID==id2 || enteredID==id3){
          int j;
          for(j=0;j<3;j++){ // loop 3 times until system freezes from wrong password
                if(!inReset){
                
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print(" Enter password");
                
                enteredPW=getPassword();
                passwordCheck=checkPW(enteredID,enteredPW);
                
                if(passwordCheck==1){
                  lcd.print("  Access granted!");
                  tone(buzzer,1000);
                  delay(1000);
                  noTone(buzzer);
                  delay(1000);
                  lcd.clear();
                  lcd.setCursor(0, 0);
                  lcd.print("  DOOR UNLOCKED");
                  digitalWrite(greenLED, HIGH);
                  delay(500);
                  digitalWrite(greenLED, LOW);
                  unlockdoor();
                  delay(2000);
                  lockdoor();
                  lcd.clear();
                  lcd.setCursor(0, 0);
                  lcd.print("  DOOR LOCKED");
                  delay(3000);
                  lcd.clear();
                  lcd.setCursor(0, 0);
                  break;
             }
             else if(!inReset){ 
                  lcd.clear();
                  lcd.setCursor(0, 0);
                  lcd.print(" Incorrect Password!");
                              
                  digitalWrite(redLED, HIGH);
                  delay(500);
                  digitalWrite(redLED, LOW);
                  continue;
             }
          }}
         if(passwordCheck==1){break;}
         i++;
         if(j==3 && !inReset){
              i=0;
              SecuritySMS();
              countDown();
              lcd.print(" Enter your ID");                   
         }   
     }
     else if(i==3 && !inReset){
          i++;  
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print(" Incorrect Id!"); 
          
          digitalWrite(redLED, HIGH);
          delay(500);
      
          digitalWrite(redLED, LOW);  
          SecuritySMS();       
          countDown();    
      }           
      else{
          i++;
          lcd.clear();
          enteredID="";
          continue;
     }
  }           
}        
        
//------------------ Function 1- Interrupt service routine for kill switch--------------//

void killunlock() {
  unlockdoor();
  inReset = true;
} 

//------------------ Function 2- get Id --------------//

String getId(){
              
        String enteredID="";
        String confirmkey="";
        
        while(enteredID.length() < 4 && !inReset){
           
              char keyPressed = digitalPad.getKey();
              
              if(keyPressed){
                enteredID+=keyPressed; 
                if(enteredID.length()==1){
                  lcd.clear();
                  lcd.setCursor(0, 0);
                }        
                lcd.print(keyPressed);
              } 
        }       
              
        while(enteredID.length()==4 && !inReset){
              lcd.setCursor(0,1);
              lcd.print("press * to confirm");
              char keyPressed = digitalPad.getKey();
              if(keyPressed){
                if(keyPressed=='*'){return enteredID;}
              }
        }
}        
     
//------------------ Function 3- get Password --------------//

String getPassword(){
  
  String enteredPW="";
  
  while(enteredPW.length() < 6 && !inReset){
                    char keyPressed = digitalPad.getKey();
                    if(keyPressed){
                      enteredPW+=keyPressed;
                      if(enteredPW.length()==1){
                        lcd.setCursor(0, 1);
                      }          
                      lcd.print("*");
                    }
   }
  delay(500);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("press * to confirm");
  while(enteredPW.length()==6 && !inReset){
                
      char keyPressed = digitalPad.getKey();
          if(keyPressed){
            if(keyPressed=='*'){return enteredPW;}
          }  
  }
} 
         
//------------------ Function 4- check Password--------------//

int checkPW(String enteredID,String enteredPW){
  
    String password1="145278";
    String password2="354691";
    String password3="789541";
    int c=0,id=0;
 
    if(enteredID=="#001"){id=1;}
    else if(enteredID=="#002"){id=2;}
    else if(enteredID=="#003"){id=3;}

    switch(id){
        case 1:
          if(enteredPW==password1){
            lcd.clear();
            lcd.setCursor(0, 0);                       
            c=1;
          }                           
          break;
        case 2:
          if(enteredPW==password2){
            lcd.clear();
            lcd.setCursor(0, 0);                     
            c=1;                     
          }
          break;
        case 3:
          if(enteredPW==password3){
            lcd.clear();
            lcd.setCursor(0, 0);                      
            c=1;
          }                 
          break;      
    }
    return c;
}    
  
//------------------ Function 5- countdown for screen freeze--------------//

void countDown(){
  
    delay(1000);
    for(int i=30;i>0;i--){
      if(!inReset){
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(String("Try again in ") + String(i)+String("seconds"));
        delay(500);  
        digitalWrite(redLED, HIGH);                          
        tone(buzzer, 450);                          
        delay(500);
        digitalWrite(redLED, LOW);                          
        noTone(buzzer); 
    }}
    lcd.clear();
    lcd.setCursor(0,0);
}  
  
//------------------ Function 6- unlock the door--------------//


void unlockdoor() {
  for(lck=0; lck<=180; lck+=5) {
    lock.write(lck);
    delay(50);
  }
}

//------------------ Function 7- lock the door--------------//


void lockdoor() {
    for(lck=180; lck>=0; lck-=5) {
    lock.write(lck);
    delay(50);
  }
}


//------------------ Function 8- security alert--------------//

void SecuritySMS()
{
  Serial.println("Sending SMS...");               //Show the message on serial monitor
  sms.print("AT+CMGF=1\r");                 
  delay(100);
  sms.print("AT+CMGS=\"+94710941199\"\r");  
  delay(500);
  sms.print("security alert! some one is trying to unlock!");       //text to send to the phone number
  delay(500);
  sms.print((char)26);
  delay(500);
  sms.println();
  Serial.println("Text Sent."); //print to serial monitor to show the modue is working
  delay(500);
}
