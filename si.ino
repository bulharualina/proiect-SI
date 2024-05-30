#include <Servo.h> // Include biblioteca Servo
#include <Wire.h> // Include biblioteca Wire pentru comunicare I2C
#include <LiquidCrystal_I2C.h> // Include biblioteca LiquidCrystal_I2C pentru LCD

// Definire pinii pentru servomotoare și senzori ultrasonici
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

// Definire pinii pentru LED-uri
#define rosu 8
#define verde 2

// Definire distanțele minime și maxime
#define MIN_DISTANCE 3.0
#define MAX_DISTANCE 12.0


LiquidCrystal_I2C lcd_1(0x27, 16, 2); // Inițializare LCD cu adresa I2C 0x27, 16 coloane și 2 rânduri

Servo servo1, servo2; // Creare obiecte pentru servomotoare

int nr_locuri = 5; // Variabilă pentru a urmări locurile de parcare disponibile

// Funcție pentru a verifica dacă valoarea distanței este într-un interval valid
bool checkValue(long value) {
  return value >= 0.0 && value <= 500.0;
}

// Funcție pentru a obține distanța de la un senzor ultrasonic
long getSensorDistance(int trigPin, int echoPin) {
  long duration, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 2) / 29.1; // Conversie a timpului în distanță
  return distance;
}

void setup() {
  Serial.begin(9600); // Pornire comunicare serială la 9600 baud
  
  // Inițializare senzori ultrasonici
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  pinMode(trigPin3, OUTPUT);
  pinMode(echoPin3, INPUT);
  pinMode(trigPin4, OUTPUT);
  pinMode(echoPin4, INPUT);

  // Inițializare LED-uri
  pinMode(rosu, OUTPUT);
  pinMode(verde, OUTPUT);

  // Inițializare LCD
  lcd_1.begin();
  lcd_1.clear();
  lcd_1.backlight();

  // Atașare și setare poziții inițiale pentru servomotoare
  servo1.attach(servoPin1);
  servo1.write(0); // Bariera intrare inchisa
  servo2.attach(servoPin2);
  servo2.write(0); // Bariera iesire inchisa
}

void loop() {

   // Obținerea distanțelor de la senzori
  long intrare1 = getSensorDistance(trigPin1, echoPin1);
  long intrare2 = getSensorDistance(trigPin2, echoPin2);
  long iesire1 = getSensorDistance(trigPin3, echoPin3);
  long iesire2 = getSensorDistance(trigPin4, echoPin4);

  // Afișare număr locuri de parcare disponibile
  print_locuri_libere(nr_locuri);
  
  // Verificare senzori de intrare
  if (checkValue(intrare1)) {
    bariera_intrare(intrare1, intrare2);
  }

  // Verificare senzori de ieșire
  if (checkValue(iesire1)) {
    bariera_iesire(iesire1, iesire2);
  }

  delay(100); 
}

// Funcție pentru controlul barierei de intrare
void bariera_intrare(long intrare1, long intrare2) {
  if (nr_locuri > 0) { // Dacă sunt locuri disponibile
     digitalWrite(rosu,LOW); // Oprire LED roșu
    digitalWrite(verde,HIGH);  // Pornire LED verde
    
    if (intrare1 > MIN_DISTANCE && intrare1 <= MAX_DISTANCE) {
      Serial.println("Bariera ridicata pentru intare!");
      servo1.write(90); // Ridicare bariera
      delay(1000);
    }

    if (intrare2 > MIN_DISTANCE && intrare2 <= MAX_DISTANCE) {
      Serial.println("Bariera coborata dupa intrare!");
      servo1.write(0);  // Coborâre bariera
      nr_locuri--; //Decrementare număr locuri disponibile
      delay(1500);
    }
  } else {// Dacă nu sunt locuri disponibile
    digitalWrite(verde,LOW);  // Oprire LED verde
    digitalWrite(rosu,HIGH);  // Pornire LED roșu
    servo1.write(0);  // Asigurare că bariera este jos
  }
}

// Funcție pentru controlul barierei de ieșire
void bariera_iesire(long iesire1, long iesire2) {
  if (nr_locuri < 5) {  // Dacă parcarea nu este plină
    if (iesire1 > MIN_DISTANCE && iesire1 <= MAX_DISTANCE) {
      Serial.println("Bariera ridicata pentru iesire!");
      servo2.write(90);  // Ridicare bariera
      delay(1200);
    }

    if (iesire2 > MIN_DISTANCE && iesire2 <= MAX_DISTANCE) {
      Serial.println("Bariera coborata dupa iesire!");
      servo2.write(0);  // Coborâre bariera
      nr_locuri++;  // Creștere număr locuri disponibile
      delay(1200);
    }
  }
}

// Funcție pentru afișarea numărului de locuri de parcare disponibile pe LCD
void print_locuri_libere(int nr_locuri) {
  lcd_1.clear();
  lcd_1.setCursor(0, 0);
  lcd_1.print("Bine ati venit!");
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

