# Reptile Thermostat

A thermostat for a terrarium made with Arduino and a **3D-printed case**.

<p align="center">
  <img src="/img/large_display_thermostat_1.jpg" alt="Thermostat Front View" width="500">
</p>

## Components (1x each)
- **Arduino Board** (Uno/Nano)
- **DHT11 Sensor** (Temperature & Humidity)
- **Relay Module** (For controlling heating elements)
- **I2C LCD Display** (16x2)
- **Buzzer** (For alerts)
- **LED** (Indicator light)
- **Rotary Encoder with Push Button** (For navigation)
- **DS1302 RTC Clock Module** (For timekeeping)

### **Extra Parts**
- Insert nuts **(M2/M4)**
- Screws **(M2/M4)**

## **Wiring Diagram**
Below is the wiring setup for connecting the components:

| Component       | Arduino Pin  | Notes |
|----------------|-------------|--------------------------|
| **DHT11**      | **D2**       | Data pin for temperature & humidity |
| **LCD (I2C)**  | **A4 (SDA), A5 (SCL)** | Connect I2C lines |
| **RTC DS1302** | **D8 (CE), D9 (I/O), D10 (SCLK)** | Real-time clock module |
| **Encoder A/B**| **D11, D12** | Rotary encoder inputs |
| **Push Button**| **D3** | For navigation |
| **Relay**      | **D7**       | Controls heating elements |
| **Buzzer**     | **D6**       | Alarm notification |

---

<p align="center">
  <img src="/img/large_display_Dht11.jpg" alt="DHT11 Setup" width="500">
</p>

<p align="center">
  <img src="/img/large_display_thermostat__2.jpg" alt="Thermostat Internal View" width="500">
</p>

---

## **3D Files**
Download the **3D-printable case** from:  
📌 [Printables - Reptile Thermostat](https://www.printables.com/model/1199039-reptile-thermostat)

---

This thermostat **automatically regulates temperature** based on the DHT11 sensor readings, switching the **relay on/off** depending on **day/night temperature thresholds**. The **encoder** allows easy adjustments, and the **LCD displays real-time values**.

---
