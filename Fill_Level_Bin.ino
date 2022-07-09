//This example code is in the Public Domain (or CC0 licensed, at your option.)
//By Evandro Copercini - 2018
//
//This example creates a bridge between Serial and Classical Bluetooth (SPP)
//and also demonstrate that SerialBT have the same functionalities of a normal Serial

#include "BluetoothSerial.h"
#include "lcdgfx.h"
#include "lcdgfx_gui.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

DisplaySH1107_64x128_SPI display(22,{-1, 5, 21, 0,-1,-1}); // Use this line for ESP32 (VSPI)  (gpio22=RST, gpio5=CE for VSPI, gpio21=D/C)

const uint8_t Owl [] PROGMEM = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x20, 0x20,
0x20, 0x20, 0x20, 0x20, 0x32, 0x30, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x23, 0x22, 0x30, 0x20,
0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0xFE, 0xFE, 0xFE, 0x06, 0x06, 0x06, 0x06, 0xFE, 0x06, 0x06, 0x06, 0x06, 0xFE, 0x06, 0x06,
0x06, 0x06, 0xFE, 0xFE, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF,
0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x3F, 0x7F, 0x78, 0x78, 0x78, 0x78, 0x7F, 0x78, 0x78, 0x78,
0x78, 0x7F, 0x78, 0x78, 0x78, 0x78, 0x3F, 0x3F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 
};

BluetoothSerial SerialBT;

const int trigPin = 14;
const int echoPin = 12;

//define sound speed in cm/uS
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

long duration;
int distanceCm;
int fill_level;
float distanceInch;


  
void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  SerialBT.begin("Recycle_Bin"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
   /* Setup screen */
    display.begin();
    display.setFixedFont(ssd1306xled_font6x8);
    display.fill( 0x00 );
    display.printFixed(9, 8, "1 EPAL", STYLE_BOLD);
    display.printFixed(12, 16, "DRAMAS", STYLE_BOLD);
    display.printFixed(8,50, "PROJECT", STYLE_BOLD);
    display.printFixed(20,60 , "GHN", STYLE_BOLD);
    display.printFixed(18,90, "2022", STYLE_NORMAL);
    lcd_delay(10000);
    display.fill(0x00);
}

static void text()
{
    display.fill( 0x00 );
    display.drawBitmap1(5, 80, 50, 31, Owl);
    display.setFixedFont(ssd1306xled_font6x8);
    display.printFixed(0,8, "FILL LEVEL", STYLE_BOLD);
    display.printFixed(20,16, "BIN", STYLE_BOLD);
    display.printFixed(0,32, "Level:" , STYLE_NORMAL);
    display.setTextCursor(35, 32);
    display.print(fill_level);
    display.printFixed(55,32, "%" , STYLE_NORMAL);
    if (fill_level >= 0 && fill_level <= 20)
    {
      display.printFixed(18,50, "Empty" , STYLE_NORMAL);
    }
     else  if (fill_level >= 80 && fill_level <= 100)
    {
      display.printFixed(18,50, "Full" , STYLE_NORMAL);
    }
    else if (distanceCm > 70 || distanceCm < 0)
    {
      display.printFixed(18,50, "Out of range" , STYLE_NORMAL);
    }
}


void loop() {
    
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance
  distanceCm = duration * SOUND_SPEED/2;
  
  // Prints the distance in the Serial Monitor
  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);
  //calibrate fill level
  if (distanceCm > 60)
  {
  }
  else if ((distanceCm<=60) && (distanceCm>=10))
  {
   fill_level=map(distanceCm,10,60,100,0);
   SerialBT.println(fill_level);
  }
  Serial.println(fill_level);
  text();
  lcd_delay(300);
  delay(1000);
}
