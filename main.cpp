/* main.cpp
 *
 * Genesis Anne Villar (RED ID: 824435476)
 * Steven Gervacio (RedID: 825656527)
 * CS 596 IOT - Prof. Donyanavard
 * Due Date: 3/17/2025
 *
 * File Description:
 * Complete traffic light cycle (red -> red-yellow ->  green ->  yellow -> red)
 * Pedestrian-activated crossing request via touch button
 * Audio feedback system with different patterns for each traffic light state
 * Configurable timing parameters for all states
 * Safety features including minimum green time after button press
 * TFT Display for status information
*/

#include <Arduino.h> 
#include <TFT_eSPI.h> // lib for esp32 display

// LEDC setup for buzzer - ESP32's LED Control peripheral
// -- generate PWM signals for the buzzer to create tones (...instead of just on/off signals)
#define LEDC_CHANNEL_0 0 // channel 0 for the buzzer
#define LEDC_TIMER_BIT 8 // 8bit resolution for PWM (values 0-255)
#define FREQ_HZ 1000 // frequency (hz) for the buzzer 

// constants for each traffic light state
#define GREEN_STATE 0  // traffic go
#define YELLOW_STATE 1  // warning, about to stop
#define RED_STATE 2   // traffic stopped
#define RED_YELLOW_STATE 3   // about to start moving

// colors for TFT display
#define TFT_BACKGROUND TFT_BLACK    
#define TFT_TEXT_COLOR TFT_WHITE      
#define TFT_RED_COLOR TFT_RED  // for RED state
#define TFT_YELLOW_COLOR TFT_YELLOW //for YELLOW state
#define TFT_GREEN_COLOR TFT_GREEN      // Green for GREEN state

TFT_eSPI tft = TFT_eSPI(); // initialize tft display object

// GLOBAL VARIABLES 

// define GPIO PINS of functions
const int RED_PIN = 2;      // red LED
const int YELLOW_PIN = 15;  // yellow LED
const int GREEN_PIN = 12;   // green LED
const int BUZZER_PIN = 21;  // buzzer
const int BUTTON_PIN = 22;  // button

// timing Constants (in ms)
const unsigned long RED_DURATION = 10000;  // tl remain in red for 10 seconds
const unsigned long YELLOW_DURATION = 2000; // tl remain in yellow light for 2 seconds
const unsigned long RED_YELLOW_DURATION = 2000; // tl remain in yellow-red light for 2 seconds
const unsigned long MIN_GREEN_AFTER_TOUCH = 5000; // after button press, green stays for at least 5 seconds

//buzzer pattern timings
const unsigned long GREEN_BUZZER_ON = 500; // buzzer on for 500ms during green
const unsigned long GREEN_BUZZER_OFF = 1500; // buzzer off for 1500ms during green
const unsigned long RED_BUZZER_ON = 250; // buzzer on for 250ms during red
const unsigned long RED_BUZZER_OFF = 250; // buzzer off for 250ms during red

// state variables
int currentState = RED_STATE; // start in RED state as (w/ given requirements)
unsigned long stateStartTime = 0; // when current state began
unsigned long buzzerStartTime = 0; // track buzzer timing
bool buttonPressed = false; // current button state
bool lastButtonState = false; // previous button state (for edge detection)
unsigned long buttonPressTime = 0; // when the button was last pressed
bool startupSequence = true;  // flag to track if we're in startup sequence
unsigned long remainingTime = 0; // time remaining in current state
String currentStateName = "";  // text for current state for display

// helper function to turn the buzzer on or off
void playTone(bool play) 
{
  if (play) 
  {
    ledcWrite(LEDC_CHANNEL_0, 127);  // 50% duty cycle (medium volume)
  } 
  else 
  {
    ledcWrite(LEDC_CHANNEL_0, 0);    // 0% duty cycle (off)
  }
}

// helper function to check for button presses and handle them
void checkButtonPress() 
{
  buttonPressed = (digitalRead(BUTTON_PIN) == LOW); // read button state (LOW when pressed due to pull-up resistor..)
  // detect new button press (transition from not pressed to pressed) -- this is edge detection to only trigger once per press
  if (buttonPressed && !lastButtonState) 
  {
    if (currentState == GREEN_STATE)  // only register button press when in GREEN state
    {
      Serial.println("BUTTON SEQUENCE: button pressed! pedestrian crossing requested");
      buttonPressTime = millis();  // record time of button press
      
      // update TFT display to show button press
      tft.fillRect(10, 100, 300, 30, TFT_BACKGROUND);  // clear previous message area
      tft.setCursor(10, 100);                        
      tft.setTextColor(TFT_RED_COLOR);                
      tft.println("BUTTON PRESSED!");               
      tft.setTextColor(TFT_TEXT_COLOR); // reset text color
    }
  }
  
  lastButtonState = buttonPressed;  // store current button state for next comparison (edge detection)
}

// helper funct to convert numeric state to text name for display
String getStateName(int state) 
{
  switch (state) 
  {
    case GREEN_STATE: return "GREEN";
    case YELLOW_STATE: return "YELLOW";
    case RED_STATE: return "RED";
    case RED_YELLOW_STATE: return "RED-YELLOW";
    default: return "UNKNOWN";  // error state just in case...
  }
}

// helper function to update the display with current system status
void updateDisplay() 
{
  unsigned long currentTime = millis();  // get current time
  unsigned long elapsedTime = currentTime - stateStartTime;  // calculate time in current state
  
  // calculate remaining time based on current state

  switch (currentState) 
  {
    case RED_STATE: // calculate seconds remaining in RED state
      if (RED_DURATION > elapsedTime) 
      {
        remainingTime = (RED_DURATION - elapsedTime) / 1000;
      } 
      else 
      {
        remainingTime = 0;
      }
      break;
      
    case YELLOW_STATE: // // Calculate seconds remaining in YELLOW state
      if (YELLOW_DURATION > elapsedTime) 
      {
        remainingTime = (YELLOW_DURATION - elapsedTime) / 1000;
      } 
      else 
      {
        remainingTime = 0;
      }
      break;
      
    case RED_YELLOW_STATE: // Calculate seconds remaining in RED-YELLOW state
      if (RED_YELLOW_DURATION > elapsedTime) 
      {
        remainingTime = (RED_YELLOW_DURATION - elapsedTime) / 1000;
      } 
      else 
      {
        remainingTime = 0;
      }
      break;
      
    case GREEN_STATE: // If button was pressed, calculate remaining time in minimum green period
      if (buttonPressTime > 0) 
      {
        unsigned long timeAfterButton = currentTime - buttonPressTime;
        if (timeAfterButton < MIN_GREEN_AFTER_TOUCH) 
        {
          remainingTime = (MIN_GREEN_AFTER_TOUCH - timeAfterButton) / 1000;
        } 
        else 
        {
          remainingTime = 0;
        }
      } 
      else  // if button not pressed... show indefinite time
      {
        remainingTime = 99;// (until button press)
      }
      break;
  }
  
  uint16_t stateColor; // helps select text color based on current state

  switch (currentState) 
  {
    case GREEN_STATE:
      stateColor = TFT_GREEN_COLOR; // GREEN state
      break;
    
    case YELLOW_STATE:
      stateColor = TFT_YELLOW_COLOR; // YELLOW state
      break;
   
    case RED_STATE:
      stateColor = TFT_RED_COLOR; // RED state
      break;
    
    case RED_YELLOW_STATE:
      stateColor = TFT_ORANGE; // orange for RED-YELLOW state
      break;
    
    default:
      stateColor = TFT_TEXT_COLOR; // default white for "unknown" states
  }
  
  tft.fillRect(10, 40, 300, 30, TFT_BACKGROUND);   // clear previous state display area to prevent text overlap
  
  // display current state
  tft.setCursor(10, 40);             
  tft.setTextColor(stateColor); // set color based on state
  tft.print("State: "); 
  tft.println(currentStateName);       // state name (RED, GREEN, ...)
  tft.setTextColor(TFT_TEXT_COLOR); // reset to default text color
  
  tft.fillRect(10, 70, 300, 30, TFT_BACKGROUND); // clear previous time display area
  
  // display remaining time
  tft.setCursor(10, 70);              
  if (currentState == GREEN_STATE && buttonPressTime == 0) // if in GREEN state with no button press, show "Until Button" instead of time
  {
    tft.println("Time: Until Button");
  } 
  else // otherwise show remaining seconds
  {
    tft.print("Time: ");
    tft.print(remainingTime);
    tft.println(" seconds");
  }
}

// helper function to handle state transitions based on timing and button presses
void handleStateTransitions() 
{
  unsigned long currentTime = millis();   // get current time
  bool stateChanged = false;              // flag to track if state changed
  
  switch (currentState) 
  {
    case GREEN_STATE:  // Green light is on - traffic is flowing
      digitalWrite(GREEN_PIN, HIGH); // turn on green LED + off for rest
      digitalWrite(YELLOW_PIN, LOW);     
      digitalWrite(RED_PIN, LOW);      
      
      if (buttonPressTime > 0 && (currentTime - buttonPressTime >= MIN_GREEN_AFTER_TOUCH))  // check if button was pressed and minimum green time has passed
      {
        //  ...transition to yellow state
        currentState = YELLOW_STATE;  // change state
        stateStartTime = currentTime; // reset state timer
        buttonPressTime = 0;   // reset button timer
        stateChanged = true; // mark state as changed
        startupSequence = false;   // no longer in startup sequence
        
        Serial.println("BUTTON SEQUENCE: Transitioning to YELLOW state");
        
        tft.fillRect(10, 100, 300, 60, TFT_BACKGROUND);  // clear button press message from display
      }
      break;
      
    case YELLOW_STATE: // yellow light is on - warning that red is coming
      digitalWrite(GREEN_PIN, LOW);    
      digitalWrite(YELLOW_PIN, HIGH);     // Turn on yellow LED + off else
      digitalWrite(RED_PIN, LOW);  
      
      if (currentTime - stateStartTime >= YELLOW_DURATION) // check if yellow duration has elapsed
      {
        // ..transition to red state
        currentState = RED_STATE;   // change state
        stateStartTime = currentTime;  // reset state timer
        stateChanged = true;   // mark state as changed
        
        // for logging and debugging at serial monitor..
        if (startupSequence) 
        {
          Serial.println("STARTUP SEQUENCE: Transitioning to RED state");
        } 
        else 
        {
          Serial.println("BUTTON SEQUENCE: Transitioning to RED state");
        }
      }
      break;
      
    case RED_STATE:  // red light is on - traffic is stopped
      digitalWrite(GREEN_PIN, LOW); 
      digitalWrite(YELLOW_PIN, LOW);
      digitalWrite(RED_PIN, HIGH);  // turn on red LED + off else
      
      if (currentTime - stateStartTime >= RED_DURATION)  // check if red duration has elapsed
      {
        // ...transition to red-yellow state
        currentState = RED_YELLOW_STATE;  // change state
        stateStartTime = currentTime;     // reset state timer
        stateChanged = true;              // mark state as changed
        
        // for logging and debugging at serial monitor..
        if (startupSequence) 
        {
          Serial.println("STARTUP SEQUENCE: Transitioning to RED-YELLOW state");
        } 
        else 
        {
          Serial.println("BUTTON SEQUENCE: Transitioning to RED-YELLOW state");
        }
      }
      break;
      
    case RED_YELLOW_STATE:  // red AND yellow lights on together -- about to go green
      digitalWrite(GREEN_PIN, LOW);  // turn off green
      digitalWrite(YELLOW_PIN, HIGH);  // turn ON yellow 
      digitalWrite(RED_PIN, HIGH);   // turn ON red 
      
      if (currentTime - stateStartTime >= RED_YELLOW_DURATION)   // check if red-yellow duration has elapsed
      {
        // ...transition to green state
        currentState = GREEN_STATE; // change state
        stateStartTime = currentTime; // reset state timer
        stateChanged = true;   // mark state as changed
        
        // for logging and debugging at serial monitor..
        if (startupSequence) 
        {
          Serial.println("STARTUP SEQUENCE: Transitioning to GREEN state");
          Serial.println("STARTUP SEQUENCE COMPLETE: System now in normal operation");
          
          // update TFT display header for normal operation
          tft.fillRect(10, 10, 300, 30, TFT_BACKGROUND);
          tft.setCursor(10, 10);
          tft.println("NORMAL OPERATION");
          
          startupSequence = false;  // startup sequence now complete
        } 
        else 
        {
          Serial.println("BUTTON SEQUENCE: Transitioning to GREEN state");
          Serial.println("BUTTON SEQUENCE COMPLETE: Waiting for next button press");
        }
      }
      break;
  }
  
  if (stateChanged)   // update display if state changed
  {
    currentStateName = getStateName(currentState);
    updateDisplay(); //refresh the display
  }
}

// helper function to control buzzer based on current state
void controlBuzzer() 
{
  unsigned long currentTime = millis();
  
  // determine buzzer pattern based on current state
  if (currentState == RED_STATE) 
  {
    //fast pattern during RED state
    unsigned long cycleTime = RED_BUZZER_ON + RED_BUZZER_OFF;  // total cycle time
    unsigned long timeInCycle = (currentTime - stateStartTime) % cycleTime;  // position in cycle
    
    // turn buzzer on or off based on position in cycle
    if (timeInCycle < RED_BUZZER_ON) 
    {
      playTone(true); // turn buzzer on
    } 
    else
    {
      playTone(false); //turn buzzer off
    }
  } 
  else if (currentState == GREEN_STATE) // control buzzer with slow pattern during GREEN state
  {
    unsigned long cycleTime = GREEN_BUZZER_ON + GREEN_BUZZER_OFF;  //total cycle time
    unsigned long timeInCycle = (currentTime - stateStartTime) % cycleTime; //position in cycle
    
    // turn buzzer on or off based on position in cycle
    if (timeInCycle < GREEN_BUZZER_ON) 
    {
      playTone(true);   // turn buzzer on
    } 
    else 
    {
      playTone(false);   // turn buzzer off
    }
  } 
  else 
  {
    playTone(false);  // turn off buzzer for other states (YELLOW and RED_YELLOW)
  }
}


void setup() 
{
  Serial.begin(9600);    
  Serial.println("traffic light controller START"); 
  
  // initialize TFT display
  tft.init();                         
  tft.setRotation(1);                  // landscape orientation
  tft.fillScreen(TFT_BACKGROUND);      // fill screen with background color
  tft.setTextColor(TFT_TEXT_COLOR);    // set default text color
  tft.setTextSize(2);                  // set text size 

  pinMode(RED_PIN, OUTPUT); // set LED pins as outputs
  pinMode(YELLOW_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP); // set button pin as input with internal pull-up resistor (button will read LOW when pressed)
  
  // initialize buzzer with LEDC (ESP32's PWM controller)
  ledcSetup(LEDC_CHANNEL_0, FREQ_HZ, LEDC_TIMER_BIT);  // configure LEDC channel
  ledcAttachPin(BUZZER_PIN, LEDC_CHANNEL_0);  // attach buzzer pin to LEDC channel
  
  // Start with red light on (as per project requirements)
  digitalWrite(RED_PIN, HIGH);  // turn on red LED + rest off
  digitalWrite(YELLOW_PIN, LOW); 
  digitalWrite(GREEN_PIN, LOW);  
  
  // record the current time as the start time of the first state
  stateStartTime = millis(); 
  buzzerStartTime = millis();   // start buzzer timing
  
  Serial.println("STARTUP SEQUENCE: Traffic Light Controller started in RED state");
  Serial.println("System will follow startup sequence: RED -> RED-YELLOW -> GREEN");
  
  // initialize TFT display with startup information
  tft.fillScreen(TFT_BACKGROUND);      // clear screen
  tft.setCursor(10, 10);             
  tft.println("STARTUP SEQUENCE");    
  tft.setCursor(10, 40);              
  tft.println("Starting in RED");     
  tft.setCursor(10, 70);              
  tft.println("Time: 10 seconds");   

  // store current state name for display
  currentStateName = getStateName(currentState);
  updateDisplay();   // update the display with all current values
}

void loop() 
{
  static unsigned long lastDisplayUpdate = 0;  // track last display update time
  unsigned long currentTime = millis();        // get current time
  checkButtonPress();//first, check for button press
  handleStateTransitions(); // handle state transitions based on timing and input
  controlBuzzer(); // control buzzer based on current state
  
  if (currentTime - lastDisplayUpdate >= 100) // update display every 100ms (for efficiency..)
  {
    updateDisplay();
    lastDisplayUpdate = currentTime;
  }
  
  delay(10); // small delay for stability
}