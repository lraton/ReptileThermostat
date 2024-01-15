/*
 * Copyright (c) 2024, lraton 
 * All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include <LiquidCrystal_I2C.h>
#include <DS1302.h>
#include <Wire.h>
#include "DHT.h"

#define DHTPIN 2     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11

#define encoderA 11 //pin encoder
#define encoderB 12

//variabili temperatura resistore
#define RT0 10000   // Ω
#define B 3977      // K
#define VCC 5    //Supply voltage
#define R 10000  //R=10KΩ

float RT, VR, ln, TX, T0, VRT;

//varibili per encoder
int newcount = 0;
int passato = 0;
int attuale = 1;

int valore = 0;
int letturaPrecedente = HIGH;

//valori temperatura
int maxgiorno = 32;
int mingiorno = maxgiorno - 3;

int maxnotte = 25;
int minnotte = maxnotte - 3;

int luminosita = 0;

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHTPIN, DHTTYPE);
DS1302 rtc(8, 9, 10);

void setup() {
  lcd.begin();
  lcd.backlight();
  rtc.halt(false);
  rtc.writeProtect(false);
  //rtc.setDOW(SUNDAY);        // Imposta il giorno della settimana a SUNDAY
  //rtc.setTime(21, 51, 0);     // Imposta l'ora come 21:51:00 (Formato 24hr)
  //rtc.setDate(3, 9, 2022);   // Imposta la data cone 3 settembre 2022
  Serial.begin(9600);
  pinMode(3, INPUT_PULLUP); //cambio pagina
  pinMode(4, INPUT_PULLUP); // diminuisco temp
  pinMode(5, INPUT_PULLUP); //aumento temp
  pinMode(6, OUTPUT); //buzzer
  pinMode(7, OUTPUT); //relè
  pinMode (encoderA, INPUT_PULLUP); //encoder
  pinMode (encoderB, INPUT_PULLUP); //encoder
  dht.begin();
  T0 = 25 + 273.15;                 //Temperature T0 from datasheet, conversion from Celsius to kelvin
}


void loop() {
  attuale = digitalRead(3);
  if (attuale == LOW && passato == HIGH) {
    if (newcount == 3) {
      newcount = 0;
    } else {
      newcount = newcount + 1;
      lcd.clear();
    }
  }

  //pagina iniziale
  if (newcount == 0) {
    delay(250);
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    float hic = dht.computeHeatIndex(t, h, false);
    //float t = temp();
    int state = digitalRead(7);

    //lcd.setCursor (0, 0); //stampo Temperatura resistore
    //lcd.print("T1: ");
    //lcd.print(t);
    //lcd.setCursor (8, 0);
    //lcd.print((char)223);

    lcd.setCursor (0, 0); //stampo temperatura dht11
    lcd.print("Temp: "); 
    lcd.print(t);
    lcd.setCursor (11, 0);
    lcd.print((char)223);
    
    lcd.setCursor (0, 1);  //stampo umidità
    lcd.print("Umidita': "); 
    lcd.print(h);
    lcd.setCursor (15, 1);
    lcd.print("%");

    //Temperatura se è giorno 8-23
    if((rtc.getTime().hour < 23) && (rtc.getTime().hour > 8)){
      Serial.println("giorno");
       if (t < mingiorno) {
        digitalWrite(7, HIGH);
        digitalWrite(6, LOW);
        lcd.setCursor (13, 0);
        lcd.print(" ON");
      } else {
        if (t > maxgiorno) {
          digitalWrite(7, LOW);
          digitalWrite(6, LOW);
          lcd.setCursor (13, 0);
          lcd.print("OFF");
        } else {
          if (t < maxgiorno && t > mingiorno) {
            if (state == HIGH) {
              lcd.setCursor (13, 0);
              lcd.print(" ON");
            } else {
              lcd.setCursor (13, 0);
              lcd.print("OFF");
            }
          } else {
            if (isnan(t)) {
              digitalWrite(6, HIGH);
            }
          }
        }
      }
    }else{
      Serial.println("notte");
       if (t < minnotte) {
        digitalWrite(7, HIGH);
        digitalWrite(6, LOW);
        lcd.setCursor (13, 0);
        lcd.print(" ON");
      } else {
        if (t > maxnotte) {
          digitalWrite(7, LOW);
          digitalWrite(6, LOW);
          lcd.setCursor (13, 0);
          lcd.print("OFF");
        } else {
          if (t < maxnotte && t > minnotte) {
            if (state == HIGH) {
              lcd.setCursor (13, 0);
              lcd.print(" ON");
            } else {
              lcd.setCursor (13, 0);
              lcd.print("OFF");
            }
          } else {
            if (isnan(t)) {
              digitalWrite(6, HIGH);
            }
          }
        }
      }
    }
  }

  //Aumento abbasso temp
  if (newcount == 1) {
    int n = digitalRead(encoderA);
    if ((letturaPrecedente == HIGH) && (n == LOW)) {
      if (digitalRead(encoderB) == HIGH) {
        maxgiorno--;
        mingiorno--;
      } else {
        maxgiorno++;
        mingiorno++;
      }
      Serial.println(valore);
    }
    digitalWrite(6, LOW);
    lcd.setCursor(6, 0);
    lcd.print(maxgiorno);
    lcd.setCursor(9, 0);
    lcd.print((char)223);
    lcd.setCursor(0, 1);
    lcd.print("<-(-)");
    lcd.setCursor(11, 1);
    lcd.print("(+)->");
    letturaPrecedente = n;
  }

  //visualizzo la data
  if (newcount == 2) {
    data();
  }

  //spengo-accendo lcd
  if (newcount == 3) {
    lcd.clear();
    if (luminosita == 1) {
      lcd.noBacklight();
      luminosita = 0;
      newcount = 0;
    } else {
      lcd.backlight();
      luminosita = 1;
      newcount = 0;
    }
  }
  
  passato = attuale;
}

void data() {
  lcd.setCursor (0, 0);
  lcd.print(rtc.getDateStr());
  Serial.println(rtc.getDateStr());
  lcd.setCursor (0, 1);
  lcd.print(rtc.getTimeStr());
}

float temp(){
  VRT = analogRead(A0);              //Acquisition analog value of VRT
  VRT = (5.00 / 1023.00) * VRT;      //Conversion to voltage
  VR = VCC - VRT;
  RT = VRT / (VR / R);               //Resistance of RT
  ln = log(RT / RT0);
  TX = (1 / ((ln / B) + (1 / T0))); //Temperature from thermistor
  TX = TX - 273.15;                 //Conversion to Celsius
  return TX;
}
