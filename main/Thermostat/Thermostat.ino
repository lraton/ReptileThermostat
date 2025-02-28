/*
 * Copyright (c) 2025, lraton 
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
#include <DHT.h>
#include <DHT_U.h>
#include <DS1302.h>
#include <Wire.h>

#define DHTPIN 2       // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11  // Using DHT11 temperature and humidity sensor
#define encoderA 11    // Encoder pin A
#define encoderB 12    // Encoder pin B
#define relay 7        // Relay pin
#define buzzer 6       // Buzzer pin
#define button 3       // Button pin
#define rtc_ena 8      // RTC en pin
#define rtc_clk 9      // RTC clk pin
#define rtc_dat 10     // RTC dat pin

// Variables for encoder control
int newCount = 0;
int oldEncoder = 0;
int newEncoder = 1;

int lastRead = HIGH;

// Temperature threshold values
int maxDay = 32;          // Maximum day temperature
int minDay = maxDay - 3;  // Minimum day temperature

int maxNight = 25;            // Maximum night temperature
int minNight = maxNight - 3;  // Minimum night temperature

int displayLuminosity = 1;  // Variable to track display backlight state

// Initialize LCD with I2C address 0x27 (16x2 characters)
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHTPIN, DHTTYPE);
DS1302 rtc(rtc_ena, rtc_clk, rtc_dat);

void setup() {
  Serial.begin(9600);

  lcd.begin();
  lcd.backlight();
  rtc.halt(false);
  rtc.writeProtect(false);


  dht.begin();

  // Button and output pin configurations
  pinMode(button, INPUT_PULLUP);    // Page change button
  pinMode(buzzer, OUTPUT);          // Buzzer output
  pinMode(relay, OUTPUT);           // Relay output
  pinMode(encoderA, INPUT_PULLUP);  // Encoder input A
  pinMode(encoderB, INPUT_PULLUP);  // Encoder input B
}

void loop() {
  newEncoder = digitalRead(button);
  if (newEncoder == LOW && oldEncoder == HIGH) {
    if (newCount == 3) {
      newCount = 0;
    } else {
      newCount++;
      lcd.clear();
    }
  }

  switch (newCount) {
    case 0:
      firstMenu();
      break;
    case 1:
      secondMenu();
      break;
    case 2:
      thirdMenu();
      break;
    case 3:
      fourthMenu();
      break;
  }

  oldEncoder = newEncoder;
}

void firstMenu() {
  delay(250);
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  int state = digitalRead(relay);

  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(t);
  lcd.setCursor(11, 0);
  lcd.print((char)223);

  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(h);
  lcd.setCursor(15, 1);
  lcd.print("%");

  // Check temp error
  lcd.setCursor(13, 0);
  if (isnan(t)) {
    digitalWrite(buzzer, HIGH);  // Turn on buzzer on temperature error
    lcd.print("ERR");
  } else {
    digitalWrite(buzzer, LOW);  // Turn off buzzer
    //Check for day or night
    if ((rtc.getTime().hour < 23) && (rtc.getTime().hour > 8)) {
      // Daytime temperature control
      Serial.println("Day mode");
      if (t < minDay) {
        digitalWrite(relay, HIGH);  // Turn on relay and led
        lcd.setCursor(13, 0);
        lcd.print(" ON");
      } else if (t > maxDay) {
        digitalWrite(relay, LOW);  // Turn off relay and led
        lcd.setCursor(13, 0);
        lcd.print("OFF");
      } else {
        lcd.setCursor(13, 0);
        lcd.print(state == HIGH ? " ON" : "OFF");
      }
    } else {
      // Nighttime temperature control
      Serial.println("Night mode");
      if (t < minNight) {
        digitalWrite(relay, HIGH);  // Turn on relay and led
        lcd.setCursor(13, 0);
        lcd.print(" ON");
      } else if (t > maxNight) {
        digitalWrite(relay, LOW);  // Turn off relay and led
        lcd.setCursor(13, 0);
        lcd.print("OFF");
      } else {
        lcd.setCursor(13, 0);
        lcd.print(state == HIGH ? " ON" : "OFF");
      }
    }
  }
}

void secondMenu() {
  int n = digitalRead(encoderA);
  if ((lastRead == HIGH) && (n == LOW)) {
    if (digitalRead(encoderB) == HIGH) {
      maxDay--;
      minDay--;
    } else {
      maxDay++;
      minDay++;
    }
  }
  digitalWrite(buzzer, LOW);
  lcd.setCursor(6, 0);
  lcd.print(maxDay);
  lcd.setCursor(9, 0);
  lcd.print((char)223);
  lcd.setCursor(0, 1);
  lcd.print("<-(-)");
  lcd.setCursor(11, 1);
  lcd.print("(+)->");
  lastRead = n;
}

void thirdMenu() {
  lcd.setCursor(0, 0);
  lcd.print(rtc.getDateStr());
  Serial.println(rtc.getDateStr());
  lcd.setCursor(0, 1);
  lcd.print(rtc.getTimeStr());
}

void fourthMenu() {
  lcd.clear();
  if (displayLuminosity == 1) {
    lcd.noBacklight();
    displayLuminosity = 0;
  } else {
    lcd.backlight();
    displayLuminosity = 1;
  }
  newCount = 0;  // Reset to first menu
}
