#include <Wire.h>
#include <math.h>
#include <LiquidCrystal_I2C.h>

const int trig_pin = 9;
const int echo_pin = 8;

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Tank Sizes
const float R = 50.0;
const float L = 200.0;
const float H = 2.0 * R;

// Sensor parameters
const int SAMPLES = 7;
const float SPEED_CM_PER_US = 0.034;
const float CALIBRATION = 1.00;
const float EMA_ALPHA = 0.28;

float emaDistance = -1.0;

float clampf(float x, float a, float b) { if (x < a) return a; if (x > b) return b; return x; }
float maxf(float a, float b) { return (a > b) ? a : b; }

unsigned long singlePulseTime() {
  digitalWrite(trig_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_pin, LOW);
  unsigned long t = pulseIn(echo_pin, HIGH, 30000UL);
  return t;
}

float getDistanceMedian() {
  unsigned long arr[SAMPLES];
  int got = 0;
  int attempts = 0;

  while (got < SAMPLES && attempts < SAMPLES * 3) {
    unsigned long t = singlePulseTime();
    if (t > 0) {
      arr[got++] = t;
    }
    attempts++;
    delay(20);
  }
  if (got == 0) return 0.0;

  // insertion sort
  for (int i = 1; i < got; ++i) {
    unsigned long key = arr[i];
    int j = i - 1;
    while (j >= 0 && arr[j] > key) {
      arr[j + 1] = arr[j];
      j--;
    }
    arr[j + 1] = key;
  }

  unsigned long medianTime = arr[got / 2];
  float dist = (medianTime * SPEED_CM_PER_US) / 2.0 * CALIBRATION;
  return dist;
}

float getSmoothedDistance() {
  float d = getDistanceMedian();
  if (d <= 0.0) return -1.0;
  if (emaDistance < 0.0) {
    emaDistance = d;
  } else {
    emaDistance = EMA_ALPHA * d + (1.0 - EMA_ALPHA) * emaDistance;
  }
  return emaDistance;
}

void setup() {
  pinMode(trig_pin, OUTPUT);
  pinMode(echo_pin, INPUT);

  lcd.init();
  lcd.backlight();
  lcd.print("Tank sistemi");
  delay(2000); // 2 sn göster
  lcd.clear();
}

void loop() {
  float dist = getSmoothedDistance();
  if (dist < 0) {
    lcd.setCursor(0, 0);
    lcd.print("Olcum hatasi!  ");
    delay(500);
    return;
  }

  float h = H - dist;
  h = clampf(h, 0.0, H);

  float arg = clampf((R - h) / R, -1.0, 1.0);
  float area = R * R * acos(arg) - (R - h) * sqrt(maxf(0.0, 2.0 * R * h - h * h));
  float liters = (area * L) / 1000.0;

  float maxArea = R * R * acos(-1.0) - (R - H) * sqrt(maxf(0.0, 2.0 * R * H - H * H));
  float maxLiter = (maxArea * L) / 1000.0;

  float perc = (liters / maxLiter) * 100.0;

  lcd.setCursor(0, 0);
  lcd.print("Yukseklik:");
  lcd.print(h, 1);
  lcd.print("cm   ");

  lcd.setCursor(0, 1);
  lcd.print("Doluluk: ");
  lcd.print(perc, 1);
  lcd.print("%   ");

  delay(700);
}
