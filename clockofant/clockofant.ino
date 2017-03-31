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

WiFiUDP ntpUDP;
// 0 minutes time offset, update every hour
// any timezone offset is dealt with by Timezone to get better precision
NTPClient timeClient(ntpUDP, "at.pool.ntp.org", 0, 60 * 60 * 1000);

static time_t local_time;


#include <SPI.h>
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

void setBlink(int value) {
  panelData[0] = value;
}


// set pixels on a specific panel
void setPixel(int x, int y, int panel, int value) {
  if (panel >= PANELS) {
    // out of bounds
    return;
  }
  setPixel(x + (panel * 8), y, value);
}

void setPixel(int x, int y, int value)
{
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

void setAllPixel(uint8_t value)
{
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

void setup()
{
  Serial.begin(9600);
  pinMode(CLEAR_PANEL_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);

  digitalWrite(CLEAR_PANEL_PIN, LOW);
  digitalWrite(DATA_PIN, LOW);
  digitalWrite(CLOCK_PIN, LOW);

  SPI.begin();
  SPI.setBitOrder(LSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV128); // biggest divider there is.
}


void screen_off()
{
  digitalWrite(CLEAR_PANEL_PIN, LOW);
}

void screen_on()
{
  digitalWrite(CLEAR_PANEL_PIN, HIGH);
}


void scanner(int panel) {
  static int col = 0;
  static int row = 0;
  static bool is_row = true;
  if (is_row) {
    for(int c=0; c < 8; c++) {
      setPixel(c, row, 1); 
    }
    row++;
    if (row > 8) { row = 0; is_row = !is_row; }
  } else {
    for(int r=0; r < 9; r++) {
      setPixel(col, r, 1);
    }
    col++;
    if (col > 9) { col = 0; is_row = !is_row; }
  }
  
  //setPixel(col, row, 1);
}


void loop ()
{
  
  setAllPixel(0);

  // uncomment any of these demo modes
  //snake();
  scanner(0);
  //stars(2, true);
  //flashPanels();
  //static int i = 0;
  //i = 10 + ((i + 1) % 100);
  //Serial.println((1.4 + sin((millis() / 500))) * 50);
  //stars((1.4 + sin((millis() / 500))) * 50, 1);
  //stars(10, 1, 0, 8, 0, 1);
  /*
  for(int d=0; d < 3; d++) {
    shiftPixelData();
    delay(100);
    for(int p=0; p < PANELDATA_SIZE; p++) {
      if(random(0, 7.) > 0) {
        panelData[p] = (int)panelData[p] << 1;
      }
    }
  }
  */
 // stars(5, 0, 0, 8, 2, 5);
  //soulmates(3, 2, 3, 1, 1 );

  shiftPixelData();

  delay( 100);

}
