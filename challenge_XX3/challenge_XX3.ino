const int BETWEEN_ATTEMPT_DELAY_MIN = 500;
const int BETWEEN_ATTEMPT_DELAY_MAX = 3000;
const int LEVEL_1_SPEED = 600;
const int LEVEL_2_SPEED = 450;
const int LEVEL_3_SPEED = 300;
const int LEVEL_4_SPEED = 200;
const int GAME_LENGTH = 20000;  //Length of each game (in milliseconds) in limited time mode

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
  for (int i = button1Pin; i <= button4Pin; i++)  //Setting button pin
    pinMode(i, INPUT);
  for (int i = led1Pin; i <= led4Pin; i++)        //Setting LED pin
    pinMode(i, OUTPUT);
  pinMode(switchPin, INPUT);                      //Setting the pin to the switch
  mode = digitalRead(switchPin);                  //Switch to Left = 1; to Right = 0
  levelSelect();                                  //Run level selection for user before start game
}

void loop() {
  delay(2000);                   //Delay before firt "mole" come out, to give player some time to react after they press restart/ selected level
  mode = digitalRead(switchPin); //Left = 1; Right = 0
//  Serial.print("mode = ");
//  Serial.println(mode);
  game();
  resultPrintAndReset();
  clickForNextGame();


}

void game() {
  if (mode == 1) {
    while (gameLastTime <= GAME_LENGTH) {
      resetButtonStatus();
      delayBetweenAttempts();
      singleAttemptGame();
    }
  }
  else {
    while (alive) {                 //Continue the loop until miss a "mole", which let the alive = false
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
//  Serial.print("Total");
//  Serial.println(totalCounter);  //Debug use
  digitalWrite(ledNo, HIGH);
  gameLastTime += delayWithButtonReading(currentLevelSpeed);
  digitalWrite(ledNo, LOW);
  if (button[ledNo - led1Pin] == 1) {   //Gotya, mole (ledNo - led1Pin means the corresponding index in array button, for example, led1 is the first element, index == 0 so when the mole is on LED1, ledNo = led1Pin, so that ledNo - led1Pin == 0)
    successCounter ++;
//    Serial.print("Get");
//    Serial.println(successCounter);   //Debug use
    blinkLed(ledNo, 50, 6);
  }

  if (successCounter != totalCounter) //Once one mole is missed, that caused success mole number != total mole number, set alive = false to end this game
    alive = false;
}



void delayBetweenAttempts() {
  int delayLength = random(BETWEEN_ATTEMPT_DELAY_MIN, BETWEEN_ATTEMPT_DELAY_MAX);
  //  Serial.print("Random delayLength = ");
  //  Serial.println(delayLength);     //Debug use
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
