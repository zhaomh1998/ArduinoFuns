const int BETWEEN_ATTEMPT_DELAY_MIN = 500;
const int BETWEEN_ATTEMPT_DELAY_MAX = 3000;
const int LEVEL_1_SPEED = 600;
const int LEVEL_2_SPEED = 450;
const int LEVEL_3_SPEED = 300;
const int LEVEL_4_SPEED = 200;

int switchPin = 5;
int button1Pin = 6;
int button2Pin = 7;
int button3Pin = 8;
int button4Pin = 9;
int led1Pin = 10;
int led2Pin = 11;
int led3Pin = 12;
int led4Pin = 13;

int button[] = {0, 0, 0, 0};

int currentLevelSpeed;
int totalCounter = 0;
int successCounter = 0;
unsigned long gameLastTime = 0;
int mode;
boolean alive = true;

void setup() {
  Serial.begin(9600);
  for (int i = button1Pin; i <= button4Pin; i++)
    pinMode(i, INPUT);
  for (int i = led1Pin; i <= led4Pin; i++)
    pinMode(i, OUTPUT);
  pinMode(switchPin, INPUT);
  mode = digitalRead(switchPin); //Left = 1; Right = 0
  levelSelect();
}

void loop() {
  delay(2000);
  mode = digitalRead(switchPin); //Left = 1; Right = 0
  Serial.print("mode = ");
  Serial.println(mode);
  game();
  resultPrintAndReset();
  clickForNextGame();


}

void game() {
  if (mode == 1) {
    while (gameLastTime <= 10000) {                        //##############TODO change it to 40s
      resetButtonStatus();
      delayBetweenAttempts();
      singleAttemptGame();
    }
  }
  else {
    while (alive) {
      resetButtonStatus();
      delayBetweenAttempts();
      singleAttemptGame();
    }
  }
}

void resultPrintAndReset() {
  if (mode == 1) {
    Serial.print("Game Ended! You got ");
    Serial.print(successCounter);
    Serial.print(" out of total ");
    Serial.println(totalCounter);
    successCounter = 0;
    totalCounter = 0;
    gameLastTime = 0;
    delay(2000);
  }

  else {
    Serial.print("Game Ended! You survived ");
    Serial.print(gameLastTime / 1000);
    Serial.print(" seconds, and got ");
    Serial.print(successCounter);
    Serial.println("moles! Press to restart.");
    successCounter = 0;
    totalCounter = 0;
    gameLastTime = 0;
    delay(2000);
  }

}

void singleAttemptGame() {
  int ledNo = random(led1Pin, led4Pin + 1);              //https://www.arduino.cc/en/Reference/random
  totalCounter ++;
  Serial.print("Total");
  Serial.println(totalCounter);
  digitalWrite(ledNo, HIGH);
  gameLastTime += delayWithButtonReading(currentLevelSpeed);
  digitalWrite(ledNo, LOW);
  if (button[ledNo - 10] == 1) {   //Gotya, mole
    successCounter ++;
    Serial.print("Get");
    Serial.println(successCounter);
    blinkLed(ledNo, 50, 6);
  }

  if (successCounter != totalCounter)
    alive = false;
}



void delayBetweenAttempts() {
  int delayLength = random(BETWEEN_ATTEMPT_DELAY_MIN, BETWEEN_ATTEMPT_DELAY_MAX);
  //  Serial.print("Random delayLength = ");
  //  Serial.println(delayLength);
  gameLastTime += delayLength;
  delay(delayLength);
}

void blinkLed (int led, int dela, int times) {
  for (int i = 1; i <= times; i++) {
    digitalWrite(led, HIGH);
    delay(dela);
    digitalWrite(led, LOW);
    delay(dela);
  }
}

void resetButtonStatus() {
  for (int i = 0; i <= 3; i++)
    button[i] = 0;
}

int delayWithButtonReading(int delayTime) {
  int timeDelayed = 0;
  for (int i = 0; i < delayTime; i += 10) {
    if (digitalRead(button1Pin) == 1) {
      button[0] = 1;
      break;
    }
    else if (digitalRead(button2Pin) == 1) {
      button[1] = 1;
      break;
    }
    else if (digitalRead(button3Pin) == 1) {
      button[2] = 1;
      break;
    }
    else if (digitalRead(button4Pin) == 1) {
      button[3] = 1;
      break;
    }
    else {
      timeDelayed += 10;
      delay(10);
    }
  }

}

void levelSelect() {
  while (1) {
    blinkAll(200);
    if ((digitalRead(button1Pin) == 1)) {
      currentLevelSpeed = LEVEL_1_SPEED;
      break;
    }
    else if ((digitalRead(button2Pin) == 1)) {
      currentLevelSpeed = LEVEL_2_SPEED;
      break;
    }
    else if ((digitalRead(button3Pin) == 1)) {
      currentLevelSpeed = LEVEL_3_SPEED;
      break;
    }
    else if ((digitalRead(button4Pin) == 1)) {
      currentLevelSpeed = LEVEL_4_SPEED;
      break;
    }
  }
}

void clickForNextGame() {
  while (digitalRead(button1Pin) == 0 && digitalRead(button2Pin) == 0 && digitalRead(button3Pin) == 0 && digitalRead(button4Pin) == 0)
    blinkAll(200);
}

void blinkAll(int delayTime) {
  digitalWrite(led1Pin, HIGH);
  digitalWrite(led2Pin, HIGH);
  digitalWrite(led3Pin, HIGH);
  digitalWrite(led4Pin, HIGH);
  delay(delayTime);
  digitalWrite(led1Pin, LOW);
  digitalWrite(led2Pin, LOW);
  digitalWrite(led3Pin, LOW);
  digitalWrite(led4Pin, LOW);
  delay(delayTime);
}
