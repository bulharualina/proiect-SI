#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define servoPin1 9
#define servoPin2 3

#define trigPin1 13
#define echoPin1 12

#define trigPin2 11
#define echoPin2 10

#define trigPin3 7
#define echoPin3 6

#define trigPin4 5
#define echoPin4 4

#define MIN_DISTANCE 3.0
#define MAX_DISTANCE 12.0

LiquidCrystal_I2C lcd_1(0x27, 16, 2);
Servo servo1, servo2;

int nr_locuri = 5;

bool checkValue(long value) {
  return value >= 0.0 && value <= 500.0;
}

long getSensorDistance(int trigPin, int echoPin) {
  long duration, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 2) / 29.1;
  return distance;
}

void setup() {
  Serial.begin(9600);
  
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  pinMode(trigPin3, OUTPUT);
  pinMode(echoPin3, INPUT);
  pinMode(trigPin4, OUTPUT);
  pinMode(echoPin4, INPUT);

  // LCD
  lcd_1.begin();
  lcd_1.clear();
  lcd_1.backlight();

  // Servomotors
  servo1.attach(servoPin1);
  servo1.write(180);
  servo2.attach(servoPin2);
  servo2.write(180);
}

void loop() {
  long intrare1 = getSensorDistance(trigPin1, echoPin1);
  long intrare2 = getSensorDistance(trigPin2, echoPin2);
  long iesire1 = getSensorDistance(trigPin3, echoPin3);
  long iesire2 = getSensorDistance(trigPin4, echoPin4);

  print_locuri_libere(nr_locuri);
  
  if (checkValue(intrare1)) {
    bariera_intrare(intrare1, intrare2);
  }

  if (checkValue(iesire1)) {
    bariera_iesire(iesire1, iesire2);
  }

  delay(100); // A small delay to allow the serial monitor to keep up
}

void bariera_intrare(long intrare1, long intrare2) {
  if (nr_locuri > 0) {
    if (intrare1 > MIN_DISTANCE && intrare1 <= MAX_DISTANCE) {
      Serial.println("Bariera ridicata pentru intare!");
      servo1.write(90);
      delay(1000);
    }

    if (intrare2 > MIN_DISTANCE && intrare2 <= MAX_DISTANCE) {
      Serial.println("Bariera coborata dupa intrare!");
      servo1.write(180);
      nr_locuri--;
      delay(1500);
    }
  } else {
    servo1.write(180);
  }
}

void bariera_iesire(long iesire1, long iesire2) {
  if (nr_locuri < 10) {
    if (iesire1 > MIN_DISTANCE && iesire1 <= MAX_DISTANCE) {
      Serial.println("Bariera ridicata pentru iesire!");
      servo2.write(90);
      delay(1200);
    }

    if (iesire2 > MIN_DISTANCE && iesire2 <= MAX_DISTANCE) {
      Serial.println("Bariera coborata dupa iesire!");
      servo2.write(180);
      nr_locuri++;
      delay(1200);
    }
  }
}

void print_locuri_libere(int nr_locuri) {
  lcd_1.clear();
  if (nr_locuri > 0) {
    lcd_1.setCursor(0, 1);
    lcd_1.print(nr_locuri);
    lcd_1.print(" loc. disponibile");
  } else {
    lcd_1.setCursor(0, 1);
    lcd_1.print("Parcare plina!");
    delay(500);
  }
}

void print_text(long firstSensor, long secondSensor, long threeSensor, long fourSensor) {
  Serial.println();
  Serial.print("L1:");
  Serial.print(firstSensor);
  Serial.print(" cm");
  Serial.print(" R1:");
  Serial.print(secondSensor);
  Serial.print(" cm");
  Serial.print(" L2:");
  Serial.print(threeSensor);
  Serial.print(" cm");
  Serial.print(" R2:");
  Serial.print(fourSensor);
  Serial.println(" cm");
}
