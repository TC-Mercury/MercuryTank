# MercuryTank - Smart Tank Level & Volume Monitoring System

MercuryTank is an Arduino-based embedded application designed to measure liquid levels in horizontal cylindrical tanks using an ultrasonic sensor (HC-SR04). It implements real-time volume calculations based on circular segment geometry and advanced signal processing.

## Key Features

* **Advanced Signal Processing:**
  * **Median Filter:** Samples 7 consecutive readings and selects the median to eliminate "spike" errors and sensor noise.
  * **EMA (Exponential Moving Average):** Utilizes an alpha-based smoothing algorithm (`EMA_ALPHA = 0.28`) to dampen liquid surface ripples and provide stable output.
* **Precise Geometric Modeling:** Liquid volume in horizontal cylinders does not change linearly with height. This project calculates the exact volume using the circular segment area formula:
  
  $$Area = R^2 \cdot \cos^{-1}\left(\frac{R-h}{R}\right) - (R-h)\sqrt{2Rh - h^2}$$

* **Robust Error Handling:** Employs `clampf` and `maxf` functions to filter out-of-bounds readings and prevent mathematical errors.
* **User Interface:** Real-time monitoring of height (cm) and fill percentage (%) via an I2C 16x2 LCD display.

## Hardware Requirements
* Arduino Uno / Nano / Mega
* HC-SR04 Ultrasonic Distance Sensor
* 16x2 I2C LCD Display
* Jumper Wires and Breadboard



## Pin Mapping
| Component | Arduino Pin |
| :--- | :--- |
| HC-SR04 Trig | Pin 9 |
| HC-SR04 Echo | Pin 8 |
| LCD SDA | A4 (Uno) |
| LCD SCL | A5 (Uno) |

## Configuration
You can customize the tank dimensions in the `src/MercuryTank.ino` file to match your specific hardware:
```cpp
const float R = 50.0;  // Tank Radius (cm)
const float L = 200.0; // Tank Length (cm)
