//////////////////////////////////////////////////////////////////////// 
// NAME:     Holly Hammons and Elijah Ball   

// GROUP:    Smart Climbing Holds 

// TITLE:    RFM69 Send Code 

// FUNCTION: This code writes an address to a RFM69 sender. You can type
//           in an address number in the serial monitor to change the
//           address. When you push the button, it sends a signal to the
//           desired reciever to light up the LED and buzz the piezzo for
//           1 sec.

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
//           Button Pin     D8
//           (+) Button     5V
//           (-)But to 1k   GND   

// SOFTWARE: Install Radiohead library: https://adafru.it/vgF

// EXECUTE:  Define RFM69 send device to have a different 
//           address than any other reciever. Open the serial
//           monitor to change the recieve address.

// RESOURCE: Based on example code from Radiohead Library.

// PURPOSE:  Setting up basic transmit/recieve code before applying
//           to specific climbing hold application.
//////////////////////////////////////////////////////////////////////

// include libraries
#include <SPI.h>
#include <RH_RF69.h>
#include <RHReliableDatagram.h>

// address sets
int destAddr = 3;
#define MY_ADDRESS     2

// Our frequency is 915 Hz
#define RF69_FREQ 915.0

// define RFM69 pins
#define RFM69_INT     3
#define RFM69_CS      4
#define RFM69_RST     2
#define LED           13

int buttonPin = 8;
int buttonState = 0;

// instanciate the radio driver
RH_RF69 rf69(RFM69_CS, RFM69_INT);

// packet counter, we increment per xmission
int16_t packetnum = 0;

// array to store RGB values to send (3 chars for each color, +7 for radiohead library)
char radiopacket[16] = "255000050";

// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram rf69_manager(rf69, MY_ADDRESS);

// these variables are for getting user input from the serial monitor
int i = 0;
char endMarker = '\r';
int message;

void setup()
{
  Serial.begin(115200);

  // RFM69 pins
  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, LOW);

  // button pin
  pinMode(buttonPin, INPUT);

  Serial.println("RGB LED RFM69 Send Code With Button");
  Serial.println();

  // manual reset
  digitalWrite(RFM69_RST, HIGH);
  delay(10);
  digitalWrite(RFM69_RST, LOW);
  delay(10);

  // check that RFM69(address) initializes properlly
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
  Serial.println("Press the button to send a signal.");
  Serial.println("Type in a number and press enter to change reciever address.");
  Serial.println("");
}

void loop() {
    // look for input from the serial monitor to change the address
    while (Serial.available() > 0) {
    message = Serial.read();
    // read the message until carraige return
    if (message != endMarker) {
      destAddr = message - '0';
    }
    else {
      Serial.print("Destination Address Entered: "); 
      Serial.println(destAddr);
      Serial.println("Type in a number and press enter to change reciever address.");
      Serial.println("");
    }
  }
  // if the button is pressed
  buttonState = digitalRead(buttonPin);

  if (buttonState == HIGH) {
    itoa(packetnum, radiopacket + 9, 10);
    Serial.print("Sending signal to activate light and buzzer to reciever address: "); 
    Serial.println(destAddr);
    Serial.println("");
    // send the code
    rf69_manager.sendtoWait((uint8_t *)radiopacket, strlen(radiopacket), destAddr);
  }
}
