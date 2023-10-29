// Otto Karppinen
// Harjoitus 3
// 22.10.2023

#include "Adafruit_SHTC3.h"
Adafruit_SHTC3 shtc3 = Adafruit_SHTC3();

#include "Adafruit_VL53L1X.h"
#define IRQ_PIN 12
#define XSHUT_PIN 11
Adafruit_VL53L1X vl53 = Adafruit_VL53L1X(XSHUT_PIN, IRQ_PIN);

const int ledRed = 7;   // LED connected to digital pin 7
const int Button1 = 4;  // pushbutton for acknowledging the alert
const int Button2 = 3;  // pushbutton for temperature and humidity measurement

volatile bool buttonB2Pressed = false;
volatile bool buttonB1Pressed = false;
volatile bool alertTriggered = false;

void setup() {
  Serial.begin(115200);

  pinMode(ledRed, OUTPUT);
  pinMode(Button1, INPUT_PULLUP);
  pinMode(Button2, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(Button2), handleB2Press, FALLING);
  attachInterrupt(digitalPinToInterrupt(Button1), handleB1Press, FALLING);

  if (!shtc3.begin() || !vl53.begin()) {
    Serial.println("Sensorin alustus epäonnistui!");
    while (1);
  }
}

void loop() {
  if (buttonB2Pressed) {
    buttonB2Pressed = false;

    sensors_event_t humidity, temp;
    shtc3.getEvent(&humidity, &temp);

    float temperature = temp.temperature;
    float humidity_value = humidity.relative_humidity;

    Serial.printf("Lämpötila on %.1f °C ja suhteellinen kosteus on %.0f%%\n", temperature, humidity_value);
  }

  if (vl53.dataReady()) {
    int16_t distance = vl53.distance();

    if (distance < 200 && distance != -1 && !alertTriggered) {  
      alertTriggered = true;
      digitalWrite(ledRed, HIGH);
      Serial.println("H Ä L Y T Y S !!!");
    }

    vl53.clearInterrupt();
  }
  
  if (buttonB1Pressed && alertTriggered) {
    buttonB1Pressed = false;
    alertTriggered = false;
    digitalWrite(ledRed, LOW);
    Serial.println("Hälytys kuitattu");
  }
}

void handleB2Press() {
  digitalWrite(ledRed, HIGH);
  delay(500);  // Not recommended to use delay inside an ISR in real applications
  digitalWrite(ledRed, LOW);
  buttonB2Pressed = true;
}

void handleB1Press() {
  buttonB1Pressed = true;
}
