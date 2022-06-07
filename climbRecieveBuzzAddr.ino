//////////////////////////////////////////////////////////////////////// 
// NAME:     Holly Hammons and Elijah Ball   

// GROUP:    Smart Climbing Holds 

// TITLE:    RFM69 Reciever Code 

// FUNCTION: This code writes an address to a RFM69 reciever. When it 
//           recieves a signal from another RFM69, to its address, it
//           will light up an LED and turn on a piezzo buzzer for 1 sec.

// HARDWARE: RFM69    ->    Arduino or Redboard
//           RST            D2
//           CS             D4
//           MOSI           D11
//           MISO           D12
//           SCK            D13
//           G0             D3
//           GND            GND
//           VIN            3.3V

//           CIRCUIT  ->    Arduino or Redboard
//           (+) Piezzo     D5
//           (-) Piezzo     GND
//           2.2k-R_LED     D6
//           2.2k-G_LED     D9
//           2.2k-B_LED     D10
//           GND_LED        GND    

// SOFTWARE: Install Radiohead library: https://adafru.it/vgF

// EXECUTE:  Define each RFM69 device to have a unique MY_ADDRESS.

// RESOURCE: Based on example code from Radiohead Library.

// PURPOSE:  Setting up basic transmit/recieve code before applying
//           to specific climbing hold application.
//////////////////////////////////////////////////////////////////////

// include libraries
#include <SPI.h>
#include <RH_RF69.h>
#include <RHReliableDatagram.h>

// address sets
#define MY_ADDRESS     4

// Our frequency is 915 Hz
#define RF69_FREQ 915.0

// define RFM69 pins
#define RFM69_INT     3
#define RFM69_CS      4
#define RFM69_RST     2
#define LED           13

// instanciate the radio driver
RH_RF69 rf69(RFM69_CS, RFM69_INT);

// name RGB LED pins
int redPin = 6;
int greenPin = 9;
int bluePin = 10;

// for holding RGB vals incoming
char rgbCode[10] = "000000000";

// for parsing RGB values from rgbCode array
char redCode[4] = "000";
char greenCode[4] = "000";
char blueCode[4] = "000";

// for making the char vals into one int
int red = 0;
int green = 0;
int blue = 0;

// for timing buzzer and led
int buzzerPin = 5;
float timeNow = 0;
float period = 1000;

// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram rf69_manager(rf69, MY_ADDRESS);

int16_t packetnum = 0;  // packet counter, we increment per xmission

void setup()
{
  Serial.begin(115200);

  // define RGB pins as outputs
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  // RFM69 pins
  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, LOW);

  Serial.println("RGB LED RFM69 Recieve Test");
  Serial.println();

  // manual reset
  digitalWrite(RFM69_RST, HIGH);
  delay(10);
  digitalWrite(RFM69_RST, LOW);
  delay(10);

  // check that RFM69 initializes properlly
  if (!rf69_manager.init()) {
    Serial.println("RFM69 radio init failed");
    while (1);
  }
  Serial.println("RFM69 radio init OK!");
  // check that frequency is set properly
  if (!rf69.setFrequency(RF69_FREQ)) {
    Serial.println("setFrequency failed");
  }
  Serial.print("RFM69 radio @");  Serial.print((int)RF69_FREQ);  Serial.println(" MHz");

  Serial.print("\n");
  Serial.println("Waiting on Color Code.");
}

// make a buffer to keep reply in
uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
uint8_t data[] = "  OK";

void loop() {
  // needed for the address library
  uint8_t len = sizeof(buf);
  uint8_t from;
  // if a message is recieved from address (from will automatically be destination if not null)
  if (rf69_manager.recvfromAckTimeout(buf, &len, 2000, &from)) {
    // zero out remaining string
    buf[len] = 0;
    // print the rgb value
    Serial.print("RGB LED Code: ");
    for (int i = 0; i < 9; i++) {
      Serial.print((char)buf[i]);
      // store whole color code in the array
      rgbCode[i] = (char)buf[i];
    }
    Serial.print("\n");

    // parse the array for red, green, and blue colors
    for (int i = 0; i < 3; i++) {
      redCode[i] = rgbCode[i];
    }
    for (int i = 0; i < 3; i++) {
      greenCode[i] = rgbCode[i + 3];
    }
    for (int i = 0; i < 3; i++) {
      blueCode[i] = rgbCode[i + 6];
    }

    // turn the char parsed arrays into one integer value for each RGB color code
    red = atoi(redCode);
    green = atoi(greenCode);
    blue = atoi(blueCode);

    float loopTime = 1000;
    float timeNow = 0;
    float currentTime = 0;

    timeNow = millis();

    // turn on LED and buzzer for specific amount of time
    while (millis() < (timeNow + period)) {
      // send color values to the rgb led with PWM
      analogWrite(redPin, red);
      analogWrite(greenPin, green);
      analogWrite(bluePin, blue);
      tone(buzzerPin, 2300);
    }
    // turn off lights and sounds
    analogWrite(redPin, 0);
    analogWrite(greenPin, 0);
    analogWrite(bluePin, 0);
    noTone(buzzerPin);
  }
}
