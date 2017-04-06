#ifdef ESP8266
  #define CLEAR_PANEL_PIN D8 // pin #10 on the blinkofant panel
  #define DATA_PIN D7 // pin #8
  #define CLOCK_PIN D5 // pin #4
#else 
  #define CLEAR_PANEL_PIN 10 // this goes on panel to pin #10 on the blinkofant panel
  #define DATA_PIN 11 // pin #8
  #define CLOCK_PIN 13 // pin #4
#endif

#define PANELS 1
#define PANELDATA_SIZE (10*PANELS)


uint8_t panelData[PANELDATA_SIZE];


#include <SPI.h>

void setupBlinkofant(int clear_pin, int data_pin, int clock_pin) {
  pinMode(clear_pin, OUTPUT);
  pinMode(data_pin, OUTPUT);
  pinMode(clock_pin, OUTPUT);

  digitalWrite(clear_pin, LOW);
  digitalWrite(data_pin, LOW);
  digitalWrite(clock_pin, LOW);

  SPI.begin();
  SPI.setBitOrder(LSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV128); // biggest divider there is.
}

void screen_off() {
  digitalWrite(CLEAR_PANEL_PIN, LOW);
}

void screen_on() {
  digitalWrite(CLEAR_PANEL_PIN, HIGH);
}


void setBlink(int value) {
  panelData[0] = value;
}

void setPixel(int x, int y, int value) {
  int index = (y + 1) + x * 10; // y+1 because 1st bit controlls blinking
  int byteNr = index >> 3; // division by 8
  int bitNr = index & 0x7; // rest bei div durch 8

  if (value)
  {
    panelData[byteNr] |= 1 << bitNr;
  }
  else
  {
    panelData[byteNr] &= ~(1 << bitNr);
  }
}

void setAllPixel(uint8_t value) {
  for (int i = 0; i < PANELDATA_SIZE; i++)
  {
    panelData[i] = value;
  }
}


void shiftPixelData() {
  screen_off();

#ifdef ESP8266
  for (int i = 0; i < PANELDATA_SIZE; i++) {
    SPI.transfer(panelData[i]);
  }
#else
  for (int i = 0; i < PANELDATA_SIZE; i++) {
    SPSR = panelData[i];
    while (!(SPSR & (1 << SPIF)));
  }
#endif
  screen_on();
}
/*
  blinkytest
  for blinkofant

  wizard23
  TomK32

  1 modul consumes 1.2A if fully lit
  If your LED drivers are the 5V compatible LS3654LS,
  you can remove the 7805 and 7808 regulators, and connect the
  5V and 8V with a 5V source. Saves space

  Useful information:
  http://metalab.at/wiki/Blinkofant
  https://www.binary-kitchen.de/wiki/doku.php?id=projekte:ledmatrix:start
  https://devlol.org/wiki/Blinkofant

*/

#include <Timezone.h>    // https://github.com/JChristensen/Timezone
  
TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120};     //Central European Summer Time
TimeChangeRule CET = {"CET ", Last, Sun, Oct, 3, 60}; //Central European Standard Time
Timezone timezone(CEST, CET);


#include <NTPClient.h> // https://github.com/arduino-libraries/NTPClient
#include <ESP8266WiFi.h> // https://github.com/esp8266/Arduino/
#include <WiFiUdp.h>
WiFiUDP ntpUDP;
// 0 minutes time offset, update every hour
// any timezone offset is dealt with by Timezone to get better precision
NTPClient timeClient(ntpUDP, "at.pool.ntp.org", 0, 60 * 60 * 1000);


char ssid[] = "/dev/lol";
char password[] = "4dprinter";
void connectWifi() {
    // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print("WiFi connected, IP address: ");
  Serial.println(WiFi.localIP());
}


time_t local_time;

void setup() {
  Serial.begin(9600);
  
  connectWifi();
  
  timeClient.begin();
  
  setupBlinkofant(CLEAR_PANEL_PIN, DATA_PIN, CLOCK_PIN);
}
int circle2[14][2] = {{4, 2}, {5, 2}, {6, 2}, {6, 3}, {6, 4}, {6, 5}, {5, 5}, {4, 5}, {3, 5}, {2, 5}, {2, 4}, {2, 3}, {2, 2}, {3, 2}};
int circle1[22][2] = {{4, 1}, {5, 1}, {6, 1}, {7, 1}, {7, 2}, {7, 3}, {7, 4}, {7, 5}, {7, 6}, {6, 6}, {5, 6}, {4, 6}, {3, 6}, {2, 6},
                      {1, 6}, {1, 5}, {1, 4}, {1, 3}, {1, 2}, {1, 1}, {2, 1}, {3, 1}};
int circle0[30][2] = {{4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}, {8, 1}, {8, 2}, {8, 3}, {8, 4}, {8, 5}, {8, 6}, {8, 7}, {7, 7}, {6, 7},
                      {5, 7}, {4, 7}, {3, 7}, {2, 7}, {1, 7}, {0, 7}, {0, 6}, {0, 5}, {0, 4}, {0, 3}, {0, 2}, {0, 1}, {0, 0}, {1, 0},
                      {2, 0}, {3, 0}};

void showClockPixel(int pos[2], bool val) {
  setPixel(pos[1], pos[0], val);
}
void showClockPixel(int pos[2]) {
  showClockPixel(pos, true);
}
void showClock() {
  // subtract from 1 as the panel's origin is in a different corner
  // second
  showClockPixel(circle0[(int)floor((1 - second(local_time) / 60.0) * 29)]);
  // minute
  showClockPixel(circle1[(int)floor((1 - minute(local_time) / 60.0) * 21)]);
  // hour
  showClockPixel(circle2[(int)floor((1 - (hour(local_time) % 12) / 12.0) * 13)]);

  // blink a colon every second
  if (second(local_time) & 1) {
    setPixel(3, 4, 1);
    setPixel(4, 4, 1);
  }
}

void runWild() {
  runWild(20);
}

void runWild(int del) {
  for (int i = 0; i < 30; i++) {
    if (i > 0) {
      showClockPixel(circle0[i-1], false);
    }
    showClockPixel(circle0[i]);
    shiftPixelData();
    delay(del);
  }
}

void loop ()
{
  timeClient.update();
  if (WiFi.status() != WL_CONNECTED) {
    connectWifi();
  }

  local_time = timezone.toLocal(timeClient.getEpochTime());

  setAllPixel(0);
  static int last_minute = minute(local_time);
  if (minute(local_time) != last_minute) {
    runWild();
    last_minute = minute(local_time);
  }

  showClock();
  shiftPixelData();

  delay( 100);

}
