#include <LiquidCrystal_I2C.h>
#include <Wire.h>
// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

#include "DHT.h"
#define DHTPIN 2     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11

DHT dht(DHTPIN, DHTTYPE);
int newcount=0;
int passato=0;
int attuale=1;

int valore = 0;
int letturaPrecedente = HIGH;

int max=30;
int min=max-3;

int luminosita=0;
#define encoderA 11
#define encoderB 12

void setup() { 
  lcd.begin();
  lcd.backlight();
  Serial.begin(9600);
  pinMode(3, INPUT_PULLUP); //cambio pagina
  pinMode(4, INPUT_PULLUP); // diminuisco temp
  pinMode(5, INPUT_PULLUP); //aumento temp
  pinMode(6, OUTPUT); //buzzer
  pinMode(7, OUTPUT); //relè
  pinMode (encoderA,INPUT_PULLUP);  //encoder
  pinMode (encoderB,INPUT_PULLUP);  //encoder
  dht.begin();
}

void loop() { 
    attuale=digitalRead(3);
    if(attuale==LOW && passato==HIGH){
      if(newcount==2){
      newcount=0;
      Serial.print("contatore ");
      Serial.println(newcount);
      }else{
      newcount=newcount+1;
      lcd.clear();
      Serial.print("contatore ");
      Serial.println(newcount);
      }
    }
    //Aumento abbasso temp
if(newcount==1){
 Serial.println("damn");
 int n = digitalRead(encoderA);
  if ((letturaPrecedente == HIGH) && (n == LOW)) {
    if (digitalRead(encoderB) == HIGH) {
      max--;
      min--;
    } else {
      max++;
      min++;
    }
    Serial.println(valore);    
  } 
  Serial.println(max);  
          digitalWrite(6,LOW);
          lcd.setCursor(6,0);
          lcd.print(max);
          lcd.setCursor(9,0);
          lcd.print((char)223);
          lcd.setCursor(0,1);
          lcd.print("<-(-)");
          lcd.setCursor(11,1);
          lcd.print("(+)->"); 
  letturaPrecedente = n;             
}
//pagina iniziale
if(newcount==0){
  delay(250);
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    float hic = dht.computeHeatIndex(t, h, false);
    int state = digitalRead(7);
    Serial.println(max);
    Serial.print(min);
    lcd.setCursor (0,1);
    lcd.print("Umidita': ");
    lcd.print(h);
    lcd.print("%");
    lcd.setCursor (0,0);
    lcd.print("Temp: ");
    lcd.print(t);
    lcd.print((char)223);
    if(t<min){
      digitalWrite(7, HIGH);
      digitalWrite(6,LOW);
      Serial.println("Alzo temperatura");
      lcd.print("  ON");
    }else{
      if(t>max){
        digitalWrite(7, LOW);
        digitalWrite(6,LOW);
        Serial.println("Abbasso temperatura");
        lcd.print(" OFF");
      }else{
        if(t<max && t>min){
          if(state==HIGH){
            lcd.print(" ON");
          }else{
            lcd.print(" OFF");
          }
        }else{
          if(isnan(t)){
            digitalWrite(6,HIGH);
          }
        }
      }
    } 
}

if(newcount==2){
  if(luminosita==1){
    lcd.noBacklight();
    luminosita=0;
    newcount=0;
  }else{
    lcd.backlight();
    luminosita=1;
    newcount=0;
  }
  
}
   passato=attuale;
  
}
