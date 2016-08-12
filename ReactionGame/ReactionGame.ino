//#####Reaction Game on Arduino#####
//#####Designed by Karcher & Scott, Coded by Scott.#####
/**
 * PLAYER INSTRUCTIONS
 * Step1 Select level: An LED will be blinking indicating the current speed of that level. Click on the button to step into next level. Hold the button for 1.5 seconds to select that level.
 * Step2 Game: when all LEDs turn off one by one, the game starts. Set a specific LED in your mind, and try to click (AND HOLD) (Reason explained in the comment for the function on buttom of this program)
 the button as that specific LED light up. Go ahead and see how close you can get to that LED! Game will automatically restarts for the same level
 Step3 Change level: Click the the tiny red button on the arduino board to reset arduino
 
 */
// -----GAME SETTING----------------------------------------------
const double DEFAULTSPEED = 350.0; //Speed for level 1
const double MULTIPLE = 0.66;  //multiple for steeping to next level
//------Variables------------------------------------------------
int led1Pin = 1;
int led2Pin = 2;
int led3Pin = 3;
int led4Pin = 4;
int led5Pin = 5;
int led6Pin = 6;
int led7Pin = 7;
int buttonPin = 13;
int stopp = 0;  //Variable that indicates the button is pressed, game stop.
int ledCount = 1;  //Variable storing the current LED number flashing during the game
double speedd = DEFAULTSPEED;
//-------System Setup-----------------------------------------
void setup() {
  for (int i = led1Pin; i <= led7Pin; i++)    // Setting up LED pins to OUTPUT
    pinMode(i, OUTPUT);
  pinMode(buttonPin, INPUT);
  setLevel();
}
//------System Loop------------------------------------------
void loop() {
  game();
}
//------Modular functions--------------------------------

void game() {
  reset();
  prepare();
  while (stopp == 0) {
    gamingLoop();
  }
  resultDisplay();
}

void setLevel() { //No.1 Set level
  int led = led1Pin;
  while (1) {
    flash(led, speedd);
    if (digitalRead(buttonPin) == 1) {     //If button pressed
      int holdTime = buttonHoldTime(1500); //Retrieve button hold time
      if (holdTime == 1500) {          //If reach maximum time limit, meaning that the current level is chosen
        break;                         //Lvl set complete. Break out of the loop and start the game!
      }
      else {            //Going to next level
        if (led == led7Pin) { //If its already the last level (level.7), go back to level.1 and reset speed
          led = led1Pin;
          speedd = DEFAULTSPEED;
        }
        else {
          speedd *= MULTIPLE; //Changing the speed to next level's
          led++; //Changing level indicator to next one
        }
      }
    }
    else {
      delay(speedd);  //Blinking led for current level
    }
  }
}
void reset() {    //-----No.2 Reset variables
  stopp = 0;
  ledCount = led1Pin;
}

void prepare() {  //-----No.3 User ready
  blinkAll(1000, 400);
  setAll(HIGH);                  //Turn on all LEDS
  for (int i = led7Pin; i >= led1Pin; i--) {  //Turn them off one by one to indicate the game is about to start
    digitalWrite(i, LOW);
    delay(100);
  }

}


void gamingLoop() { //-----No.4 GAME TIME
  while (stopp == 0) {
    flash(ledCount, speedd);
    ledCount++;
    if (ledCount == 8 && stopp == 0)
      ledCount = led1Pin;
  }
}


void resultDisplay() { //-----No.5 Game result
  ledCount--;
  flash(ledCount, 1000);
  delay(400);
  flash(ledCount, 1000);
  delay(400);
  flash(ledCount, 1000);
  delay(400);
  flash(ledCount, 1000);
}

//-----Core Functions---------------------------------------------------

/**
   Count how long the button has been hold by
   delaying 50 milliseconds and detect whether
   the button has been released. If not, continue
   the loop, and if released, return the total
   delay time (hold time).

   @Parameter: maximumLimit: Gives the maximum time one could hold the button. When that time is reached, loop breaks and return the maximum time.
   @Returns: holdTime: The time the button is hold
*/
unsigned int buttonHoldTime(int maximumLimit) {
  unsigned int holdTime = 0;
  while (1) {
    holdTime += 50;
    delay(50);
    if (digitalRead(buttonPin) == 0 || holdTime >= maximumLimit)  //Button released or maximumTime reached
      break;
  }
  return holdTime;

}

/**
   Set all led lights to input status (HIGH/ LOW)

   @Parameter: status: The status LEDs will be set to
*/
void setAll(int status) {
  for (int i = led1Pin; i <= led7Pin; i++)
    digitalWrite(i, status);
}

/**
   Turn on all LEDs, keep them some time,
   and turn off all LEDs, and delay some time.
   This will blink all LEDs if is used contimuously

   @Parameter: time: The time to hold all LEDs on in milliseconds
   @Parameter: dly: The time to hold all LEDs off in milliseconds
*/
void blinkAll(int time, int dly) {
  setAll(HIGH);
  delay(time);
  setAll(LOW);
  delay(dly);
}

/**
   Flash the specific LED, turning it on and hold some time, then turn off
   IMMIDIATELY BEFORE THE LED SHUT OFF, if button pressed, value 1 will
   be assigned to the global variable stopp for the game to stop
   ##THUS, player need to hold the button for longer than the delay in that level,
   to make sure the led currently displayed to the user could be stopped.##
   ##This limitation could be overcomed by replacing the "delay(interval);" by use of
   Clock Interrupt.##

   @Parameter: ledPin: The specific LED that will be flashed
   @Parameter: interval: The time interval the specific LED will be hold on
*/
void flash(int ledPin, int interval) {
  digitalWrite(ledPin, HIGH);
  delay(interval);
  if (digitalRead(buttonPin) == HIGH) //If the button is pressed
    stopp = 1;
  digitalWrite(ledPin, LOW);
}


