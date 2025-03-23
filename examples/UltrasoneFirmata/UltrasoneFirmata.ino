/*
 * Firmata is a generic protocol for communicating with microcontrollers
 * from software on a host computer. It is intended to work with
 * any host computer software package.
 *
 * To download a host software package, please click on the following link
 * to open the list of Firmata client libraries in your default browser.
 *
 * https://github.com/firmata/arduino#firmata-client-libraries
 */

#include <Firmata.h>

typedef struct {
  bool some;
  uint8_t pin;
} option;

option trigPin = {false, 0};
option echoPin = {false, 0};

void sendUltrasoneData(uint8_t distance) {
  if (trigPin.some == false || echoPin.some == false) {
    return;
  }
  Firmata.sendUltrasoneDistance(echoPin.pin, distance);
}

uint8_t read_ultrasone() {
  if (trigPin.some == false || echoPin.some == false) {
    return 255;
  }

  digitalWrite(trigPin.pin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin.pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin.pin, LOW);

  float duration = pulseIn(echoPin.pin, HIGH);
  // speed of sound is 343 m/s
  // The duration is in microseconds.
  // duration * 0.0343 gives us the total travel distance in cm
  // The sound has to travel twice the distance of where the object is.
  // The time from the sensor to the object and the time back to the sensor,
  // so devide the result by 2 to get the distance from the sensor to the object.
  uint16_t distance = (uint16_t)((duration * .0343) / 2);
  return (uint8_t)min(distance, 255);
}

void setTrigPin(uint8_t pin, int _) {
  pinMode(A0, OUTPUT);
  trigPin.pin = pin;
  trigPin.some = true;
  pinMode(trigPin.pin, OUTPUT);
}

void setEchoPin(uint8_t pin, int _) {
  echoPin.pin = pin;
  echoPin.some = true;
  pinMode(echoPin.pin, INPUT);
  pinMode(A0, OUTPUT);
}

void setup() {
  Firmata.setFirmwareVersion(FIRMATA_FIRMWARE_MAJOR_VERSION, FIRMATA_FIRMWARE_MINOR_VERSION);
  Firmata.attach(SET_TRIG_MESSAGE, setTrigPin);
  Firmata.attach(SET_ECHO_MESSAGE, setEchoPin);
  Firmata.begin(57600);
}

long long lastUltrasoneSendTimems = 0;
uint16_t ultrasoneSendDelayms = 100;

void loop() {

  while (Firmata.available()) {
    Firmata.processInput();
  }

  //send ultrasone data once in the 500ms
  long long timeNow = millis();
  if (timeNow - lastUltrasoneSendTimems > ultrasoneSendDelayms) {
    uint8_t distance = read_ultrasone();
    sendUltrasoneData(distance);
    lastUltrasoneSendTimems = timeNow;
  }
}
