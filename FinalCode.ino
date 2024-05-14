#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <IRremote.hpp>
#include <Keypad.h>
#include <EmonLib.h>

// Create an instance of LiquidCrystal_I2C
LiquidCrystal_I2C lcd(0x27, 16, 2);



// Create instances for current transformers >
EnergyMonitor ct1;
EnergyMonitor ct2;
EnergyMonitor ct3;
EnergyMonitor ct4;
// < Create instances for current transformers



// Define that the infrared receiver is connected to digital pin 2 on the Arduino
#define IR_PIN 2



// Define keypad >
const byte ROWS = 4;
const byte COLUMNS = 4;

char hexaKeys[ROWS][COLUMNS] = {
  { '1', '2', '3', 'U' },
  { '4', '5', '6', 'D' },
  { '7', '8', '9', 'O' },
  { 'C', '0', 'H', 'E' }
};

byte rowPins[ROWS] = { 23, 25, 27, 29 };        // R1 > R2 > R3 > R4
byte columnPins[COLUMNS] = { 22, 24, 26, 28 };  // C1 > C2 > C3 > C4

// Create an instance of Keypad
Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, columnPins, ROWS, COLUMNS);
// < Define keypad



// Digital pins for the relays >
const int RELAY_ONE_PIN = 8;
const int RELAY_TWO_PIN = 9;
const int RELAY_THREE_PIN = 10;
const int RELAY_FOUR_PIN = 11;
// < Digital pins for the relays



// Analog pins for the current transformers >
const int CT_ONE_PIN = 1;
const int CT_TWO_PIN = 2;
const int CT_THREE_PIN = 3;
const int CT_FOUR_PIN = 4;
// < Analog pins for the current transformers



// Defining buttons on the remote >
const int IR_BUTTON_OK = 64;
const int IR_BUTTON_UP = 70;
const int IR_BUTTON_DOWN = 21;
const int IR_BUTTON_LEFT = 68;
const int IR_BUTTON_RIGHT = 67;
const int IR_BUTTON_1 = 22;
const int IR_BUTTON_2 = 25;
const int IR_BUTTON_3 = 13;
const int IR_BUTTON_4 = 12;
const int IR_BUTTON_5 = 24;
const int IR_BUTTON_6 = 94;
const int IR_BUTTON_7 = 8;
const int IR_BUTTON_8 = 28;
const int IR_BUTTON_9 = 90;
const int IR_BUTTON_0 = 82;
const int IR_BUTTON_ASTERISK = 66;
const int IR_BUTTON_POUND = 74;
// < Defining buttons on the remote



// Defining button lists >
const int loadList[5] = { IR_BUTTON_1, IR_BUTTON_2, IR_BUTTON_3, IR_BUTTON_4, IR_BUTTON_5 };

const int remoteButtonList[17] = {
  IR_BUTTON_0,
  IR_BUTTON_1,
  IR_BUTTON_2,
  IR_BUTTON_3,
  IR_BUTTON_4,
  IR_BUTTON_5,
  IR_BUTTON_6,
  IR_BUTTON_7,
  IR_BUTTON_8,
  IR_BUTTON_9,
  IR_BUTTON_OK,
  IR_BUTTON_LEFT,
  IR_BUTTON_RIGHT,
  IR_BUTTON_UP,
  IR_BUTTON_DOWN,
  IR_BUTTON_POUND,
  IR_BUTTON_ASTERISK,
};
// < Defining button lists



// Used to indicate if the IR receiver and keypad are currently active >
bool receiverActive = true;
// < Used to indicate if the IR receiver and keypad are currently active



unsigned long currentMillisLoadOne = 0;
unsigned long currentMillisLoadTwo = 0;
unsigned long currentMillisLoadThree = 0;
unsigned long currentMillisLoadFour = 0;



// Controlling the loads (they're ON by default) >
bool loadOneOff = false;
bool loadTwoOff = false;
bool loadThreeOff = false;
bool loadFourOff = false;
// < Controlling the loads (they're ON by default)



// Indicates if a load was turned off due to exceeding MAX_CURRENT >
bool loadOneOffDueToExcessCurrent = false;
bool loadTwoOffDueToExcessCurrent = false;
bool loadThreeOffDueToExcessCurrent = false;
bool loadFourOffDueToExcessCurrent = false;
// < Indicates if a load was turned off due to exceeding MAX_CURRENT



unsigned long turnLoadsBackOnFromExcessCurrentMillis;
bool shouldResetTurnLoadsBackOnFromExcessCurrentMillis = false;
const int TIME_TO_TURN_LOADS_BACK_ON = 10000;  // Milliseconds



// Used for CheckLoadStatuses() >
bool shouldRetrieveLoadOne = true;
bool shouldRetrieveLoadTwo = true;
bool shouldRetrieveLoadThree = true;
bool shouldRetrieveLoadFour = true;
// < Used for CheckLoadStatuses()



// For load timers >
int loadOneHours;
int loadOneMinutes;
int loadOneSeconds;

int loadTwoHours;
int loadTwoMinutes;
int loadTwoSeconds;

int loadThreeHours;
int loadThreeMinutes;
int loadThreeSeconds;

int loadFourHours;
int loadFourMinutes;
int loadFourSeconds;
// < For load timers



// Tells if load timers are currently active >
bool loadOneTimerActive = false;
bool loadTwoTimerActive = false;
bool loadThreeTimerActive = false;
bool loadFourTimerActive = false;
// < Tells if load timers are currently active



// Tells if load timers were recently active >
bool loadOneTimerRecentlyActive = false;
bool loadTwoTimerRecentlyActive = false;
bool loadThreeTimerRecentlyActive = false;
bool loadFourTimerRecentlyActive = false;
// < Tells if load timers were recently active



// Load priorities >
char loadOnePriority = 'H';
char loadTwoPriority = 'H';
char loadThreePriority = 'H';
char loadFourPriority = 'H';

char previousLoadOnePriority = 'H';
char previousLoadTwoPriority = 'H';
char previousLoadThreePriority = 'H';
char previousLoadFourPriority = 'H';
// < Load priorities



int loadsOut = 0;
int previousLoadsOut = 0;
bool numLoadsOut[4] = { 0, 0, 0, 0 };

int selectedLoad = -1;



// Indicates if a "set" function is currently active >
bool setFunctionActive = false;
// < Indicates if a "set" function is currently active



// Used to turn off the LCD's backlight if a command hasn't been pressed in a while >
int lcdTimeout = 60;  // Seconds
unsigned long currentCheckLCDTimeoutMillis;
// < Used to turn off the LCD's backlight if a command hasn't been pressed in a while



// Indicates if something is being run for the first time or consecutively >
bool firstWelcomeScreenRun = true;
bool firstLoopRun = true;
bool firstCheckLCDTimeoutRun = true;
bool firstLoadOneTimerRun = true;
bool firstLoadTwoTimerRun = true;
bool firstLoadThreeTimerRun = true;
bool firstLoadFourTimerRun = true;
// < Indicates if something is being run for the first time or consecutively



// Current sensor values >
float sensorValue1 = 0;
float sensorValue2 = 0;
float sensorValue3 = 0;
float sensorValue4 = 0;

float previousCurrentReading;
float previousLoad1CurrentReading = 0;
float previousLoad2CurrentReading = 0;
float previousLoad3CurrentReading = 0;
float previousLoad4CurrentReading = 0;
float previousTotalCurrentReading = 0;

float total = 0;
// < Current sensor values



// Used to create a specific interval in which current should be read to improve program execution speed >
const int TIME_BETWEEN_CURRENT_READS = 800;

int selector = 0;
unsigned long readCurrentMillis;
bool shouldResetReadCurrentMillis = false;
// < Used to create a specific interval in which current should be read to improve program execution speed



bool totalRecentlyExceeded = false;



// For current graphs >
int columnToPrint = 9;  // Used to determine which column on the LCD to print the current graph on

byte lineOne[8] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111
};

byte lineTwo[8] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
  B00000
};

byte lineThree[8] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
  B00000,
  B00000
};

byte lineFour[8] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
  B00000,
  B00000,
  B00000
};

byte lineFive[8] = {
  B00000,
  B00000,
  B00000,
  B11111,
  B00000,
  B00000,
  B00000,
  B00000
};

byte lineSix[8] = {
  B00000,
  B00000,
  B11111,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};

byte lineSeven[8] = {
  B00000,
  B11111,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};

byte lineEight[8] = {
  B11111,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};
// < For current graphs



// Values for caculating load currents >

// Number of samples used when calculating Irms
const int NUM_OF_SAMPLES = 1484;

// Calibration constant for calculating Irms via the current transformers
const float CALIBRATION_CONSTANT = 20.0;

// < Values for calculating load currents



// Maximum current limit for all connected loads
const float MAX_CURRENT = 14.0;

// Threshold for setting low priorities
const float LOW_PRIORITY_CURRENT = 0.5;





void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);



  // Setting up the LCD >
  lcd.init();
  lcd.backlight();

  lcd.createChar(1, lineOne);
  lcd.createChar(2, lineTwo);
  lcd.createChar(3, lineThree);
  lcd.createChar(4, lineFour);
  lcd.createChar(5, lineFive);
  lcd.createChar(6, lineSix);
  lcd.createChar(7, lineSeven);
  lcd.createChar(8, lineEight);
  // < Setting up the LCD



  // Setting up the IR receiver >
  IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK);
  receiverActive = true;
  // < Setting up the IR receiver



  // These digital pins are connected to the relays >
  pinMode(RELAY_ONE_PIN, OUTPUT);
  pinMode(RELAY_TWO_PIN, OUTPUT);
  pinMode(RELAY_THREE_PIN, OUTPUT);
  pinMode(RELAY_FOUR_PIN, OUTPUT);
  // < These digital pins are connected to the relays



  // Current transformers >
  ct1.current(CT_ONE_PIN, CALIBRATION_CONSTANT);  // input pin (CT_ONE_PIN), calibration constant
  ct2.current(CT_TWO_PIN, CALIBRATION_CONSTANT);
  ct3.current(CT_THREE_PIN, CALIBRATION_CONSTANT);
  ct4.current(CT_FOUR_PIN, CALIBRATION_CONSTANT);
  // < Current transformers



  // Turn the relays OFF >
  for (int i = RELAY_ONE_PIN; i <= RELAY_FOUR_PIN; i++) {
    digitalWrite(i, HIGH);
  }
  // < Turn the relays OFF



  // Inform the user what the max current capacity is set to >
  lcd.clear();
  lcd.setCursor(5, 0);
  lcd.print("Hello!");
  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Max current set");
  lcd.setCursor(0, 1);
  lcd.print("to ");
  lcd.print(MAX_CURRENT);
  lcd.print("A");
  // < Inform the user what the max current capacity is set to



  // Turning all relays ON with a 1 second delay >
  for (int i = RELAY_ONE_PIN; i <= RELAY_FOUR_PIN; i++) {
    digitalWrite(i, LOW);

    delay(1000);
  }
  // < Turning all relays ON with a 1 second delay

}  // setup()





void loop() {
  // put your main code here, to run repeatedly:
  int command;

  int warningMessageDelayTime = 2000;    // In milliseconds
  int currentReadingRefreshTime = 1000;  // Time between current reading refreshes in milliseconds

  int remoteDisableTime = 500;  // In milliseconds

  bool whileLoopBreak = false;  // For breaking out of the welcome screen

  // Check if loads should be ON or OFF
  CheckLoadStatuses();

  // Run load timers if they're active
  CheckTimerStatuses();

  // Read load currents
  ReadCurrent();

  // Turn the LCD OFF if no command is pressed within a certain amount of time
  CheckLCDTimeout();



  // Checking if loop() is being run for the first time
  if (firstLoopRun) {
    command = IR_BUTTON_0;

    firstLoopRun = 0;
  }

  else {
    command = ReceiveCommand();  // Retrieve the command from the remote
  }



  // If the user is on the Overall System Info Screen, update the total current and LO if they change >
  if (selectedLoad == IR_BUTTON_5 && !setFunctionActive) {
    if (total != previousTotalCurrentReading) {

      // Erase the outdated total current reading to be replaced
      lcd.setCursor(0, 1);
      lcd.print("       ");

      // Update the total current reading
      lcd.setCursor(0, 1);
      lcd.print(total);
      lcd.print("A");

      previousTotalCurrentReading = total;

      command = CustomDelay(TIME_BETWEEN_CURRENT_READS * 4);
    }

    else {
      SetLoadPriorities();

      command = CustomDelay(TIME_BETWEEN_CURRENT_READS * 4);
    }  // else
  }    // if (selectedLoad == IR_BUTTON_5 && !setFunctionActive)

  if (selectedLoad == IR_BUTTON_5) {

    if (loadsOut != previousLoadsOut) {
      lcd.setCursor(7, 1);
      lcd.print("         ");

      lcd.setCursor(10, 1);
      lcd.print("LO = ");
      lcd.print(loadsOut);

      previousLoadsOut = loadsOut;
    }
  }  // if (selectedLoad == IR_BUTTON_5)

  // < If the user is on the Overall System Info Screen, update the total current and LO if they change



  // Welcome Screen >
  if (command == IR_BUTTON_0) {

    // Repeatedly display the instructions
    while (true) {
      command = WelcomeScreen();

      if (command != IR_BUTTON_0) {
        firstWelcomeScreenRun = true;  // The welcome screen will have a fresh run

        // Check if the user pressed buttons 6 or 8 (which are invalid)
        if (command == IR_BUTTON_6 || command == IR_BUTTON_8) {
          InvalidCommand();

          command = IR_BUTTON_0;
        }



        // Checks if the user pressed an invalid button other than 6 or 8 >
        for (int i = 9; i < 17; i++) {
          if (command == remoteButtonList[i]) {
            InvalidCommand();

            command = IR_BUTTON_0;

            break;
          }
        }
        // < Checks if the user pressed an invalid button other than 6 or 8



        // Check if a load's button was pressed
        for (int i = 1; i < 6; i++) {
          if (command == remoteButtonList[i]) {
            RetrieveLoadInfo(command);

            whileLoopBreak = true;

            break;
          }
        }
        if (whileLoopBreak) {
          break;
        }

        // If the user wants to set a timer
        if (command == IR_BUTTON_7) {
          SetTimer();

          break;
        }
      }  // if (command != IR_BUTTON_0)

      else {
        firstWelcomeScreenRun = false;  // The welcome screen has been running
      }
    }  // while (true)
  }    // if (command == IR_BUTTON_0)
  // < Welcome Screen



  // Turning a load ON/OFF >
  else if (command == IR_BUTTON_OK) {

    // Load 1 >
    if (selectedLoad == IR_BUTTON_1) {

      // Check if Load 1's timer is currently active before changing its state >
      if (loadOneTimerActive) {
        StopReceiver();

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Load 1's timer");
        ScrollText("is currently active.", 1);

        CustomDelay(warningMessageDelayTime);

        lcd.clear();
        lcd.setCursor(0, 0);

        if (loadOneOff) {
          ScrollText("Turning ON Load 1", 0);
        } else {
          ScrollText("Turning OFF Load 1", 0);
        }

        ScrollText("will deactivate its timer.", 1);

        CustomDelay(warningMessageDelayTime);

        lcd.clear();
        lcd.setCursor(0, 0);

        if (loadOneOff) {
          lcd.print("Turn Load 1 ON?");

        } else {
          lcd.print("Turn Load 1 OFF?");
        }

        StartReceiver();

        while (true) {
          command = CustomDelay(1000);

          if (command == IR_BUTTON_1) {
            loadOneOff = !loadOneOff;
            numLoadsOut[0] = !numLoadsOut[0];

            loadOneHours = 0;
            loadOneMinutes = 0;
            loadOneSeconds = 0;

            loadOneTimerActive = false;
            loadOneTimerRecentlyActive = false;

            selectedLoad = IR_BUTTON_1;  // selectedLoad was set to -1 in ScrollText(), so it needs to be set appropriately again here

            break;
          }

          else if (command == IR_BUTTON_0) {
            StopReceiver();

            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Command");
            lcd.setCursor(0, 1);
            lcd.print("Cancelled");

            CustomDelay(warningMessageDelayTime);

            StartReceiver();

            selectedLoad = IR_BUTTON_1;  // selectedLoad was set to -1 in ScrollText(), so it needs to be set appropriately again here

            break;
          }

          command = ScrollText("1 to continue, 0 to cancel", 1);

          if (command == IR_BUTTON_1) {
            loadOneOff = !loadOneOff;
            numLoadsOut[0] = !numLoadsOut[0];

            loadOneHours = 0;
            loadOneMinutes = 0;
            loadOneSeconds = 0;

            loadOneTimerActive = false;
            loadOneTimerRecentlyActive = false;

            selectedLoad = IR_BUTTON_1;  // selectedLoad was set to -1 in ScrollText(), so it needs to be set appropriately again here

            break;
          }

          else if (command == IR_BUTTON_0) {
            StopReceiver();

            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Command");
            lcd.setCursor(0, 1);
            lcd.print("Cancelled");

            CustomDelay(warningMessageDelayTime);

            StartReceiver();

            selectedLoad = IR_BUTTON_1;  // selectedLoad was set to -1 in ScrollText(), so it needs to be set appropriately again here

            break;
          }

          command = CustomDelay(1000);

          if (command == IR_BUTTON_1) {
            loadOneOff = !loadOneOff;
            numLoadsOut[0] = !numLoadsOut[0];

            loadOneHours = 0;
            loadOneMinutes = 0;
            loadOneSeconds = 0;

            loadOneTimerActive = false;
            loadOneTimerRecentlyActive = false;

            selectedLoad = IR_BUTTON_1;  // selectedLoad was set to -1 in ScrollText(), so it needs to be set appropriately again here

            break;
          }

          else if (command == IR_BUTTON_0) {
            StopReceiver();

            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Command");
            lcd.setCursor(0, 1);
            lcd.print("Cancelled");

            CustomDelay(warningMessageDelayTime);

            StartReceiver();

            selectedLoad = IR_BUTTON_1;  // selectedLoad was set to -1 in ScrollText(), so it needs to be set appropriately again here

            break;
          }

          lcd.setCursor(0, 1);
          lcd.print("                ");
          lcd.setCursor(0, 1);
        }  // while (true)
      }    //if (loadOneTimerActive)
      // < Check if Load 1's timer is currently active before changing its state



      else {
        loadOneOff = !loadOneOff;
        numLoadsOut[0] = !numLoadsOut[0];

        loadOnePriority = 'H';

        loadOneOffDueToExcessCurrent = false;
      }
    }  // if (selectedLoad == IR_BUTTON_1)
    // < Load 1



    // Load 2 >
    else if (selectedLoad == IR_BUTTON_2) {

      // Check if Load 2's timer is currently active before changing its state >
      if (loadTwoTimerActive) {
        StopReceiver();

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Load 2's timer");
        ScrollText("is currently active.", 1);

        CustomDelay(warningMessageDelayTime);

        lcd.clear();
        lcd.setCursor(0, 0);

        if (loadTwoOff) {
          ScrollText("Turning ON Load 2", 0);
        } else {
          ScrollText("Turning OFF Load 2", 0);
        }

        ScrollText("will deactivate its timer.", 1);

        CustomDelay(warningMessageDelayTime);

        lcd.clear();
        lcd.setCursor(0, 0);

        if (loadOneOff) {
          lcd.print("Turn Load 2 ON?");

        } else {
          lcd.print("Turn Load 2 OFF?");
        }

        StartReceiver();

        while (true) {
          command = CustomDelay(1000);

          if (command == IR_BUTTON_1) {
            loadTwoOff = !loadTwoOff;
            numLoadsOut[1] = !numLoadsOut[1];

            loadTwoHours = 0;
            loadTwoMinutes = 0;
            loadTwoSeconds = 0;

            loadTwoTimerActive = false;
            loadTwoTimerRecentlyActive = false;

            selectedLoad = IR_BUTTON_2;  // selectedLoad was set to -1 in ScrollText(), so it needs to be set appropriately again here

            break;
          }

          else if (command == IR_BUTTON_0) {
            StopReceiver();

            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Command");
            lcd.setCursor(0, 1);
            lcd.print("Cancelled");

            CustomDelay(warningMessageDelayTime);

            StartReceiver();

            selectedLoad = IR_BUTTON_2;  // selectedLoad was set to -1 in ScrollText(), so it needs to be set appropriately again here

            break;
          }

          command = ScrollText("1 to continue, 0 to cancel", 1);

          if (command == IR_BUTTON_1) {
            loadTwoOff = !loadTwoOff;
            numLoadsOut[1] = !numLoadsOut[1];

            loadTwoHours = 0;
            loadTwoMinutes = 0;
            loadTwoSeconds = 0;

            loadTwoTimerActive = false;
            loadTwoTimerRecentlyActive = false;

            selectedLoad = IR_BUTTON_2;  // selectedLoad was set to -1 in ScrollText(), so it needs to be set appropriately again here

            break;
          }

          else if (command == IR_BUTTON_0) {
            StopReceiver();

            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Command");
            lcd.setCursor(0, 1);
            lcd.print("Cancelled");

            CustomDelay(warningMessageDelayTime);

            StartReceiver();

            selectedLoad = IR_BUTTON_2;  // selectedLoad was set to -1 in ScrollText(), so it needs to be set appropriately again here

            break;
          }

          command = CustomDelay(1000);

          if (command == IR_BUTTON_1) {
            loadTwoOff = !loadTwoOff;
            numLoadsOut[1] = !numLoadsOut[1];

            loadTwoHours = 0;
            loadTwoMinutes = 0;
            loadTwoSeconds = 0;

            loadTwoTimerActive = false;
            loadTwoTimerRecentlyActive = false;

            selectedLoad = IR_BUTTON_2;  // selectedLoad was set to -1 in ScrollText(), so it needs to be set appropriately again here

            break;
          }

          else if (command == IR_BUTTON_0) {
            StopReceiver();

            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Command");
            lcd.setCursor(0, 1);
            lcd.print("Cancelled");

            CustomDelay(warningMessageDelayTime);

            StartReceiver();

            selectedLoad = IR_BUTTON_2;  // selectedLoad was set to -1 in ScrollText(), so it needs to be set appropriately again here

            break;
          }

          lcd.setCursor(0, 1);
          lcd.print("                ");
          lcd.setCursor(0, 1);
        }  // while (true)
      }    //if (loadTwoTimerActive)
      // < Check if Load 2's timer is currently active before changing its state



      else {
        loadTwoOff = !loadTwoOff;
        numLoadsOut[1] = !numLoadsOut[1];

        loadTwoPriority = 'H';

        loadTwoOffDueToExcessCurrent = false;
      }
    }
    // < Load 2



    // Load 3 >
    else if (selectedLoad == IR_BUTTON_3) {

      // Check if Load 3's timer is currently active before changing its state >
      if (loadThreeTimerActive) {
        StopReceiver();

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Load 3's timer");
        ScrollText("is currently active.", 1);

        CustomDelay(warningMessageDelayTime);

        lcd.clear();
        lcd.setCursor(0, 0);

        if (loadThreeOff) {
          ScrollText("Turning ON Load 3", 0);
        } else {
          ScrollText("Turning OFF Load 3", 0);
        }

        ScrollText("will deactivate its timer.", 1);

        CustomDelay(warningMessageDelayTime);

        lcd.clear();
        lcd.setCursor(0, 0);

        if (loadThreeOff) {
          lcd.print("Turn Load 3 ON?");

        } else {
          lcd.print("Turn Load 3 OFF?");
        }

        StartReceiver();

        while (true) {
          command = CustomDelay(1000);

          if (command == IR_BUTTON_1) {
            loadThreeOff = !loadThreeOff;
            numLoadsOut[2] = !numLoadsOut[2];

            loadThreeHours = 0;
            loadThreeMinutes = 0;
            loadThreeSeconds = 0;

            loadThreeTimerActive = false;
            loadThreeTimerRecentlyActive = false;

            selectedLoad = IR_BUTTON_3;  // selectedLoad was set to -1 in ScrollText(), so it needs to be set appropriately again here

            break;
          }

          else if (command == IR_BUTTON_0) {
            StopReceiver();

            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Command");
            lcd.setCursor(0, 1);
            lcd.print("Cancelled");

            CustomDelay(warningMessageDelayTime);

            StartReceiver();

            selectedLoad = IR_BUTTON_3;  // selectedLoad was set to -1 in ScrollText(), so it needs to be set appropriately again here

            break;
          }

          command = ScrollText("1 to continue, 0 to cancel", 1);

          if (command == IR_BUTTON_1) {
            loadThreeOff = !loadThreeOff;
            numLoadsOut[2] = !numLoadsOut[2];

            loadThreeHours = 0;
            loadThreeMinutes = 0;
            loadThreeSeconds = 0;

            loadThreeTimerActive = false;
            loadThreeTimerRecentlyActive = false;

            selectedLoad = IR_BUTTON_3;  // selectedLoad was set to -1 in ScrollText(), so it needs to be set appropriately again here

            break;
          }

          else if (command == IR_BUTTON_0) {
            StopReceiver();

            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Command");
            lcd.setCursor(0, 1);
            lcd.print("Cancelled");

            CustomDelay(warningMessageDelayTime);

            StartReceiver();

            selectedLoad = IR_BUTTON_3;  // selectedLoad was set to -1 in ScrollText(), so it needs to be set appropriately again here

            break;
          }

          command = CustomDelay(1000);

          if (command == IR_BUTTON_1) {
            loadThreeOff = !loadThreeOff;
            numLoadsOut[2] = !numLoadsOut[2];

            loadThreeHours = 0;
            loadThreeMinutes = 0;
            loadThreeSeconds = 0;

            loadThreeTimerActive = false;
            loadThreeTimerRecentlyActive = false;

            selectedLoad = IR_BUTTON_3;  // selectedLoad was set to -1 in ScrollText(), so it needs to be set appropriately again here

            break;
          }

          else if (command == IR_BUTTON_0) {
            StopReceiver();

            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Command");
            lcd.setCursor(0, 1);
            lcd.print("Cancelled");

            CustomDelay(warningMessageDelayTime);

            StartReceiver();

            selectedLoad = IR_BUTTON_3;  // selectedLoad was set to -1 in ScrollText(), so it needs to be set appropriately again here

            break;
          }

          lcd.setCursor(0, 1);
          lcd.print("                ");
          lcd.setCursor(0, 1);
        }  // while (true)
      }    //if (loadThreeTimerActive)
      // < Check if Load 3's timer is currently active before changing its state



      else {
        loadThreeOff = !loadThreeOff;
        numLoadsOut[2] = !numLoadsOut[2];

        loadThreePriority = 'H';

        loadThreeOffDueToExcessCurrent = false;
      }
    }
    // < Load 3



    // Load 4 >
    else if (selectedLoad == IR_BUTTON_4) {

      // Check if Load 4's timer is currently active before changing its state >
      if (loadFourTimerActive) {
        StopReceiver();

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Load 4's timer");
        ScrollText("is currently active.", 1);

        CustomDelay(warningMessageDelayTime);

        lcd.clear();
        lcd.setCursor(0, 0);

        if (loadFourOff) {
          ScrollText("Turning ON Load 4", 0);
        } else {
          ScrollText("Turning OFF Load 4", 0);
        }

        ScrollText("will deactivate its timer.", 1);

        CustomDelay(warningMessageDelayTime);

        lcd.clear();
        lcd.setCursor(0, 0);

        if (loadFourOff) {
          lcd.print("Turn Load 4 ON?");

        } else {
          lcd.print("Turn Load 4 OFF?");
        }

        StartReceiver();

        while (true) {
          command = CustomDelay(1000);

          if (command == IR_BUTTON_1) {
            loadFourOff = !loadFourOff;
            numLoadsOut[3] = !numLoadsOut[3];

            loadFourHours = 0;
            loadFourMinutes = 0;
            loadFourSeconds = 0;

            loadFourTimerActive = false;
            loadFourTimerRecentlyActive = false;

            selectedLoad = IR_BUTTON_4;  // selectedLoad was set to -1 in ScrollText(), so it needs to be set appropriately again here

            break;
          }

          else if (command == IR_BUTTON_0) {
            StopReceiver();

            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Command");
            lcd.setCursor(0, 1);
            lcd.print("Cancelled");

            CustomDelay(warningMessageDelayTime);

            StartReceiver();

            selectedLoad = IR_BUTTON_4;  // selectedLoad was set to -1 in ScrollText(), so it needs to be set appropriately again here

            break;
          }

          command = ScrollText("1 to continue, 0 to cancel", 1);

          if (command == IR_BUTTON_1) {
            loadFourOff = !loadFourOff;
            numLoadsOut[3] = !numLoadsOut[3];

            loadFourHours = 0;
            loadFourMinutes = 0;
            loadFourSeconds = 0;

            loadFourTimerActive = false;
            loadFourTimerRecentlyActive = false;

            selectedLoad = IR_BUTTON_4;  // selectedLoad was set to -1 in ScrollText(), so it needs to be set appropriately again here

            break;
          }

          else if (command == IR_BUTTON_0) {
            StopReceiver();

            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Command");
            lcd.setCursor(0, 1);
            lcd.print("Cancelled");

            CustomDelay(warningMessageDelayTime);

            StartReceiver();

            selectedLoad = IR_BUTTON_4;  // selectedLoad was set to -1 in ScrollText(), so it needs to be set appropriately again here

            break;
          }

          command = CustomDelay(1000);

          if (command == IR_BUTTON_1) {
            loadFourOff = !loadFourOff;
            numLoadsOut[3] = !numLoadsOut[3];

            loadFourHours = 0;
            loadFourMinutes = 0;
            loadFourSeconds = 0;

            loadFourTimerActive = false;
            loadFourTimerRecentlyActive = false;

            selectedLoad = IR_BUTTON_4;  // selectedLoad was set to -1 in ScrollText(), so it needs to be set appropriately again here

            break;
          }

          else if (command == IR_BUTTON_0) {
            StopReceiver();

            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Command");
            lcd.setCursor(0, 1);
            lcd.print("Cancelled");

            CustomDelay(warningMessageDelayTime);

            StartReceiver();

            selectedLoad = IR_BUTTON_4;  // selectedLoad was set to -1 in ScrollText(), so it needs to be set appropriately again here

            break;
          }

          lcd.setCursor(0, 1);
          lcd.print("                ");
          lcd.setCursor(0, 1);
        }  // while (true)
      }    //if (loadFourTimerActive)
      // < Check if Load 4's timer is currently active before changing its state



      else {
        loadFourOff = !loadFourOff;
        numLoadsOut[3] = !numLoadsOut[3];

        loadFourPriority = 'H';

        loadFourOffDueToExcessCurrent = false;
      }
    }
    // < Load 4



    RetrieveLoadInfo(selectedLoad);  // Get the selected load's information

    command = 0;  // Reset command so that the current command isn't unwantedly repeated

    delay(remoteDisableTime);
  }  // else if (command == IR_BUTTON_OK)
  // < Turning a load ON/OFF



  // Scrolling to previous load >
  else if (command == IR_BUTTON_LEFT && selectedLoad != IR_BUTTON_1) {
    int newLoad = Scroll(selectedLoad, 1);

    command = 0;  // Reset command so that the current command isn't unwantedly repeated

    StopReceiver();

    CustomDelay(remoteDisableTime);

    StartReceiver();

    selectedLoad = newLoad;
  }
  // < Scrolling to previous load



  // Scrolling to next load >
  else if (command == IR_BUTTON_RIGHT && selectedLoad != IR_BUTTON_5) {
    int newLoad = Scroll(selectedLoad, 2);

    command = 0;  // Reset command so that the current command isn't unwantedly repeated

    StopReceiver();

    CustomDelay(remoteDisableTime);

    StartReceiver();

    selectedLoad = newLoad;
  }
  // < Scrolling to next load



  // Setting load timers >
  else if (command == IR_BUTTON_7) {
    SetTimer();
  }
  // < Setting load timers



  // Checking load's timer >
  else if (command == IR_BUTTON_9) {
    CheckTimer(selectedLoad);
  }
  // < Checking load's timer



  // Just retrieving load information >
  else if (command == IR_BUTTON_1 || command == IR_BUTTON_2 || command == IR_BUTTON_3 || command == IR_BUTTON_4 || command == IR_BUTTON_5) {
    RetrieveLoadInfo(command);
  }
  // < Just retrieving load information



  // Invalid commands >
  else if (command == IR_BUTTON_6 || command == IR_BUTTON_8 || command == IR_BUTTON_POUND || command == IR_BUTTON_ASTERISK) {
    InvalidCommand();

    RetrieveLoadInfo(selectedLoad);
  }
  // < Invalid commands

}  // loop()





void ReadCurrent() {

  // Resetting shouldResetReadCurrentMillis if appropriate >
  if (shouldResetReadCurrentMillis) {
    readCurrentMillis = millis();
    shouldResetReadCurrentMillis = false;
  }
  // < Resetting shouldResetReadCurrentMillis if appropriate



  // Creating a delay between current readings to speed up program execution >
  if (millis() < readCurrentMillis + TIME_BETWEEN_CURRENT_READS) {
    goto checkTotalCurrent;
  }
  // < Creating delay between current readings to speed up program execution



  shouldResetReadCurrentMillis = true;  // Reset shouldResetReadCurrentMillis to restart the timer



  // Reading currents >
startReadingCurrents:

  if (selector == 0) {
    sensorValue1 = ct1.calcIrms(NUM_OF_SAMPLES);
    selector++;

    if (!loadOneOff && selectedLoad == IR_BUTTON_1 && !setFunctionActive) {
      if (sensorValue1 != previousLoad1CurrentReading) {

        // Erase the second row to update current reading
        lcd.setCursor(0, 1);
        lcd.print("      ");

        // Update current reading
        lcd.setCursor(0, 1);
        lcd.print(sensorValue1);
        lcd.print("A");

        SetLoadPriorities();
        UpdateDisplayedLoadPriority();

        previousLoad1CurrentReading = sensorValue1;
      }

      else {
        SetLoadPriorities();
        UpdateDisplayedLoadPriority();
      }  // else

      PrintCurrentGraph();
    }  // if (!loadOneOff && selectedLoad == IR_BUTTON_1 && !setFunctionActive)

    else if (loadOneOff) {
      sensorValue1 = 0;
    }
  }  // if (selector == 0)



  else if (selector == 1) {
    sensorValue2 = ct2.calcIrms(NUM_OF_SAMPLES);
    selector++;

    if (!loadTwoOff && selectedLoad == IR_BUTTON_2 && !setFunctionActive) {
      if (sensorValue2 != previousLoad2CurrentReading) {

        // Erase the second row to update current reading
        lcd.setCursor(0, 1);
        lcd.print("      ");

        // Update current reading
        lcd.setCursor(0, 1);
        lcd.print(sensorValue2);
        lcd.print("A");

        SetLoadPriorities();
        UpdateDisplayedLoadPriority();

        previousLoad2CurrentReading = sensorValue2;
      }

      else {
        SetLoadPriorities();
        UpdateDisplayedLoadPriority();
      }  // else

      PrintCurrentGraph();
    }  // if (!loadTwoOff && selectedLoad == IR_BUTTON_2 && !setFunctionActive)

    else if (loadTwoOff) {
      sensorValue2 = 0;
    }
  }  // else if (selector == 1)



  else if (selector == 2) {
    sensorValue3 = ct3.calcIrms(NUM_OF_SAMPLES);
    selector++;

    if (!loadThreeOff && selectedLoad == IR_BUTTON_3 && !setFunctionActive) {
      if (sensorValue3 != previousLoad3CurrentReading) {

        // Erase the second row to update current reading
        lcd.setCursor(0, 1);
        lcd.print("      ");

        // Update current reading
        lcd.setCursor(0, 1);
        lcd.print(sensorValue3);
        lcd.print("A");

        SetLoadPriorities();
        UpdateDisplayedLoadPriority();

        previousLoad3CurrentReading = sensorValue3;
      }

      else {
        SetLoadPriorities();
        UpdateDisplayedLoadPriority();
      }  // else

      PrintCurrentGraph();
    }  // if (!loadThreeOff && selectedLoad == IR_BUTTON_3 && !setFunctionActive)

    else if (loadThreeOff) {
      sensorValue3 = 0;
    }
  }  // else if (selector == 2)



  else {
    sensorValue4 = ct4.calcIrms(NUM_OF_SAMPLES);
    selector = 0;

    if (!loadFourOff && selectedLoad == IR_BUTTON_4 && !setFunctionActive) {
      if (sensorValue4 != previousLoad4CurrentReading) {

        // Erase the second row to update current reading
        lcd.setCursor(0, 1);
        lcd.print("      ");

        // Update current reading
        lcd.setCursor(0, 1);
        lcd.print(sensorValue4);
        lcd.print("A");

        SetLoadPriorities();
        UpdateDisplayedLoadPriority();

        previousLoad4CurrentReading = sensorValue4;
      }

      else {
        SetLoadPriorities();
        UpdateDisplayedLoadPriority();
      }  // else

      PrintCurrentGraph();
    }  // if (!loadFourOff && selectedLoad == IR_BUTTON_4 && !setFunctionActive)

    else if (loadFourOff) {
      sensorValue4 = 0;
    }
  }  // else
  // < Reading currents


checkTotalCurrent:
  // Calculating total current being pulled by all loads & determining if loads need to be turned off due to exceeding capacity >
  total = sensorValue1 + sensorValue2 + sensorValue3 + sensorValue4;

  if (total > MAX_CURRENT) {

    SetLoadPriorities();

    if (loadOnePriority == 'L' && !loadOneOff) {
      loadOneOff = true;
      loadOneOffDueToExcessCurrent = true;

      //CheckLoadStatuses();

      goto startReadingCurrents;
    }

    else if (loadTwoPriority == 'L' && !loadTwoOff) {
      loadTwoOff = true;
      loadTwoOffDueToExcessCurrent = true;

      //CheckLoadStatuses();

      goto startReadingCurrents;
    }

    if (loadThreePriority == 'L' && !loadThreeOff) {
      loadThreeOff = true;
      loadThreeOffDueToExcessCurrent = true;

      //CheckLoadStatuses();

      goto startReadingCurrents;
    }

    if (loadFourPriority == 'L' && !loadFourOff) {
      loadFourOff = true;
      loadFourOffDueToExcessCurrent = true;

      //CheckLoadStatuses();

      goto startReadingCurrents;
    }

    CheckLoadStatuses();

    totalRecentlyExceeded = true;
  }  // if (total > MAX_CURRENT)

  // < Calculating total current being pulled by all loads & determining if loads need to be turned off due to exceeding capacity



  // Turning loads back on if they were turned off due to exceeding MAX_CURRENT >

  else {
    if (shouldResetTurnLoadsBackOnFromExcessCurrentMillis && totalRecentlyExceeded) {
      turnLoadsBackOnFromExcessCurrentMillis = millis();
      shouldResetTurnLoadsBackOnFromExcessCurrentMillis = false;
      totalRecentlyExceeded = false;

      return;
    }



    // Resetting shouldResetTurnLoadsBackOnFromExcessCurrentMillis if appropriate >
    if (millis() < turnLoadsBackOnFromExcessCurrentMillis + TIME_TO_TURN_LOADS_BACK_ON) {
      return;
    }
    // < Resetting shouldResetTurnLoadsBackOnFromExcessCurrentMillis if appropriate



    shouldResetTurnLoadsBackOnFromExcessCurrentMillis = true;

    if (loadOneOffDueToExcessCurrent) {
      loadOneOff = false;
      loadOneOffDueToExcessCurrent = false;

      if (selectedLoad == IR_BUTTON_1 && !setFunctionActive) {
        RetrieveLoadInfo(IR_BUTTON_1);  // Refresh the load screen
      }
    }

    if (loadTwoOffDueToExcessCurrent) {
      loadTwoOff = false;
      loadTwoOffDueToExcessCurrent = false;

      if (selectedLoad == IR_BUTTON_2 && !setFunctionActive) {
        RetrieveLoadInfo(IR_BUTTON_2);  // Refresh the load screen
      }
    }

    if (loadThreeOffDueToExcessCurrent) {
      loadThreeOff = false;
      loadThreeOffDueToExcessCurrent = false;

      if (selectedLoad == IR_BUTTON_3 && !setFunctionActive) {
        RetrieveLoadInfo(IR_BUTTON_3);  // Refresh the load screen
      }
    }

    if (loadFourOffDueToExcessCurrent) {
      loadFourOff = false;
      loadFourOffDueToExcessCurrent = false;

      if (selectedLoad == IR_BUTTON_4 && !setFunctionActive) {
        RetrieveLoadInfo(IR_BUTTON_4);  // Refresh the load screen
      }
    }
  }  // else

  // < Turning loads back on if they were turned off due to exceeding MAX_CURRENT

  CheckLoadStatuses();

}  // ReadCurrent()





void StartReceiver() {
  IrReceiver.begin(IR_PIN);
  receiverActive = true;
}  // StartReceiver()





void StopReceiver() {
  IrReceiver.end();
  receiverActive = false;
}  // StopReceiver()





void InvalidCommand() {
  int holdTime = 2000;  // Milliseconds

  StopReceiver();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Invalid Command");
  delay(holdTime);

  StartReceiver();

  return;
}  // InvalidCommand()





int ReceiveCommand() {
  unsigned long rawCommand = 0;
  char key = keypad.getKey();

  int finalCommand = -1;

  if (receiverActive) {
    if (IrReceiver.decode())  // If some data is received
    {
      IrReceiver.resume();

      rawCommand = IrReceiver.decodedIRData.command;

      // Prevents random IR signals from being accepted
      for (int i = 0; i < 17; i++) {
        if (rawCommand == remoteButtonList[i]) {
          finalCommand = rawCommand;
        }
      }
    }

    // Looking for a press on the keypad
    else if (key) {
      switch (key) {
        case '0':
          finalCommand = IR_BUTTON_0;
          break;

        case '1':
          finalCommand = IR_BUTTON_1;
          break;

        case '2':
          finalCommand = IR_BUTTON_2;
          break;

        case '3':
          finalCommand = IR_BUTTON_3;
          break;

        case '4':
          finalCommand = IR_BUTTON_4;
          break;

        case '5':
          finalCommand = IR_BUTTON_5;
          break;

        case '6':
          finalCommand = IR_BUTTON_6;
          break;

        case '7':
          finalCommand = IR_BUTTON_7;
          break;

        case '8':
          finalCommand = IR_BUTTON_8;
          break;

        case '9':
          finalCommand = IR_BUTTON_9;
          break;

        case 'U':
          finalCommand = IR_BUTTON_LEFT;
          break;

        case 'D':
          finalCommand = IR_BUTTON_RIGHT;
          break;

        case 'O':
          finalCommand = IR_BUTTON_ASTERISK;
          break;

        case 'C':
          finalCommand = IR_BUTTON_ASTERISK;
          break;

        case 'H':
          finalCommand = IR_BUTTON_ASTERISK;
          break;

        case 'E':
          finalCommand = IR_BUTTON_OK;
          break;
      }  // switch (key)
    }    // else if (key)
  }      // if (receiverActive);

  if (finalCommand != -1) {
    lcd.backlight();  // Turn the LCD's backlight ON

    lcdTimeout = 60;
    firstCheckLCDTimeoutRun = true;
  }

  return finalCommand;
}  // ReceiveCommand()





void CheckLCDTimeout() {

  if (firstCheckLCDTimeoutRun) {
    currentCheckLCDTimeoutMillis = millis();

    firstCheckLCDTimeoutRun = false;
  }

  if (millis() >= currentCheckLCDTimeoutMillis + 1000 && lcdTimeout > 0) {  // If 1 second has passed
    firstCheckLCDTimeoutRun = true;

    lcdTimeout--;
  }

  if (lcdTimeout <= 0) {
    lcd.noBacklight();
  }
}  // CheckLCDTimeout()





void CheckTimerStatuses() {

  // Checks if any load timers are currently active and marks them accordingly >
  if (loadOneHours > 0 || loadOneMinutes > 0 || loadOneSeconds > 0) {
    loadOneTimerActive = true;

    loadOneTimerRecentlyActive = true;
  } else {
    loadOneTimerActive = false;
  }

  if (loadTwoHours > 0 || loadTwoMinutes > 0 || loadTwoSeconds > 0) {
    loadTwoTimerActive = true;

    loadTwoTimerRecentlyActive = true;
  } else {
    loadTwoTimerActive = false;
  }

  if (loadThreeHours > 0 || loadThreeMinutes > 0 || loadThreeSeconds > 0) {
    loadThreeTimerActive = true;

    loadThreeTimerRecentlyActive = true;
  } else {
    loadThreeTimerActive = false;
  }

  if (loadFourHours > 0 || loadFourMinutes > 0 || loadFourSeconds > 0) {
    loadFourTimerActive = true;

    loadFourTimerRecentlyActive = true;
  } else {
    loadFourTimerActive = false;
  }
  // < Checks if any load timers are currently active and marks them accordingly



  // Run load timers if they're active >

  // Load 1 timer
  if (loadOneTimerActive) {
    Timer(1);
  } else if (!loadOneTimerActive && loadOneTimerRecentlyActive) {
    loadOneOff = !loadOneOff;
    loadOneTimerRecentlyActive = false;

    CheckLoadStatuses();


    // Refresh the load screen if it's currently being displayed
    if (selectedLoad == IR_BUTTON_1 && shouldRetrieveLoadOne && !setFunctionActive) {
      RetrieveLoadInfo(IR_BUTTON_1);
    }
  }



  // Load 2 timer
  if (loadTwoTimerActive) {
    Timer(2);
  } else if (!loadTwoTimerActive && loadTwoTimerRecentlyActive) {
    loadTwoOff = !loadTwoOff;
    loadTwoTimerRecentlyActive = false;

    CheckLoadStatuses();


    // Refresh the load screen if it's currently being displayed
    if (selectedLoad == IR_BUTTON_2 && shouldRetrieveLoadTwo && !setFunctionActive) {
      RetrieveLoadInfo(IR_BUTTON_2);
    }
  }



  // Load 3 timer
  if (loadThreeTimerActive) {
    Timer(3);
  } else if (!loadThreeTimerActive && loadThreeTimerRecentlyActive) {
    loadThreeOff = !loadThreeOff;
    loadThreeTimerRecentlyActive = false;

    CheckLoadStatuses();

    // Refresh the load screen if it's currently being displayed
    if (selectedLoad == IR_BUTTON_3 && shouldRetrieveLoadThree && !setFunctionActive) {
      RetrieveLoadInfo(IR_BUTTON_3);
    }
  }



  // Load 4 timer
  if (loadFourTimerActive) {
    Timer(4);
  } else if (!loadFourTimerActive && loadFourTimerRecentlyActive) {
    loadFourOff = !loadFourOff;
    loadFourTimerRecentlyActive = false;

    CheckLoadStatuses();

    // Refresh the load screen if it's currently being displayed
    if (selectedLoad == IR_BUTTON_4 && shouldRetrieveLoadFour && !setFunctionActive) {
      RetrieveLoadInfo(IR_BUTTON_4);
    }
  }
  // < Run load timers if they're active

}  // CheckTimerStatuses()





void CheckLoadStatuses() {
  loadsOut = 0;

  if (loadOneOff) {
    digitalWrite(RELAY_ONE_PIN, HIGH);
    numLoadsOut[0] = 0;

    loadOnePriority = 'H';

    sensorValue1 = 0;

    if (selectedLoad == IR_BUTTON_1 && shouldRetrieveLoadOne && !setFunctionActive) {
      shouldRetrieveLoadOne = false;

      RetrieveLoadInfo(IR_BUTTON_1);
    }
  } else {
    digitalWrite(RELAY_ONE_PIN, LOW);
    numLoadsOut[0] = 1;

    shouldRetrieveLoadOne = true;
  }



  if (loadTwoOff) {
    digitalWrite(RELAY_TWO_PIN, HIGH);
    numLoadsOut[1] = 0;

    loadTwoPriority = 'H';

    sensorValue2 = 0;

    if (selectedLoad == IR_BUTTON_2 && shouldRetrieveLoadTwo && !setFunctionActive) {
      shouldRetrieveLoadTwo = false;

      RetrieveLoadInfo(IR_BUTTON_2);
    }
  } else {
    digitalWrite(RELAY_TWO_PIN, LOW);
    numLoadsOut[1] = 1;

    shouldRetrieveLoadTwo = true;
  }



  if (loadThreeOff) {
    digitalWrite(RELAY_THREE_PIN, HIGH);
    numLoadsOut[2] = 0;

    loadThreePriority = 'H';

    sensorValue3 = 0;

    if (selectedLoad == IR_BUTTON_3 && shouldRetrieveLoadThree && !setFunctionActive) {
      shouldRetrieveLoadThree = false;

      RetrieveLoadInfo(IR_BUTTON_3);
    }
  } else {
    digitalWrite(RELAY_THREE_PIN, LOW);
    numLoadsOut[2] = 1;

    shouldRetrieveLoadThree = true;
  }



  if (loadFourOff) {
    digitalWrite(RELAY_FOUR_PIN, HIGH);
    numLoadsOut[3] = 0;

    loadFourPriority = 'H';

    sensorValue4 = 0;

    if (selectedLoad == IR_BUTTON_4 && shouldRetrieveLoadFour && !setFunctionActive) {
      shouldRetrieveLoadFour = false;

      RetrieveLoadInfo(IR_BUTTON_4);
    }
  } else {
    digitalWrite(RELAY_FOUR_PIN, LOW);
    numLoadsOut[3] = 1;

    shouldRetrieveLoadFour = true;
  }



  // Counting how many loads are ON >
  for (int i = 0; i < 4; i++) {
    if (numLoadsOut[i] == 1) {
      loadsOut++;
    }
  }
  // < Counting how many loads are ON

}  // CheckLoadStatuses()





void CheckTimer(int load) {
  setFunctionActive = true;

  unsigned long currentMillis = millis();

  int stayTime = 3000;  // How long the program will stay in CheckTimer() in milliseconds

  bool firstRun = true;

  int previousHour;
  int previousMinute;
  int previousSecond;

  if (load == IR_BUTTON_1 || load == IR_BUTTON_2 || load == IR_BUTTON_3 || load == IR_BUTTON_4) {
    while (millis() < currentMillis + stayTime) {
      StopReceiver();

      CheckLoadStatuses();

      SetLoadPriorities();

      ReadCurrent();



      switch (load) {
        case IR_BUTTON_1:

          // Checks if any load timers are currently active and marks them accordingly
          if ((loadOneHours > 0 || loadOneMinutes > 0 || loadOneSeconds > 0)) {
            loadOneTimerActive = true;

            loadOneTimerRecentlyActive = true;
          } else {
            loadOneTimerActive = false;
          }

          if (loadOneTimerActive) {
            if (firstRun) {
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("L1 T Remaining:");

              lcd.setCursor(4, 1);

              if (loadOneHours < 10) {
                lcd.print("0");
                lcd.print(loadOneHours);
              } else {
                lcd.print(loadOneHours);
              }

              lcd.print(":");

              if (loadOneMinutes < 10) {
                lcd.print("0");
                lcd.print(loadOneMinutes);
              } else {
                lcd.print(loadOneMinutes);
              }

              lcd.print(":");

              if (loadOneSeconds < 10) {
                lcd.print("0");
                lcd.print(loadOneSeconds);
              } else {
                lcd.print(loadOneSeconds);
              }

              firstRun = false;
            }  // if (firstRun)

            previousHour = loadOneHours;
            previousMinute = loadOneMinutes;
            previousSecond = loadOneSeconds;
            Timer(1);

            if (loadOneHours != previousHour || loadOneMinutes != previousMinute || loadOneSeconds != previousSecond) {
              lcd.setCursor(4, 1);

              if (loadOneHours < 10) {
                lcd.print("0");
                lcd.print(loadOneHours);
              } else {
                lcd.print(loadOneHours);
              }

              lcd.print(":");

              if (loadOneMinutes < 10) {
                lcd.print("0");
                lcd.print(loadOneMinutes);
              } else {
                lcd.print(loadOneMinutes);
              }

              lcd.print(":");

              if (loadOneSeconds < 10) {
                lcd.print("0");
                lcd.print(loadOneSeconds);
              } else {
                lcd.print(loadOneSeconds);
              }
            }  // if (loadOneHours != previousHour || loadOneMinutes != previousMinute || loadOneSeconds != previousSecond)
          }    // if (loadOneTimerActive)

          else if (!loadOneTimerActive && loadOneTimerRecentlyActive) {
            goto endCheckTimer;
          }

          else if (!loadOneTimerActive && !loadOneTimerRecentlyActive) {
            if (firstRun) {
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("L1 Timer is");
              lcd.setCursor(0, 1);
              lcd.print("Inactive!");

              firstRun = false;
            }
          }

          break;  // Break out of case IR_BUTTON_1



        case IR_BUTTON_2:

          // Checks if any load timers are currently active and marks them accordingly
          if ((loadTwoHours > 0 || loadTwoMinutes > 0 || loadTwoSeconds > 0)) {
            loadTwoTimerActive = true;

            loadTwoTimerRecentlyActive = true;
          } else {
            loadTwoTimerActive = false;
          }

          if (loadTwoTimerActive) {
            if (firstRun) {
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("L2 T Remaining:");

              lcd.setCursor(4, 1);

              if (loadTwoHours < 10) {
                lcd.print("0");
                lcd.print(loadTwoHours);
              } else {
                lcd.print(loadTwoHours);
              }

              lcd.print(":");

              if (loadTwoMinutes < 10) {
                lcd.print("0");
                lcd.print(loadTwoMinutes);
              } else {
                lcd.print(loadTwoMinutes);
              }

              lcd.print(":");

              if (loadTwoSeconds < 10) {
                lcd.print("0");
                lcd.print(loadTwoSeconds);
              } else {
                lcd.print(loadTwoSeconds);
              }

              firstRun = false;
            }  // if (firstRun)

            previousHour = loadTwoHours;
            previousMinute = loadTwoMinutes;
            previousSecond = loadTwoSeconds;
            Timer(2);

            if (loadTwoHours != previousHour || loadTwoMinutes != previousMinute || loadTwoSeconds != previousSecond) {
              lcd.setCursor(4, 1);

              if (loadTwoHours < 10) {
                lcd.print("0");
                lcd.print(loadTwoHours);
              } else {
                lcd.print(loadTwoHours);
              }

              lcd.print(":");

              if (loadTwoMinutes < 10) {
                lcd.print("0");
                lcd.print(loadTwoMinutes);
              } else {
                lcd.print(loadTwoMinutes);
              }

              lcd.print(":");

              if (loadTwoSeconds < 10) {
                lcd.print("0");
                lcd.print(loadTwoSeconds);
              } else {
                lcd.print(loadTwoSeconds);
              }
            }  // if (loadTwoHours != previousHour || loadTwoMinutes != previousMinute || loadTwoSeconds != previousSecond)
          }    // if (loadTwoTimerActive)

          else if (!loadTwoTimerActive && loadTwoTimerRecentlyActive) {
            goto endCheckTimer;
          }

          else if (!loadTwoTimerActive && !loadTwoTimerRecentlyActive) {
            if (firstRun) {
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("L2 Timer is");
              lcd.setCursor(0, 1);
              lcd.print("Inactive!");

              firstRun = false;
            }
          }

          break;  // Break out of case IR_BUTTON_2



        case IR_BUTTON_3:
          // Checks if any load timers are currently active and marks them accordingly
          if (loadThreeHours > 0 || loadThreeMinutes > 0 || loadThreeSeconds > 0) {
            loadThreeTimerActive = true;

            loadThreeTimerRecentlyActive = true;
          } else {
            loadThreeTimerActive = false;
          }

          if (loadThreeTimerActive) {
            if (firstRun) {
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("L3 T Remaining:");

              lcd.setCursor(4, 1);

              if (loadThreeHours < 10) {
                lcd.print("0");
                lcd.print(loadThreeHours);
              } else {
                lcd.print(loadThreeHours);
              }

              lcd.print(":");

              if (loadThreeMinutes < 10) {
                lcd.print("0");
                lcd.print(loadThreeMinutes);
              } else {
                lcd.print(loadThreeMinutes);
              }

              lcd.print(":");

              if (loadThreeSeconds < 10) {
                lcd.print("0");
                lcd.print(loadThreeSeconds);
              } else {
                lcd.print(loadThreeSeconds);
              }

              firstRun = false;
            }  // if (firstRun)

            previousHour = loadThreeHours;
            previousMinute = loadThreeMinutes;
            previousSecond = loadThreeSeconds;
            Timer(3);

            if (loadThreeHours != previousHour || loadThreeMinutes != previousMinute || loadThreeSeconds != previousSecond) {
              lcd.setCursor(4, 1);

              if (loadThreeHours < 10) {
                lcd.print("0");
                lcd.print(loadThreeHours);
              } else {
                lcd.print(loadThreeHours);
              }

              lcd.print(":");

              if (loadThreeMinutes < 10) {
                lcd.print("0");
                lcd.print(loadThreeMinutes);
              } else {
                lcd.print(loadThreeMinutes);
              }

              lcd.print(":");

              if (loadThreeSeconds < 10) {
                lcd.print("0");
                lcd.print(loadThreeSeconds);
              } else {
                lcd.print(loadThreeSeconds);
              }
            }  // if (loadThreeHours != previousHour || loadThreeMinutes != previousMinute || loadThreeSeconds != previousSecond)
          }    // if (loadThreeTimerActive)

          else if (!loadThreeTimerActive && loadThreeTimerRecentlyActive) {
            goto endCheckTimer;
          }

          else if (!loadThreeTimerActive && !loadThreeTimerRecentlyActive) {
            if (firstRun) {
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("L3 Timer is");
              lcd.setCursor(0, 1);
              lcd.print("Inactive!");

              firstRun = false;
            }
          }

          break;  // Break out of case IR_BUTTON_3



        case IR_BUTTON_4:
          // Checks if any load timers are currently active and marks them accordingly
          if (loadFourHours > 0 || loadFourMinutes > 0 || loadFourSeconds > 0) {
            loadFourTimerActive = true;

            loadFourTimerRecentlyActive = true;
          } else {
            loadFourTimerActive = false;
          }

          if (loadFourTimerActive) {
            if (firstRun) {
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("L4 T Remaining:");

              lcd.setCursor(4, 1);

              if (loadFourHours < 10) {
                lcd.print("0");
                lcd.print(loadFourHours);
              } else {
                lcd.print(loadFourHours);
              }

              lcd.print(":");

              if (loadFourMinutes < 10) {
                lcd.print("0");
                lcd.print(loadFourMinutes);
              } else {
                lcd.print(loadFourMinutes);
              }

              lcd.print(":");

              if (loadFourSeconds < 10) {
                lcd.print("0");
                lcd.print(loadFourSeconds);
              } else {
                lcd.print(loadFourSeconds);
              }

              firstRun = false;
            }  // if (firstRun)

            previousHour = loadFourHours;
            previousMinute = loadFourMinutes;
            previousSecond = loadFourSeconds;
            Timer(4);

            if (loadFourHours != previousHour || loadFourMinutes != previousMinute || loadFourSeconds != previousSecond) {
              lcd.setCursor(4, 1);

              if (loadFourHours < 10) {
                lcd.print("0");
                lcd.print(loadFourHours);
              } else {
                lcd.print(loadFourHours);
              }

              lcd.print(":");

              if (loadFourMinutes < 10) {
                lcd.print("0");
                lcd.print(loadFourMinutes);
              } else {
                lcd.print(loadFourMinutes);
              }

              lcd.print(":");

              if (loadFourSeconds < 10) {
                lcd.print("0");
                lcd.print(loadFourSeconds);
              } else {
                lcd.print(loadFourSeconds);
              }
            }  // if (loadFourHours != previousHour || loadFourMinutes != previousMinute || loadFourSeconds != previousSecond)
          }    // if (loadFourTimerActive)

          else if (!loadFourTimerActive && loadFourTimerRecentlyActive) {
            goto endCheckTimer;
          }

          else if (!loadFourTimerActive && !loadFourTimerRecentlyActive) {
            if (firstRun) {
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("L4 Timer is");
              lcd.setCursor(0, 1);
              lcd.print("Inactive!");

              firstRun = false;
            }
          }

          break;  // Break out of case IR_BUTTON_4
      }           // switch (load)
    }             // while (millis() < currentMillis + stayTime)
  }

endCheckTimer:
  StartReceiver();

  setFunctionActive = false;

  RetrieveLoadInfo(load);
}  // CheckTimer()





void Timer(int load) {
  switch (load) {

    // Load 1
    case 1:
      if (firstLoadOneTimerRun) {
        currentMillisLoadOne = millis();

        firstLoadOneTimerRun = false;
      }

      if (millis() >= currentMillisLoadOne + 1000) {
        firstLoadOneTimerRun = true;  // Reset firstTimerRun

        if (loadOneSeconds > 0) {
          loadOneSeconds--;
        }

        else if (loadOneMinutes > 0 && loadOneSeconds == 0) {
          loadOneSeconds = 59;

          loadOneMinutes--;
        }

        else if (loadOneHours > 0 && loadOneMinutes == 0 && loadOneSeconds == 0) {
          loadOneMinutes = 59;
          loadOneSeconds = 59;

          loadOneHours--;
        }
      }

      break;  // Break out of case 1

    // Load 2
    case 2:
      if (firstLoadTwoTimerRun) {
        currentMillisLoadTwo = millis();

        firstLoadTwoTimerRun = false;
      }

      if (millis() >= currentMillisLoadTwo + 1000) {
        firstLoadTwoTimerRun = true;  // Reset firstTimerRun

        if (loadTwoSeconds > 0) {
          loadTwoSeconds--;
        }

        else if (loadTwoMinutes > 0 && loadTwoSeconds == 0) {
          loadTwoSeconds = 59;

          loadTwoMinutes--;
        }

        else if (loadTwoHours > 0 && loadTwoMinutes == 0 && loadTwoSeconds == 0) {
          loadTwoMinutes = 59;
          loadTwoSeconds = 59;

          loadTwoHours--;
        }
      }

      break;  // Break out of case 2

    // Load 3
    case 3:
      if (firstLoadThreeTimerRun) {
        currentMillisLoadThree = millis();

        firstLoadThreeTimerRun = false;
      }

      if (millis() >= currentMillisLoadThree + 1000) {
        firstLoadThreeTimerRun = true;  // Reset firstTimerRun

        if (loadThreeSeconds > 0) {
          loadThreeSeconds--;
        }

        else if (loadThreeMinutes > 0 && loadThreeSeconds == 0) {
          loadThreeSeconds = 59;

          loadThreeMinutes--;
        }

        else if (loadThreeHours > 0 && loadThreeMinutes == 0 && loadThreeSeconds == 0) {
          loadThreeMinutes = 59;
          loadThreeSeconds = 59;

          loadThreeHours--;
        }
      }

      break;  // Break out of case 3

    // Load 4
    case 4:
      if (firstLoadFourTimerRun) {
        currentMillisLoadFour = millis();

        firstLoadFourTimerRun = false;
      }

      if (millis() >= currentMillisLoadFour + 1000) {
        firstLoadFourTimerRun = true;  // Reset firstTimerRun

        if (loadFourSeconds > 0) {
          loadFourSeconds--;
        }

        else if (loadFourMinutes > 0 && loadFourSeconds == 0) {
          loadFourSeconds = 59;

          loadFourMinutes--;
        }

        else if (loadFourHours > 0 && loadFourMinutes == 0 && loadFourSeconds == 0) {
          loadFourMinutes = 59;
          loadFourSeconds = 59;

          loadFourHours--;
        }
      }

      break;  // Break out of case 4
  }
}  // Timer()





void SetTimer() {
  setFunctionActive = true;

  int command;

  int delayTime = 2000;                  // In milliseconds
  int errorDisplayTime = 3000;           // In milliseconds
  int warningMessageDisplayTime = 2000;  // In milliseconds
  int blinkTime = 1000;                  // In milliseconds

  int i;
  int column;
  int temp;
  int buttonPressed = 0;
  int actualSelectedLoad;

  bool breakOutOfWhileLoop = 0;

  String loadOneHoursString = "00";
  String loadOneMinutesString = "00";
  String loadOneSecondsString = "00";
  String loadTwoHoursString = "00";
  String loadTwoMinutesString = "00";
  String loadTwoSecondsString = "00";
  String loadThreeHoursString = "00";
  String loadThreeMinutesString = "00";
  String loadThreeSecondsString = "00";
  String loadFourHoursString = "00";
  String loadFourMinutesString = "00";
  String loadFourSecondsString = "00";

  StopReceiver();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Set Load");
  lcd.setCursor(0, 1);
  lcd.print("Timer");

  CustomDelay(delayTime);

  StartReceiver();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Select Load");
  lcd.setCursor(0, 1);
  lcd.print("Load: ");



  // Selecting a load >
  while (true) {

    // Check load timer statuses
    CheckTimerStatuses();

    // Automatically set load priorities
    SetLoadPriorities();

    // Read load currents
    ReadCurrent();

    // Turn the LCD OFF if no command is pressed within a certain amount of time
    CheckLCDTimeout();

    command = ReceiveCommand();

    for (i = 0; i < 4; i++) {
      if (command == loadList[i]) {
        selectedLoad = command;
        buttonPressed = i + 1;
        actualSelectedLoad = buttonPressed;

        lcd.setCursor(6, 1);
        lcd.print(buttonPressed);

        breakOutOfWhileLoop = 1;

        StopReceiver();

        CustomDelay(delayTime);

        StartReceiver();



        // Checking if the selected load's timer is already active >
        switch (actualSelectedLoad) {

          // Load 1
          case 1:
            if (loadOneTimerActive) {

              StopReceiver();

              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Load 1's timer");
              ScrollText("is currently active.", 1);

              CustomDelay(warningMessageDisplayTime);

              lcd.clear();
              lcd.setCursor(0, 0);
              ScrollText("Continuing will override", 0);
              lcd.setCursor(0, 1);
              ScrollText("Load 1's current timer.", 1);

              CustomDelay(warningMessageDisplayTime);

              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Continue?");

              StartReceiver();

              while (true) {
                command = CustomDelay(1000);

                if (command == IR_BUTTON_1) {
                  loadOneHours = 0;
                  loadOneMinutes = 0;
                  loadOneSeconds = 0;

                  loadOneTimerActive = false;
                  loadOneTimerRecentlyActive = false;

                  selectedLoad = IR_BUTTON_1;  // selctedLoad was set to -1 in ScrollText(), so it needs to be set appropriately again here

                  break;  // Break out of while (true)
                }

                else if (command == IR_BUTTON_0) {
                  StopReceiver();

                  lcd.clear();
                  lcd.setCursor(0, 0);
                  lcd.print("Command");
                  lcd.setCursor(0, 1);
                  lcd.print("Cancelled");

                  CustomDelay(warningMessageDisplayTime);

                  StartReceiver();

                  selectedLoad = IR_BUTTON_1;  // selctedLoad was set to -1 in ScrollText(), so it needs to be set appropriately again here

                  goto endOfSetTimer;  // Skip to the end of SetTimer()
                }

                command = ScrollText("1 to continue, 0 to cancel", 1);

                if (command == IR_BUTTON_1) {
                  loadOneHours = 0;
                  loadOneMinutes = 0;
                  loadOneSeconds = 0;

                  loadOneTimerActive = false;
                  loadOneTimerRecentlyActive = false;

                  selectedLoad = IR_BUTTON_1;  // selctedLoad was set to -1 in ScrollText(), so it needs to be set appropriately again here

                  break;  // Break out of while (true)
                }

                else if (command == IR_BUTTON_0) {
                  StopReceiver();

                  lcd.clear();
                  lcd.setCursor(0, 0);
                  lcd.print("Command");
                  lcd.setCursor(0, 1);
                  lcd.print("Cancelled");

                  CustomDelay(warningMessageDisplayTime);

                  StartReceiver();

                  selectedLoad = IR_BUTTON_1;  // selctedLoad was set to -1 in ScrollText(), so it needs to be set appropriately again here

                  goto endOfSetTimer;  // Skip to the end of SetTimer()
                }

                command = CustomDelay(1000);

                if (command == IR_BUTTON_1) {
                  loadOneHours = 0;
                  loadOneMinutes = 0;
                  loadOneSeconds = 0;

                  loadOneTimerActive = false;
                  loadOneTimerRecentlyActive = false;

                  selectedLoad = IR_BUTTON_1;  // selctedLoad was set to -1 in ScrollText(), so it needs to be set appropriately again here

                  break;  // Break out of while (true)
                }

                else if (command == IR_BUTTON_0) {
                  StopReceiver();

                  lcd.clear();
                  lcd.setCursor(0, 0);
                  lcd.print("Command");
                  lcd.setCursor(0, 1);
                  lcd.print("Cancelled");

                  CustomDelay(warningMessageDisplayTime);

                  StartReceiver();

                  selectedLoad = IR_BUTTON_1;  // selctedLoad was set to -1 in ScrollText(), so it needs to be set appropriately again here

                  goto endOfSetTimer;  // Skip to the end of SetTimer()
                }

                // Clear the bottom row of the screen to repeat
                lcd.setCursor(0, 1);
                lcd.print("                ");
              }  // while (true)
            }    // if (loadOneTimerActive)

            break;  // Break out of case 1

          // Load 2
          case 2:
            if (loadTwoTimerActive) {

              StopReceiver();

              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Load 2's timer");
              ScrollText("is currently active.", 1);

              CustomDelay(warningMessageDisplayTime);

              lcd.clear();
              lcd.setCursor(0, 0);
              ScrollText("Continuing will override", 0);
              lcd.setCursor(0, 1);
              ScrollText("Load 2's current timer.", 1);

              CustomDelay(warningMessageDisplayTime);

              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Continue?");

              StartReceiver();

              while (true) {
                command = CustomDelay(1000);

                if (command == IR_BUTTON_1) {
                  loadTwoHours = 0;
                  loadTwoMinutes = 0;
                  loadTwoSeconds = 0;

                  loadTwoTimerActive = false;
                  loadTwoTimerRecentlyActive = false;

                  selectedLoad = IR_BUTTON_2;  // selectedLoad was set to -1 in ScrollText(), so it needs to be set appropriately again here

                  break;  // Break out of while (true)
                }

                else if (command == IR_BUTTON_0) {
                  StopReceiver();

                  lcd.clear();
                  lcd.setCursor(0, 0);
                  lcd.print("Command");
                  lcd.setCursor(0, 1);
                  lcd.print("Cancelled");

                  CustomDelay(warningMessageDisplayTime);

                  StartReceiver();

                  selectedLoad = IR_BUTTON_2;  // selectedLoad was set to -1 in ScrollText(), so it needs to be set appropriately again here

                  goto endOfSetTimer;  // Skip to the end of SetTimer()
                }

                command = ScrollText("1 to continue, 0 to cancel", 1);

                if (command == IR_BUTTON_1) {
                  loadTwoHours = 0;
                  loadTwoMinutes = 0;
                  loadTwoSeconds = 0;

                  loadTwoTimerActive = false;
                  loadTwoTimerRecentlyActive = false;

                  selectedLoad = IR_BUTTON_2;  // selectedLoad was set to -1 in ScrollText(), so it needs to be set appropriately again here

                  break;  // Break out of while (true)
                }

                else if (command == IR_BUTTON_0) {
                  StopReceiver();

                  lcd.clear();
                  lcd.setCursor(0, 0);
                  lcd.print("Command");
                  lcd.setCursor(0, 1);
                  lcd.print("Cancelled");

                  CustomDelay(warningMessageDisplayTime);

                  StartReceiver();

                  selectedLoad = IR_BUTTON_2;  // selectedLoad was set to -1 in ScrollText(), so it needs to be set appropriately again here

                  goto endOfSetTimer;  // Skip to the end of SetTimer()
                }

                command = CustomDelay(1000);

                if (command == IR_BUTTON_1) {
                  loadTwoHours = 0;
                  loadTwoMinutes = 0;
                  loadTwoSeconds = 0;

                  loadTwoTimerActive = false;
                  loadTwoTimerRecentlyActive = false;

                  selectedLoad = IR_BUTTON_2;  // selectedLoad was set to -1 in ScrollText(), so it needs to be set appropriately again here

                  break;  // Break out of while (true)
                }

                else if (command == IR_BUTTON_0) {
                  StopReceiver();

                  lcd.clear();
                  lcd.setCursor(0, 0);
                  lcd.print("Command");
                  lcd.setCursor(0, 1);
                  lcd.print("Cancelled");

                  CustomDelay(warningMessageDisplayTime);

                  StartReceiver();

                  selectedLoad = IR_BUTTON_2;  // selectedLoad was set to -1 in ScrollText(), so it needs to be set appropriately again here

                  goto endOfSetTimer;  // Skip to the end of SetTimer()
                }

                // Clear the bottom row of the screen to repeat
                lcd.setCursor(0, 1);
                lcd.print("                ");
              }  // while (true)
            }    // if (loadOneTimerActive)

            break;  // Break out of case 2

          // Load 3
          case 3:
            if (loadThreeTimerActive) {

              StopReceiver();

              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Load 3's timer");
              ScrollText("is currently active.", 1);

              CustomDelay(warningMessageDisplayTime);

              lcd.clear();
              lcd.setCursor(0, 0);
              ScrollText("Continuing will override", 0);
              lcd.setCursor(0, 1);
              ScrollText("Load 3's current timer.", 1);

              CustomDelay(warningMessageDisplayTime);

              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Continue?");

              StartReceiver();

              while (true) {
                command = CustomDelay(1000);

                if (command == IR_BUTTON_1) {
                  loadThreeHours = 0;
                  loadThreeMinutes = 0;
                  loadThreeSeconds = 0;

                  loadThreeTimerActive = false;
                  loadThreeTimerRecentlyActive = false;

                  selectedLoad = IR_BUTTON_3;  // selectedLoad was set to -1 in ScrollText(), so it needs to be set appropriately again here

                  break;  // Break out of while (true)
                }

                else if (command == IR_BUTTON_0) {
                  StopReceiver();

                  lcd.clear();
                  lcd.setCursor(0, 0);
                  lcd.print("Command");
                  lcd.setCursor(0, 1);
                  lcd.print("Cancelled");

                  CustomDelay(warningMessageDisplayTime);

                  StartReceiver();

                  selectedLoad = IR_BUTTON_3;  // selectedLoad was set to -1 in ScrollText(), so it needs to be set appropriately again here

                  goto endOfSetTimer;  // Skip to the end of SetTimer()
                }

                command = ScrollText("1 to continue, 0 to cancel", 1);

                if (command == IR_BUTTON_1) {
                  loadThreeHours = 0;
                  loadThreeMinutes = 0;
                  loadThreeSeconds = 0;

                  loadThreeTimerActive = false;
                  loadThreeTimerRecentlyActive = false;

                  selectedLoad = IR_BUTTON_3;  // selctedLoad was set to -1 in ScrollText(), so it needs to be set appropriately again here

                  break;  // Break out of while (true)
                }

                else if (command == IR_BUTTON_0) {
                  StopReceiver();

                  lcd.clear();
                  lcd.setCursor(0, 0);
                  lcd.print("Command");
                  lcd.setCursor(0, 1);
                  lcd.print("Cancelled");

                  CustomDelay(warningMessageDisplayTime);

                  StartReceiver();

                  selectedLoad = IR_BUTTON_3;  // selectedLoad was set to -1 in ScrollText(), so it needs to be set appropriately again here

                  goto endOfSetTimer;  // Skip to the end of SetTimer()
                }

                command = CustomDelay(1000);

                if (command == IR_BUTTON_1) {
                  loadThreeHours = 0;
                  loadThreeMinutes = 0;
                  loadThreeSeconds = 0;

                  loadThreeTimerActive = false;
                  loadThreeTimerRecentlyActive = false;

                  selectedLoad = IR_BUTTON_3;  // selctedLoad was set to -1 in ScrollText(), so it needs to be set appropriately again here

                  break;  // Break out of while (true)
                }

                else if (command == IR_BUTTON_0) {
                  StopReceiver();

                  lcd.clear();
                  lcd.setCursor(0, 0);
                  lcd.print("Command");
                  lcd.setCursor(0, 1);
                  lcd.print("Cancelled");

                  CustomDelay(warningMessageDisplayTime);

                  StartReceiver();

                  selectedLoad = IR_BUTTON_3;  // selectedLoad was set to -1 in ScrollText(), so it needs to be set appropriately again here

                  goto endOfSetTimer;  // Skip to the end of SetTimer()
                }

                // Clear the bottom row of the screen to repeat
                lcd.setCursor(0, 1);
                lcd.print("                ");
              }  // while (true)
            }    // if (loadOneTimerActive)

            break;  // Break out of case 3

          // Load 4
          case 4:
            if (loadFourTimerActive) {

              StopReceiver();

              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Load 4's timer");
              ScrollText("is currently active.", 1);

              CustomDelay(warningMessageDisplayTime);

              lcd.clear();
              lcd.setCursor(0, 0);
              ScrollText("Continuing will override", 0);
              lcd.setCursor(0, 1);
              ScrollText("Load 4's current timer.", 1);

              CustomDelay(warningMessageDisplayTime);

              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Continue?");

              StartReceiver();

              while (true) {
                command = CustomDelay(1000);

                if (command == IR_BUTTON_1) {
                  loadFourHours = 0;
                  loadFourMinutes = 0;
                  loadFourSeconds = 0;

                  loadFourTimerActive = false;
                  loadFourTimerRecentlyActive = false;

                  selectedLoad = IR_BUTTON_4;  // selectedLoad was set to -1 in ScrollText(), so it needs to be set appropriately again here

                  break;  // Break out of while (true)
                }

                else if (command == IR_BUTTON_0) {
                  StopReceiver();

                  lcd.clear();
                  lcd.setCursor(0, 0);
                  lcd.print("Command");
                  lcd.setCursor(0, 1);
                  lcd.print("Cancelled");

                  CustomDelay(warningMessageDisplayTime);

                  StartReceiver();

                  selectedLoad = IR_BUTTON_4;  // selectedLoad was set to -1 in ScrollText(), so it needs to be set appropriately again here

                  goto endOfSetTimer;  // Skip to the end of SetTimer()
                }

                command = ScrollText("1 to continue, 0 to cancel", 1);

                if (command == IR_BUTTON_1) {
                  loadFourHours = 0;
                  loadFourMinutes = 0;
                  loadFourSeconds = 0;

                  loadFourTimerActive = false;
                  loadFourTimerRecentlyActive = false;

                  selectedLoad = IR_BUTTON_4;  // selctedLoad was set to -1 in ScrollText(), so it needs to be set appropriately again here

                  break;  // Break out of while (true)
                }

                else if (command == IR_BUTTON_0) {
                  StopReceiver();

                  lcd.clear();
                  lcd.setCursor(0, 0);
                  lcd.print("Command");
                  lcd.setCursor(0, 1);
                  lcd.print("Cancelled");

                  CustomDelay(warningMessageDisplayTime);

                  StartReceiver();

                  selectedLoad = IR_BUTTON_4;  // selectedLoad was set to -1 in ScrollText(), so it needs to be set appropriately again here

                  goto endOfSetTimer;  // Skip to the end of SetTimer()
                }

                command = CustomDelay(1000);

                if (command == IR_BUTTON_1) {
                  loadFourHours = 0;
                  loadFourMinutes = 0;
                  loadFourSeconds = 0;

                  loadFourTimerActive = false;
                  loadFourTimerRecentlyActive = false;

                  selectedLoad = IR_BUTTON_4;  // selctedLoad was set to -1 in ScrollText(), so it needs to be set appropriately again here

                  break;  // Break out of while (true)
                }

                else if (command == IR_BUTTON_0) {
                  StopReceiver();

                  lcd.clear();
                  lcd.setCursor(0, 0);
                  lcd.print("Command");
                  lcd.setCursor(0, 1);
                  lcd.print("Cancelled");

                  CustomDelay(warningMessageDisplayTime);

                  StartReceiver();

                  selectedLoad = IR_BUTTON_4;  // selectedLoad was set to -1 in ScrollText(), so it needs to be set appropriately again here

                  goto endOfSetTimer;  // Skip to the end of SetTimer()
                }

                // Clear the bottom row of the screen to repeat
                lcd.setCursor(0, 1);
                lcd.print("                ");
              }  // while (true)
            }    // if (loadOneTimerActive)

            break;  // Break out of case 4
        }           // switch (actualSelectedLoad)
        // < Checking if the selected load's timer is already active



      }  // if (command == loadList[i])
    }    // for (i = 0; i < 4; i++)

    if (breakOutOfWhileLoop) {
      breakOutOfWhileLoop = 0;

      break;  // Break out of while loop
    }

    // Checks if the user pressed 0
    if (command == remoteButtonList[0]) {
      StopReceiver();

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select a load");
      lcd.setCursor(0, 1);
      lcd.print("1-4");

      command = -1;

      CustomDelay(errorDisplayTime);

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select Load");
      lcd.setCursor(0, 1);
      lcd.print("Load: ");

      StartReceiver();
    }



    // Checks if the user presses a button other than 0 or 1-4 >
    for (i = 5; i < 17; i++) {
      if (command == remoteButtonList[i]) {
        StopReceiver();

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Select a load");
        lcd.setCursor(0, 1);
        lcd.print("1-4");

        command = -1;

        CustomDelay(errorDisplayTime);

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Select Load");
        lcd.setCursor(0, 1);
        lcd.print("Load: ");

        StartReceiver();
      }
    }  // for (i = 5; i < 17; i++)
    // < Checks if the user presses a button other than 0 or 1-4



  }  // while (true)
  // < Selecting a load



  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Load ");
  lcd.print(actualSelectedLoad);
  lcd.print(" Timer");

  lcd.setCursor(4, 1);
  lcd.print("00:00:00");

  StopReceiver();

  CustomDelay(blinkTime);

  StartReceiver();

  column = 4;
  buttonPressed = -1;

  // Setting the load's timer
  while (true) {
    lcd.setCursor(column, 1);
    lcd.print(" ");

    command = CustomDelay(blinkTime);

    // Checking if the user pressed a button while in CustomDelay() & matching it to remoteButtonList
    if (command > 0) {
      for (i = 0; i < 17; i++) {
        if (command == remoteButtonList[i]) {
          buttonPressed = i;
        }
      }
    }



    // If the user has not pressed a button yet, keep displaying "0" >
    if (buttonPressed < 0) {
      lcd.setCursor(column, 1);
      lcd.print("0");

      command = CustomDelay(blinkTime);

      // Checking if the user pressed a button while in CustomDelay() & matching it to remoteButtonList
      if (command > 0) {
        for (i = 0; i < 17; i++) {
          if (command == remoteButtonList[i]) {
            buttonPressed = i;

            goto showButton;  // Skip the blink at the beginning of the loop
          }
        }
      }
    }
    // < If the user has not pressed a button yet, keep displaying "0"



    // If the user has pressed a button, check if it's valid >
    else {
showButton:

      // If the user tries to input minutes or seconds greater than 59, don't allow it
      if ((column == 7 || column == 10) && (buttonPressed == 6 || buttonPressed == 7 || buttonPressed == 8 || buttonPressed == 9 || buttonPressed == 10 || buttonPressed == 13 || buttonPressed == 14 || buttonPressed == 15 || buttonPressed == 16)) {
        StopReceiver();

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Press a button");
        lcd.setCursor(0, 1);
        lcd.print("0-5");

        CustomDelay(errorDisplayTime);

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Load ");
        lcd.print(actualSelectedLoad);
        lcd.print(" Timer");
        lcd.setCursor(4, 1);

        switch (selectedLoad) {
          case IR_BUTTON_1:
            lcd.print(loadOneHoursString);
            lcd.print(":");
            lcd.print(loadOneMinutesString);
            lcd.print(":");
            lcd.print(loadOneSecondsString);
            break;

          case IR_BUTTON_2:
            lcd.print(loadTwoHoursString);
            lcd.print(":");
            lcd.print(loadTwoMinutesString);
            lcd.print(":");
            lcd.print(loadTwoSecondsString);
            break;

          case IR_BUTTON_3:
            lcd.print(loadThreeHoursString);
            lcd.print(":");
            lcd.print(loadThreeMinutesString);
            lcd.print(":");
            lcd.print(loadThreeSecondsString);
            break;

          case IR_BUTTON_4:
            lcd.print(loadFourHoursString);
            lcd.print(":");
            lcd.print(loadFourMinutesString);
            lcd.print(":");
            lcd.print(loadFourSecondsString);
            break;
        }  // switch (selectedLoad)

        StartReceiver();

        buttonPressed = -1;

        continue;  // Skip the rest of the loop and move to the next iteration
      }            // if ((column == 7 || column == 10) && (buttonPressed == 6 || buttonPressed == 7 || buttonPressed == 8 || buttonPressed == 9 || buttonPressed == 10 || buttonPressed == 13 || buttonPressed == 14 || buttonPressed == 15 || buttonPressed == 16))

      // If the user tries to press a non-numeric button other than left or right, don't allow it
      else if ((column != 7 && column != 10) && (buttonPressed == 10 || buttonPressed == 13 || buttonPressed == 14 || buttonPressed == 15 || buttonPressed == 16)) {
        StopReceiver();

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Press a numeric");
        lcd.setCursor(0, 1);
        lcd.print("button 0-9");

        CustomDelay(errorDisplayTime);

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Load ");
        lcd.print(actualSelectedLoad);
        lcd.print(" Timer");
        lcd.setCursor(4, 1);

        switch (selectedLoad) {
          case IR_BUTTON_1:
            lcd.print(loadOneHoursString);
            lcd.print(":");
            lcd.print(loadOneMinutesString);
            lcd.print(":");
            lcd.print(loadOneSecondsString);
            break;

          case IR_BUTTON_2:
            lcd.print(loadTwoHoursString);
            lcd.print(":");
            lcd.print(loadTwoMinutesString);
            lcd.print(":");
            lcd.print(loadTwoSecondsString);
            break;

          case IR_BUTTON_3:
            lcd.print(loadThreeHoursString);
            lcd.print(":");
            lcd.print(loadThreeMinutesString);
            lcd.print(":");
            lcd.print(loadThreeSecondsString);
            break;

          case IR_BUTTON_4:
            lcd.print(loadFourHoursString);
            lcd.print(":");
            lcd.print(loadFourMinutesString);
            lcd.print(":");
            lcd.print(loadFourSecondsString);
            break;
        }  // switch (selectedLoad)

        StartReceiver();

        buttonPressed = -1;

        continue;  // Skip the rest of the loop and move to the next iteration
      }            // else if (buttonPressed == 10 || buttonPressed == 13 || buttonPressed == 14 || buttonPressed == 15 || buttonPressed == 16)



      // Going to the left >
      else if (buttonPressed == 11) {

        // Hours section
        if (column == 4 || column == 5) {
          buttonPressed = -1;

          lcd.setCursor(column, 1);

          // Left end of the timer (don't go before this)
          if (column == 4) {
            continue;
          }

          else if (column == 5) {
            //buttonPressed = -1;
            switch (actualSelectedLoad) {
              case 1:
                lcd.print(loadOneHoursString[1]);
                break;

              case 2:
                lcd.print(loadTwoHoursString[1]);
                break;

              case 3:
                lcd.print(loadThreeHoursString[1]);
                break;

              case 4:
                lcd.print(loadFourHoursString[1]);
                break;
            }

            column--;
          }

          continue;
        }

        // Minutes section
        else if (column == 7 || column == 8) {
          buttonPressed = -1;

          lcd.setCursor(column, 1);

          if (column == 7) {
            switch (actualSelectedLoad) {
              case 1:
                lcd.print(loadOneMinutesString[0]);
                break;

              case 2:
                lcd.print(loadTwoMinutesString[0]);
                break;

              case 3:
                lcd.print(loadThreeMinutesString[0]);
                break;

              case 4:
                lcd.print(loadFourMinutesString[0]);
                break;
            }

            column -= 2;
          }

          else if (column == 8) {
            switch (actualSelectedLoad) {
              case 1:
                lcd.print(loadOneMinutesString[1]);
                break;

              case 2:
                lcd.print(loadTwoMinutesString[1]);
                break;

              case 3:
                lcd.print(loadThreeMinutesString[1]);
                break;

              case 4:
                lcd.print(loadFourMinutesString[1]);
                break;
            }

            column--;
          }

          continue;
        }

        // Seconds section
        else if (column == 10 || column == 11) {
          buttonPressed = -1;

          lcd.setCursor(column, 1);

          if (column == 10) {
            switch (actualSelectedLoad) {
              case 1:
                lcd.print(loadOneSecondsString[0]);
                break;

              case 2:
                lcd.print(loadTwoSecondsString[0]);
                break;

              case 3:
                lcd.print(loadThreeSecondsString[0]);
                break;

              case 4:
                lcd.print(loadFourSecondsString[0]);
                break;
            }

            column -= 2;
          }

          else if (column == 11) {
            switch (actualSelectedLoad) {
              case 1:
                lcd.print(loadOneSecondsString[1]);
                break;

              case 2:
                lcd.print(loadTwoSecondsString[1]);
                break;

              case 3:
                lcd.print(loadThreeSecondsString[1]);
                break;

              case 4:
                lcd.print(loadFourSecondsString[1]);
                break;
            }

            column--;
          }

          continue;
        }
      }
      // < Going to the left



      // Going to the right >
      else if (buttonPressed == 12) {

        // Hours section
        if (column == 4 || column == 5) {
          buttonPressed = -1;

          lcd.setCursor(column, 1);

          // Left end of the timer (don't go before this)
          if (column == 4) {
            switch (actualSelectedLoad) {
              case 1:
                lcd.print(loadOneHoursString[0]);
                break;

              case 2:
                lcd.print(loadTwoHoursString[0]);
                break;

              case 3:
                lcd.print(loadThreeHoursString[0]);
                break;

              case 4:
                lcd.print(loadFourHoursString[0]);
                break;
            }

            column++;
          }

          else if (column == 5) {
            switch (actualSelectedLoad) {
              case 1:
                lcd.print(loadOneHoursString[1]);
                break;

              case 2:
                lcd.print(loadTwoHoursString[1]);
                break;

              case 3:
                lcd.print(loadThreeHoursString[1]);
                break;

              case 4:
                lcd.print(loadFourHoursString[1]);
                break;
            }

            column += 2;
          }

          continue;
        }

        // Minutes section
        else if (column == 7 || column == 8) {
          buttonPressed = -1;

          lcd.setCursor(column, 1);

          if (column == 7) {
            switch (actualSelectedLoad) {
              case 1:
                lcd.print(loadOneMinutesString[0]);
                break;

              case 2:
                lcd.print(loadTwoMinutesString[0]);
                break;

              case 3:
                lcd.print(loadThreeMinutesString[0]);
                break;

              case 4:
                lcd.print(loadFourMinutesString[0]);
                break;
            }

            column++;
          }

          else if (column == 8) {
            switch (actualSelectedLoad) {
              case 1:
                lcd.print(loadOneMinutesString[1]);
                break;

              case 2:
                lcd.print(loadTwoMinutesString[1]);
                break;

              case 3:
                lcd.print(loadThreeMinutesString[1]);
                break;

              case 4:
                lcd.print(loadFourMinutesString[1]);
                break;
            }

            column += 2;
          }

          continue;
        }

        // Seconds section
        else if (column == 10 || column == 11) {
          buttonPressed = -1;

          lcd.setCursor(column, 1);

          if (column == 10) {
            switch (actualSelectedLoad) {
              case 1:
                lcd.print(loadOneSecondsString[0]);
                break;

              case 2:
                lcd.print(loadTwoSecondsString[0]);
                break;

              case 3:
                lcd.print(loadThreeSecondsString[0]);
                break;

              case 4:
                lcd.print(loadFourSecondsString[0]);
                break;
            }

            column++;
          }

          // Right end of the timer (don't go after this)
          else if (column == 11) {
            continue;
          }

          continue;
        }
      }  // else if (buttonPressed == 12)
      // < Going to the right



      lcd.setCursor(column, 1);
      lcd.print(buttonPressed);



      // Recording valid button presses >
      switch (selectedLoad) {

        // Load 1
        case IR_BUTTON_1:
          switch (column) {
            case 4:
              loadOneHoursString.setCharAt(0, ('0' + buttonPressed));
              column++;
              break;

            case 5:
              loadOneHoursString.setCharAt(1, ('0' + buttonPressed));
              column += 2;
              break;

            case 7:
              loadOneMinutesString.setCharAt(0, ('0' + buttonPressed));
              column++;
              break;

            case 8:
              loadOneMinutesString.setCharAt(1, ('0' + buttonPressed));
              column += 2;
              break;

            case 10:
              loadOneSecondsString.setCharAt(0, ('0' + buttonPressed));
              column++;
              break;

            case 11:
              loadOneSecondsString.setCharAt(1, ('0' + buttonPressed));
              column++;
              break;
          }  // switch (column)

          break;  // Break out of case IR_BUTTON_1

        // Load 2
        case IR_BUTTON_2:
          switch (column) {
            case 4:
              loadTwoHoursString.setCharAt(0, ('0' + buttonPressed));
              column++;
              break;

            case 5:
              loadTwoHoursString.setCharAt(1, ('0' + buttonPressed));
              column += 2;
              break;

            case 7:
              loadTwoMinutesString.setCharAt(0, ('0' + buttonPressed));
              column++;
              break;

            case 8:
              loadTwoMinutesString.setCharAt(1, ('0' + buttonPressed));
              column += 2;
              break;

            case 10:
              loadTwoSecondsString.setCharAt(0, ('0' + buttonPressed));
              column++;
              break;

            case 11:
              loadTwoSecondsString.setCharAt(1, ('0' + buttonPressed));
              column++;
              break;
          }  // switch (column)

          break;  // Break out of case IR_BUTTON_2

        // Load 3
        case IR_BUTTON_3:
          switch (column) {
            case 4:
              loadThreeHoursString.setCharAt(0, ('0' + buttonPressed));
              column++;
              break;

            case 5:
              loadThreeHoursString.setCharAt(1, ('0' + buttonPressed));
              column += 2;
              break;

            case 7:
              loadThreeMinutesString.setCharAt(0, ('0' + buttonPressed));
              column++;
              break;

            case 8:
              loadThreeMinutesString.setCharAt(1, ('0' + buttonPressed));
              column += 2;
              break;

            case 10:
              loadThreeSecondsString.setCharAt(0, ('0' + buttonPressed));
              column++;
              break;

            case 11:
              loadThreeSecondsString.setCharAt(1, ('0' + buttonPressed));
              column++;
              break;
          }  // switch (column)

          break;  // Break out of case IR_BUTTON_3

        // Load 4
        case IR_BUTTON_4:
          switch (column) {
            case 4:
              loadFourHoursString.setCharAt(0, ('0' + buttonPressed));
              column++;
              break;

            case 5:
              loadFourHoursString.setCharAt(1, ('0' + buttonPressed));
              column += 2;
              break;

            case 7:
              loadFourMinutesString.setCharAt(0, ('0' + buttonPressed));
              column++;
              break;

            case 8:
              loadFourMinutesString.setCharAt(1, ('0' + buttonPressed));
              column += 2;
              break;

            case 10:
              loadFourSecondsString.setCharAt(0, ('0' + buttonPressed));
              column++;
              break;

            case 11:
              loadFourSecondsString.setCharAt(1, ('0' + buttonPressed));
              column++;
              break;
          }  // switch (column)

          break;  // Break out of case IR_BUTTON_4
      }           // switch (selectedLoad)
      // < Recording valid button presses



      if (column == 12) {
        StopReceiver();

        CustomDelay(delayTime);

        break;  // Break out of while loop
      }

      StopReceiver();

      CustomDelay(450);

      StartReceiver();

      buttonPressed = -1;
    }  // else
    // < If the user has pressed a button, check if it's valid



  }  // while (true)



  // Setting the actual timers >
  switch (selectedLoad) {

    // Load 1
    case IR_BUTTON_1:
      loadOneHours = loadOneHoursString.toInt();
      loadOneMinutes = loadOneMinutesString.toInt();
      loadOneSeconds = loadOneSecondsString.toInt();

      break;  // Break out of case IR_BUTTON_1

    // Load 2
    case IR_BUTTON_2:
      loadTwoHours = loadTwoHoursString.toInt();
      loadTwoMinutes = loadTwoMinutesString.toInt();
      loadTwoSeconds = loadTwoSecondsString.toInt();

      break;  // Break out of case IR_BUTTON_2

    // Load 3
    case IR_BUTTON_3:
      loadThreeHours = loadThreeHoursString.toInt();
      loadThreeMinutes = loadThreeMinutesString.toInt();
      loadThreeSeconds = loadThreeSecondsString.toInt();

      break;  // Break out of case IR_BUTTON_3

    // Load 4
    case IR_BUTTON_4:
      loadFourHours = loadFourHoursString.toInt();
      loadFourMinutes = loadFourMinutesString.toInt();
      loadFourSeconds = loadFourSecondsString.toInt();

      break;  // Break out of case IR_BUTTON_4
  }           // switch (selectedLoad)
  // < Setting the actual timers



  // Mark the load's timer as active >
  switch (actualSelectedLoad) {
    case 1:
      loadOneTimerActive = true;
      break;

    case 2:
      loadTwoTimerActive = true;
      break;

    case 3:
      loadThreeTimerActive = true;
      break;

    case 4:
      loadFourTimerActive = true;
      break;
  }
  // < Mark the load's timer as active



  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Timer set");
  lcd.setCursor(0, 1);
  lcd.print("successfully!");

  CustomDelay(delayTime);

  StartReceiver();

endOfSetTimer:

  setFunctionActive = false;

  RetrieveLoadInfo(selectedLoad);
}  // SetTimer()





void SetLoadPriorities() {

  // Load 1 >
  if (sensorValue1 >= LOW_PRIORITY_CURRENT) {
    if (loadOnePriority = 'L') {
      previousLoadOnePriority = 'L';
    }

    loadOnePriority = 'H';
  } else {
    if (loadOnePriority = 'H') {
      previousLoadOnePriority = 'H';
    }

    loadOnePriority = 'L';
  }
  // < Load 1



  // Load 2 >
  if (sensorValue2 >= LOW_PRIORITY_CURRENT) {
    if (loadTwoPriority = 'L') {
      previousLoadTwoPriority = 'L';
    }

    loadTwoPriority = 'H';
  } else {
    if (loadTwoPriority = 'H') {
      previousLoadTwoPriority = 'H';
    }

    loadTwoPriority = 'L';
  }
  // < Load 2



  // Load 3 >
  if (sensorValue3 >= LOW_PRIORITY_CURRENT) {
    if (loadThreePriority = 'L') {
      previousLoadThreePriority = 'L';
    }

    loadThreePriority = 'H';
  } else {
    if (loadThreePriority = 'H') {
      previousLoadThreePriority = 'H';
    }

    loadThreePriority = 'L';
  }
  // < Load 3



  // Load 4 >
  if (sensorValue4 >= LOW_PRIORITY_CURRENT) {
    if (loadFourPriority = 'L') {
      previousLoadFourPriority = 'L';
    }

    loadFourPriority = 'H';
  } else {
    if (loadFourPriority = 'H') {
      previousLoadFourPriority = 'H';
    }

    loadFourPriority = 'L';
  }
  // < Load 4

}  // SetLoadPriorities()





void UpdateDisplayedLoadPriority() {
  switch (selectedLoad) {
    case IR_BUTTON_1:
      if (!loadOneOff && loadOnePriority != previousLoadOnePriority) {

        // Erase the previous priority to update it
        lcd.setCursor(15, 0);
        lcd.print(" ");

        lcd.setCursor(15, 0);
        lcd.print(loadOnePriority);
      }

      break;

    case IR_BUTTON_2:
      if (!loadTwoOff && loadTwoPriority != previousLoadTwoPriority) {

        // Erase the previous priority to update it
        lcd.setCursor(15, 0);
        lcd.print(" ");

        lcd.setCursor(15, 0);
        lcd.print(loadTwoPriority);
      }

      break;

    case IR_BUTTON_3:
      if (!loadThreeOff && loadThreePriority != previousLoadThreePriority) {

        // Erase the previous priority to update it
        lcd.setCursor(15, 0);
        lcd.print(" ");

        lcd.setCursor(15, 0);
        lcd.print(loadThreePriority);
      }

      break;

    case IR_BUTTON_4:
      if (!loadFourOff && loadFourPriority != previousLoadFourPriority) {

        // Erase the previous priority to update it
        lcd.setCursor(15, 0);
        lcd.print(" ");

        lcd.setCursor(15, 0);
        lcd.print(loadFourPriority);
      }

      break;
  }
}  // UpdateDisplayedLoadPriority()





int CustomDelay(int interval) {
  unsigned long currentMillis = millis();

  int command = -1;

  int i;

  while (millis() < currentMillis + interval) {

    // Running load timers if active
    CheckTimerStatuses();

    // Read currents
    ReadCurrent();

    // Automatically set load priorities
    SetLoadPriorities();

    // Turn the LCD OFF if no command is pressed within a certain amount of time
    CheckLCDTimeout();

    // Retrieve a command from the remote
    command = ReceiveCommand();

    for (i = 0; i < 17; i++) {
      if (command == remoteButtonList[i]) {
        command = remoteButtonList[i];
        return command;
      }
    }
  }

  return command;
}  // CustomDelay()





int ScrollText(String str, int rowToPrint) {
  selectedLoad = -1;

  int command = -1;

  int i, j = 0;

  int delayTime = 100;

  bool shouldWeBreak = 0;

  for (i = 0; i < str.length(); i++) {

    // Running load timers if active
    CheckTimerStatuses();

    // Automatically set load priorities
    SetLoadPriorities();

    // Read currents
    ReadCurrent();

    // Turn the LCD OFF if no command is pressed within a certain amount of time
    CheckLCDTimeout();

    // Checking for a button press from the remote
    command = ReceiveCommand();

    if (command != -1) {
      return command;
    }

    // Gradually printing the text from left to right
    if (i < 15) {
      lcd.setCursor(i, rowToPrint);
      lcd.print(str.charAt(i));

      command = CustomDelay(delayTime);

      if (command != -1) {
        return command;
      }
    }

    // Scrolling the text to the left
    else {
      for (i = 16; i < str.length(); i++) {
        j++;
        lcd.setCursor(0, rowToPrint);
        lcd.print(str.substring(j, j + 16));

        command = CustomDelay(delayTime);

        if (command != -1) {
          return command;
        }
      }
    }
  }  // for loop

  return command;
}  // ScrollText()





void PrintCurrentGraph() {

  // Load 1 >
  if (selectedLoad == IR_BUTTON_1 && !loadOneOff) {
printLoadOneGraph:
    if (columnToPrint < 16) {
      lcd.setCursor(columnToPrint, 1);

      if (sensorValue1 > 0.0 && sensorValue1 < (MAX_CURRENT / 8)) {
        lcd.write(1);
      }

      else if (sensorValue1 >= (MAX_CURRENT / 8) && sensorValue1 < 2 * (MAX_CURRENT / 8)) {
        lcd.write(2);
      }

      else if (sensorValue1 >= 2 * (MAX_CURRENT / 8) && sensorValue1 < 3 * (MAX_CURRENT / 8)) {
        lcd.write(3);
      }

      else if (sensorValue1 >= 3 * (MAX_CURRENT / 8) && sensorValue1 < 4 * (MAX_CURRENT / 8)) {
        lcd.write(4);
      }

      else if (sensorValue1 >= 4 * (MAX_CURRENT / 8) && sensorValue1 < 5 * (MAX_CURRENT / 8)) {
        lcd.write(5);
      }

      else if (sensorValue1 >= 5 * (MAX_CURRENT / 8) && sensorValue1 < 6 * (MAX_CURRENT / 8)) {
        lcd.write(6);
      }

      else if (sensorValue1 >= 6 * (MAX_CURRENT / 8) && sensorValue1 < 7 * (MAX_CURRENT / 8)) {
        lcd.write(7);
      }

      else {
        lcd.write(8);
      }

      columnToPrint++;
    }  // if (columnToPrint < 16);

    else {
      lcd.setCursor(9, 1);
      lcd.print("       ");

      columnToPrint = 9;  // Reset columnToPrint

      goto printLoadOneGraph;
    }  // else
  }    // if (selectedLoad == IR_BUTTON_1)
  // < Load 1



  // Load 2 >
  if (selectedLoad == IR_BUTTON_2) {
printLoadTwoGraph:
    if (columnToPrint < 16) {
      lcd.setCursor(columnToPrint, 1);

      if (sensorValue2 > 0.0 && sensorValue2 < (MAX_CURRENT / 8)) {
        lcd.write(1);
      }

      else if (sensorValue2 >= (MAX_CURRENT / 8) && sensorValue2 < 2 * (MAX_CURRENT / 8)) {
        lcd.write(2);
      }

      else if (sensorValue2 >= 2 * (MAX_CURRENT / 8) && sensorValue2 < 3 * (MAX_CURRENT / 8)) {
        lcd.write(3);
      }

      else if (sensorValue2 >= 3 * (MAX_CURRENT / 8) && sensorValue2 < 4 * (MAX_CURRENT / 8)) {
        lcd.write(4);
      }

      else if (sensorValue2 >= 4 * (MAX_CURRENT / 8) && sensorValue2 < 5 * (MAX_CURRENT / 8)) {
        lcd.write(5);
      }

      else if (sensorValue2 >= 5 * (MAX_CURRENT / 8) && sensorValue2 < 6 * (MAX_CURRENT / 8)) {
        lcd.write(6);
      }

      else if (sensorValue2 >= 6 * (MAX_CURRENT / 8) && sensorValue2 < 7 * (MAX_CURRENT / 8)) {
        lcd.write(7);
      }

      else {
        lcd.write(8);
      }

      columnToPrint++;
    }  // if (columnToPrint < 16);

    else {
      lcd.setCursor(9, 1);
      lcd.print("       ");

      columnToPrint = 9;  // Reset columnToPrint

      goto printLoadTwoGraph;
    }  // else
  }    // if (selectedLoad == IR_BUTTON_2)
  // < Load 2



  // Load 3 >
  if (selectedLoad == IR_BUTTON_3) {
printLoadThreeGraph:
    if (columnToPrint < 16) {
      lcd.setCursor(columnToPrint, 1);

      if (sensorValue3 > 0.0 && sensorValue3 < (MAX_CURRENT / 8)) {
        lcd.write(1);
      }

      else if (sensorValue3 >= (MAX_CURRENT / 8) && sensorValue3 < 2 * (MAX_CURRENT / 8)) {
        lcd.write(2);
      }

      else if (sensorValue3 >= 2 * (MAX_CURRENT / 8) && sensorValue3 < 3 * (MAX_CURRENT / 8)) {
        lcd.write(3);
      }

      else if (sensorValue3 >= 3 * (MAX_CURRENT / 8) && sensorValue3 < 4 * (MAX_CURRENT / 8)) {
        lcd.write(4);
      }

      else if (sensorValue3 >= 4 * (MAX_CURRENT / 8) && sensorValue3 < 5 * (MAX_CURRENT / 8)) {
        lcd.write(5);
      }

      else if (sensorValue3 >= 5 * (MAX_CURRENT / 8) && sensorValue3 < 6 * (MAX_CURRENT / 8)) {
        lcd.write(6);
      }

      else if (sensorValue3 >= 6 * (MAX_CURRENT / 8) && sensorValue3 < 7 * (MAX_CURRENT / 8)) {
        lcd.write(7);
      }

      else {
        lcd.write(8);
      }

      columnToPrint++;
    }  // if (columnToPrint < 16);

    else {
      lcd.setCursor(9, 1);
      lcd.print("       ");

      columnToPrint = 9;  // Reset columnToPrint

      goto printLoadThreeGraph;
    }  // else
  }    // if (selectedLoad == IR_BUTTON_3)
  // < Load 3



  // Load 4 >
  if (selectedLoad == IR_BUTTON_4) {
printLoadFourGraph:
    if (columnToPrint < 16) {
      lcd.setCursor(columnToPrint, 1);

      if (sensorValue4 > 0.0 && sensorValue4 < (MAX_CURRENT / 8)) {
        lcd.write(1);
      }

      else if (sensorValue4 >= (MAX_CURRENT / 8) && sensorValue4 < 2 * (MAX_CURRENT / 8)) {
        lcd.write(2);
      }

      else if (sensorValue4 >= 2 * (MAX_CURRENT / 8) && sensorValue4 < 3 * (MAX_CURRENT / 8)) {
        lcd.write(3);
      }

      else if (sensorValue4 >= 3 * (MAX_CURRENT / 8) && sensorValue4 < 4 * (MAX_CURRENT / 8)) {
        lcd.write(4);
      }

      else if (sensorValue4 >= 4 * (MAX_CURRENT / 8) && sensorValue4 < 5 * (MAX_CURRENT / 8)) {
        lcd.write(5);
      }

      else if (sensorValue4 >= 5 * (MAX_CURRENT / 8) && sensorValue4 < 6 * (MAX_CURRENT / 8)) {
        lcd.write(6);
      }

      else if (sensorValue4 >= 6 * (MAX_CURRENT / 8) && sensorValue4 < 7 * (MAX_CURRENT / 8)) {
        lcd.write(7);
      }

      else {
        lcd.write(8);
      }

      columnToPrint++;
    }  // if (columnToPrint < 16);

    else {
      lcd.setCursor(9, 1);
      lcd.print("       ");

      columnToPrint = 9;  // Reset columnToPrint

      goto printLoadFourGraph;
    }  // else
  }    // if (selectedLoad == IR_BUTTON_4)
  // < Load 4

}  // PrintCurrentGraph()





int WelcomeScreen() {
  selectedLoad = -1;

  int interval = 2000;

  int command = -1;

  bool firstRun = 1;



  // Welcome Screen strings >
  String welcomeScreenInfo = "0: Welcome Screen";
  String loadScreensInfo = "1-4: Loads 1-4";
  String turningLoadsOnOffInfo = "OK or ENTER: Turn load ON/OFF";
  String overallInfo = "5: Overall system info";
  String scrollingInfo = "< or >: Scroll left or right between loads";
  String pInfo = "P = Load's Priority";
  String tInfo = "T = Load Timer Active";
  String setTimerInfo = "7: Set Load Timer";
  String checkTimerInfo = "9: Check Load Timer Status";
  // < Welcome Screen strings



  // If this is the first run of WelcomeScreen() >
  if (firstRun == true) {
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("Welcome!");
  }
  // < If this is the first run of WelcomeScreen()



  // Running load timers if active
  CheckTimerStatuses();

  // Automatically setting load priorities
  SetLoadPriorities();

  // Read currents
  ReadCurrent();

  // Turn the LCD OFF if no command is pressed within a certain amount of time
  CheckLCDTimeout();



  // Instructions for returning to the welcome screen >
  command = ScrollText(welcomeScreenInfo, 1);

  if (command != -1) {
    return command;
  }

  command = CustomDelay(interval);

  if (command != -1) {
    return command;
  }

  lcd.setCursor(0, 1);
  lcd.print("                ");
  // < Instructions for returning to the welcome screen



  // Instructions for getting to specific load screens >
  command = ScrollText(loadScreensInfo, 1);

  if (command != -1) {
    return command;
  }

  command = CustomDelay(interval);

  if (command != -1) {
    return command;
  }

  lcd.setCursor(0, 1);
  lcd.print("                ");
  // < Instructions for getting to specific load screens



  // Instructions for scrolling between load screens >
  command = ScrollText(scrollingInfo, 1);

  if (command != -1) {
    return command;
  }

  command = CustomDelay(interval);

  if (command != -1) {
    return command;
  }

  lcd.setCursor(0, 1);
  lcd.print("                ");
  // < Instructions for scrolling between load screens



  // Explanation of "P" >
  command = ScrollText(pInfo, 1);

  if (command != -1) {
    return command;
  }

  command = CustomDelay(interval);

  if (command != -1) {
    return command;
  }

  lcd.setCursor(0, 1);
  lcd.print("                ");
  // < Explanation of "P"



  // Explanation of "T" >
  command = ScrollText(tInfo, 1);

  if (command != -1) {
    return command;
  }

  command = CustomDelay(interval);

  if (command != -1) {
    return command;
  }

  lcd.setCursor(0, 1);
  lcd.print("                ");
  // < Explanation of "T"



  // Instructions for turning loads ON/OFF >
  command = ScrollText(turningLoadsOnOffInfo, 1);

  if (command != -1) {
    return command;
  }

  command = CustomDelay(interval);

  if (command != -1) {
    return command;
  }

  lcd.setCursor(0, 1);
  lcd.print("                ");
  // < Instructions for turning loads ON/OFF



  // Intructions for getting to overall system information screen >
  command = ScrollText(overallInfo, 1);

  if (command != -1) {
    return command;
  }

  command = CustomDelay(interval);

  if (command != -1) {
    return command;
  }

  lcd.setCursor(0, 1);
  lcd.print("                ");
  // < Intructions for getting to overall system information screen



  // Instructions for setting load timers >
  command = ScrollText(setTimerInfo, 1);

  if (command != -1) {
    return command;
  }

  command = CustomDelay(interval);

  if (command != -1) {
    return command;
  }

  lcd.setCursor(0, 1);
  lcd.print("                ");
  // < Instructions for setting load timers



  // Instructions for checking load timer status >
  command = ScrollText(checkTimerInfo, 1);

  if (command != -1) {
    return command;
  }

  command = CustomDelay(interval);

  if (command != -1) {
    return command;
  }

  lcd.setCursor(0, 1);
  lcd.print("                ");
  // < Instructions for checking load timer status



  return IR_BUTTON_0;
}  // WelcomeScreen()





int Scroll(int currentLoad, int direction) {
  int newLoad = -1;

  const int commandList[7] = { -1, IR_BUTTON_1, IR_BUTTON_2, IR_BUTTON_3, IR_BUTTON_4, IR_BUTTON_5, -2 };  // -1 and -2 are placeholders to indicate the ends of the array



  for (int i = 0; i < 7; i++) {
    if (commandList[i] == currentLoad) {
      if (direction == 1 && commandList[i - 1] != -1) {  // Scrolling left (to the previous load)
        newLoad = commandList[i - 1];
      }

      else if (direction == 2 && commandList[i + 1] != -2) {  // Scrolling right (to the next load)
        newLoad = commandList[i + 1];
      }

      RetrieveLoadInfo(newLoad);
    }
  }

  //loadsOut = 0;

  return newLoad;
}  // Scroll()





void RetrieveLoadInfo(int load) {
startOfRetrieveLoadInfo:
  int command = -1;

  loadsOut = 0;

  CheckLoadStatuses();

  switch (load) {

    // Load 1 is selected
    case IR_BUTTON_1:
      selectedLoad = IR_BUTTON_1;
      lcd.clear();
      lcd.setCursor(0, 0);

      if (loadOneTimerActive) {
        lcd.print("Load 1: T");
      }

      else {
        lcd.print("Load 1:");
      }

      // If Load 1 is ON
      if (!loadOneOff) {
        columnToPrint = 9;  // For PrintCurrentGraph()

        lcd.setCursor(11, 0);
        lcd.print("P = ");
        lcd.print(loadOnePriority);

        lcd.setCursor(0, 1);
        lcd.print("                ");

        digitalWrite(RELAY_ONE_PIN, LOW);  // Turn Load 1 ON

        lcd.setCursor(0, 1);
        lcd.print(sensorValue1);
        lcd.print("A");

        previousLoad1CurrentReading = sensorValue1;  // Used to determine if the current reading being displayed needs updating
      }                                              // if (!loadOneOff)

      // If Load 1 is OFF
      else {
        digitalWrite(RELAY_ONE_PIN, HIGH);  // Turn Load 1 OFF
        sensorValue1 = 0;

        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 1);
        lcd.print("Load 1 is OFF");
      }

      break;  // Break out of case IR_BUTTON_1



    // Load 2 is selected
    case IR_BUTTON_2:
      selectedLoad = IR_BUTTON_2;
      lcd.clear();
      lcd.setCursor(0, 0);

      if (loadTwoTimerActive) {
        lcd.print("Load 2: T");
      }

      else {
        lcd.print("Load 2:");
      }

      // If Load 2 is ON
      if (!loadTwoOff) {
        columnToPrint = 9;  // For PrintCurrentGraph()

        lcd.setCursor(11, 0);
        lcd.print("P = ");
        lcd.print(loadTwoPriority);

        lcd.setCursor(0, 1);
        lcd.print("                ");

        digitalWrite(RELAY_TWO_PIN, LOW);  // Turn Load 2 ON

        lcd.setCursor(0, 1);
        lcd.print(sensorValue2);
        lcd.print("A");

        previousLoad2CurrentReading = sensorValue2;  // Used to determine if the current reading being displayed needs updating
      }

      // If Load 2 is OFF
      else {
        digitalWrite(RELAY_TWO_PIN, HIGH);  // Turn Load 2 OFF
        sensorValue2 = 0;

        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 1);
        lcd.print("Load 2 is OFF");
      }

      break;  // Break out of case IR_BUTTON_2



    // Load 3 is selected
    case IR_BUTTON_3:
      selectedLoad = IR_BUTTON_3;
      lcd.clear();
      lcd.setCursor(0, 0);

      if (loadThreeTimerActive) {
        lcd.print("Load 3: T");
      }

      else {
        lcd.print("Load 3:");
      }

      // If Load 3 is ON
      if (!loadThreeOff) {
        columnToPrint = 9;  // For PrintCurrentGraph()

        lcd.setCursor(11, 0);
        lcd.print("P = ");
        lcd.print(loadThreePriority);

        lcd.setCursor(0, 1);
        lcd.print("                ");

        digitalWrite(RELAY_THREE_PIN, LOW);  // Turn Load 3 ON

        lcd.setCursor(0, 1);
        lcd.print(sensorValue3);
        lcd.print("A");

        previousLoad3CurrentReading = sensorValue3;  // Used to determine if the current reading being displayed needs updating
      }

      // If Load 3 is OFF
      else {
        digitalWrite(RELAY_THREE_PIN, HIGH);  // Turn Load 3 OFF
        sensorValue3 = 0;

        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 1);
        lcd.print("Load 3 is OFF");
      }

      break;  // Break out of case IR_BUTTON_3



    // Load 4 is selected
    case IR_BUTTON_4:
      selectedLoad = IR_BUTTON_4;
      lcd.clear();
      lcd.setCursor(0, 0);

      if (loadFourTimerActive) {
        lcd.print("Load 4: T");
      }

      else {
        lcd.print("Load 4:");
      }

      // If Load 4 is ON
      if (!loadFourOff) {
        columnToPrint = 9;  // For PrintCurrentGraph()

        lcd.setCursor(11, 0);
        lcd.print("P = ");
        lcd.print(loadFourPriority);

        lcd.setCursor(0, 1);
        lcd.print("                ");

        digitalWrite(RELAY_FOUR_PIN, LOW);  // Turn Load 4 ON

        lcd.setCursor(0, 1);
        lcd.print(sensorValue4);
        lcd.print("A");

        previousLoad4CurrentReading = sensorValue4;  // Used to determine if the current reading being displayed needs updating
      }

      // If Load 4 is OFF
      else {
        digitalWrite(RELAY_FOUR_PIN, HIGH);  // Turn Load 4 OFF
        sensorValue4 = 0;

        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 1);
        lcd.print("Load 4 is OFF");
      }

      break;  // Break out of case IR_BUTTON_4



    // Overall load information
    case IR_BUTTON_5:
      selectedLoad = IR_BUTTON_5;

      lcd.clear();
      lcd.setCursor(5, 0);
      lcd.print("Total:");
      lcd.setCursor(0, 1);
      lcd.print(total);
      lcd.print("A");

      lcd.setCursor(10, 1);
      lcd.print("LO = ");
      lcd.print(loadsOut);

      break;
  }  // switch (load)
}  //RetrieveLoadInfo()