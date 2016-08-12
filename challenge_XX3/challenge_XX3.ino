//------------------------------------Game Settings-------------------------------------------------------
const int BETWEEN_ATTEMPT_DELAY_MIN = 500;
const int BETWEEN_ATTEMPT_DELAY_MAX = 3000;
const int LEVEL_1_SPEED = 600;
const int LEVEL_2_SPEED = 450;
const int LEVEL_3_SPEED = 300;
const int LEVEL_4_SPEED = 200;
const int GAME_LENGTH = 20000;  //Length of each game (in milliseconds) in limited time mode
//------------------------------------Defining Variables-------------------------------------------------------
int switchPin = 5;   //Pin for the switch to switch between two modes
int button1Pin = 6;
int button2Pin = 7;
int button3Pin = 8;
int button4Pin = 9;
int led1Pin = 10;
int led2Pin = 11;
int led3Pin = 12;
int led4Pin = 13;

int button[] = {0, 0, 0, 0};  //An array that stores when the specific button is pressed, the data on the corresponding index will be changed to 1

int currentLevelSpeed;        //Stores the speed for the current level selected
int totalCounter = 0;         //Stores the total "mole" come out
int successCounter = 0;       //Stores the number of "mole" player got
unsigned long gameLastTime = 0;  //Stores the time passed in that round of the game in milliseconds
int mode;                        //Stores the mode (output of the switch)
boolean alive = true;            //In survival mode, after player missed one "mole", this variable will be set to false

//----------------------------------Game Initiation-----------------------------------------------------------------
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
//-----------------------------------Game Loop-----------------------------------------------------------------------
void loop() {
  delay(2000);                   //Delay before firt "mole" come out, to give player some time to react after they press restart/ selected level
  mode = digitalRead(switchPin); //Left = 1; Right = 0
//  Serial.print("mode = ");     //Print what mode is selected. Switch debug use.
//  Serial.println(mode);
  game();
  resultPrintAndReset();
  clickForNextGame();


}
//-----------------------------Modular Functions--------------------------------------------------------------
void game() {
  if (mode == 1) {     //If the switch is reading 1 (put to the left), timing mode is selected, run game in timing mode
    while (gameLastTime <= GAME_LENGTH) {   //With in the time set in the game settings, game continue looping
      resetButtonStatus();                  //Reset the button array (which stores the button status before)
      delayBetweenAttempts();               //Random delay between two "moles" come out
      singleAttemptGame();                  //The game for single "mole" comming out and read button hitting, etc. See comments in the function for more details
    }
  }
  else {               //If the switch is reading 0 (put to the right), survival mode is selected, run game in survival mode
    while (alive) {                 //Continue the loop until miss a "mole", which let the alive = false
      resetButtonStatus();          //Apart from the requirements for game to continue (the parameter in the while function), other gaming process are the same
      delayBetweenAttempts();
      singleAttemptGame();
    }
  }
}

void levelSelect() {
  while (1) {                        //Blink all LEDs until a button is pressed
    blinkAll(200);
    if ((digitalRead(button1Pin) == 1)) {
      currentLevelSpeed = LEVEL_1_SPEED;    //Change the current level speed to the level 1 speed set as game settings
      break;                                //Break out of the while loop
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


void resultPrintAndReset() {
  if (mode == 1) {                               //In timing mode, print result this way
    Serial.print("Game Ended! You got ");
    Serial.print(successCounter);
    Serial.print(" out of total ");
    Serial.println(totalCounter);
    successCounter = 0;                         //Clear and Reset the counter after the game ended
    totalCounter = 0;
    gameLastTime = 0;
    delay(2000);                              //Give some time for the player to rest before next game
  }

  else {                                      //In survival mode, print result this way
    Serial.print("Game Ended! You survived ");
    Serial.print(gameLastTime / 1000);
    Serial.print(" seconds, and got ");
    Serial.print(successCounter);
    Serial.println("moles! Press to restart.");
    successCounter = 0;                        //Clear and Reset the counter after the game ended
    totalCounter = 0;
    gameLastTime = 0;
    delay(2000);                              //Give some time for the player to rest before next game
  }

}

void singleAttemptGame() {
  int ledNo = random(led1Pin, led4Pin + 1);       //Choose a random LED pin number between LED1's pin and LED4's pin. See https://www.arduino.cc/en/Reference/random for more details
  totalCounter ++;                                //Add one to the variable that stores the total number of "moles" came out
//  Serial.print("Total");
//  Serial.println(totalCounter);  //Debug use
  digitalWrite(ledNo, HIGH);                      //Turn on the LED for the specific "mole"
  gameLastTime += delayWithButtonReading(currentLevelSpeed);  //Add the delay time to the gameLastTime, and delay. This statement will run function delayWithButtonReading(...), the function will return a value for the time spent on this process. Delay is included in this function. See this function below for more detail.
  digitalWrite(ledNo, LOW);                       //After the delay, turn the specific LED off
  if (button[ledNo - led1Pin] == 1) {   //Read the array button to see if the mole is captured. The array will be changed during executing the function delayWithButtonReading(...), once a button is pressed (ledNo - led1Pin means the corresponding index in array button, for example, led1 is the first element, index == 0 so when the mole is on LED1, ledNo = led1Pin, so that ledNo - led1Pin == 0)
    successCounter ++;                            //Add one to the variable that stores the total number of "moles" successfully got
//    Serial.print("Get");
//    Serial.println(successCounter);   //Debug use
    blinkLed(ledNo, 50, 6);                       //Blink the "mole" six times to indicate that "mole" is got
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
