#include <TimerOne.h>
int buttonPin = 22;
int sensorPin = 12;
int ledPin = 13;
int state = 0; //Stores the status for LED, uses in function callback
const int DOT_LENGTH_LIMIT = 350;
const int END_INPUT_LIMIT = 1000;
const int arrayLength = 41;

long num[] = {12, 2111, 2121, 211, 1, 1121, 221, 1111, 11, 1222, 212, 1211, 22, 21, 222, 1221, 2212, 121, 111, 2, 112, 1112, 122, 2112, 2122, 2211, 22222, 12222, 11222, 11122, 11112, 11111, 21111, 22111, 22211, 22221, 121212, 221122, 112211, 21121, 122121};
String content[] = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", ".", ",", "?", "/", "@"};

void setup() {
  Serial.begin(9600);
  pinMode(sensorPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);

  Timer1.initialize(26);
  Timer1.attachInterrupt(callback);

  testConnection();
  Serial.println("Please start to input");
}

void loop() {
  while (buttonPressed()) { //Button pressed, start receiving data
    Serial.println("Start receiving data");
    Serial.println(dataDeco(dataRecv()));
    Serial.println("Waiting for input...\n\n");
  }
}

void testConnection() {
  Serial.println("Testing connection... Please press the button and hold it until the test ends");
  double timesSent = 0.0;
  double timesReceived = 0.0;
  while (1) {
    if (buttonPressed()) {
      delay(200);
      for (timesSent = 0; timesSent < 100; timesSent++) {
        if (!sensorData())
          timesReceived ++;
        delay(10);
      }
      Serial.println("Test ends, please release the button");
      Serial.print("Data received percentage is  ");
      Serial.println(timesReceived / timesSent);
      break;
    }
  }
}


void callback() {
  if (buttonPressed()) {
    digitalWrite(ledPin, (state) ? HIGH : LOW); //Toggle IR LED
    state = !state;
  }
}

unsigned long dataRecv() {
  unsigned long data = 0;
  do {
    data *= 10;
    //    data += dotDashDecoder(holdTimeCounter());
    data += dotDashDecoder(sensorZeroTimeCounter());
    Serial.print("Data now is  ");
    Serial.println(data);
    //  } while (releaseTimeCounter(END_INPUT_LIMIT) < END_INPUT_LIMIT);
  } while (sensorPositiveTimeCounter(END_INPUT_LIMIT) < END_INPUT_LIMIT);
  Serial.print("Data retrieve end with data  ");
  Serial.println(data);
  return data;

}

String dataDeco(unsigned long dataInput) {
  int searchIndex = 0;
  Serial.println("Searching in array...");
  while (searchIndex <= arrayLength - 1) {
    //    Serial.print("Searching in array for No.");
    //    Serial.println(searchIndex);
    if (dataInput == num[searchIndex]) {
      Serial.print("Result match:  ");
      return content[searchIndex];
    }
    searchIndex ++;
  }
  return "##INPUT DATA ERROR##";
}


unsigned long sensorZeroTimeCounter() {
  unsigned long timePassed = 0;
  while (1) {
    timePassed += 50;
    delay(50);
    if (sensorData()) {       //Error correction
      delay(15);
      if (sensorData()) {
        delay(15);
        if (sensorData()) {
          break;
        }
      }
    }
  }
  return timePassed;
}

unsigned long sensorPositiveTimeCounter(int limit) {
  unsigned long timePassed = 0;
  while (sensorData()) {
    timePassed += 10;
    delay(10);
    if (timePassed >= limit)
      break;
  }
  return timePassed;
}


unsigned int dotDashDecoder(long buttonHoldTime) {
  if (buttonHoldTime > DOT_LENGTH_LIMIT) //A Dash decoded
    return 2;
  else                                   //A Dot decoded
    return 1;
}


boolean buttonPressed() {
  return digitalRead(buttonPin);
}

boolean sensorData() {
  return digitalRead(sensorPin);
}



unsigned long holdTimeCounter() {     //DEBUG USE---WILL COUNT HOLD TIME OF BUTTON---NOT RELATED TO IR SENSOR
  unsigned long timePassed = 0;
  while (buttonPressed()) {
    timePassed += 50;
    delay(50);
  }
  return timePassed;
}

unsigned long releaseTimeCounter(int limit) {     //DEBUG USE---WILL COUNT RELEASE TIME OF BUTTON---NOT RELATED TO IR SENSOR
  unsigned long timePassed = 0;
  while (!buttonPressed()) {
    timePassed += 10;
    delay(10);
    if (timePassed >= limit)
      break;
  }
  return timePassed;
}


