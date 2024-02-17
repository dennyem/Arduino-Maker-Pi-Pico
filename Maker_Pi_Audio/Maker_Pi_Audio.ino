/************ Maker Pi SD card is on SPI1 **************
** For this to work 3 lines need to be added to the config file at:
**    C:\Users\yourName\AppData\Local\Arduino15\packages\rp2040\hardware\rp2040\3.7.1\libraries\ESP8266SdFat\src\SdFatConfig.h
**
** Right click on "SD_CONFIG" below and select "Go to Definition" to open the file here, it will be locked however.
**
** Add the new lines following these in the file
      #endif  // __AVR__
    //
**
** Add thefollowing 5 lines

#define SDCARD_SPI SPI1
#define SDCARD_SS_PIN 15
#define SDCARD_MOSI_PIN 11
#define SDCARD_MISO_PIN 12
#define SDCARD_SCK_PIN 10

***************************************************/

#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_GC9A01A.h"
#include "audio.h"
#include "SdFat.h"        //Included in Earle Philhower RP2040 Core
#include "TimeLib.h"

void showTime(time_t t);
void sayNumber(int hr);

// SD_FAT_TYPE = 0 for SdFat/File as defined in SdFatConfig.h,
// 1 for FAT16/FAT32, 2 for exFAT, 3 for FAT16/FAT32 and exFAT.
#define SD_FAT_TYPE 3
#define SD_CS       15

#define TFT_CLK  6
#define TFT_MOSI 7
#define TFT_DC   9
#define TFT_CS   17
#define TFT_RST  4
#define TFT_BL   8    // Backlight control

Adafruit_GC9A01A tft(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK,TFT_RST);

// Max SPI clock for an SD is 50Mhz. Reduce SPI_CLOCK if errors occur.
#define SPI_CLOCK SD_SCK_MHZ(16)

const uint8_t SD_CS_PIN = SD_CS;
// Try to select the best SD card configuration.
#if HAS_SDIO_CLASS
#define SD_CONFIG SdioConfig(FIFO_SDIO)
#elif  ENABLE_DEDICATED_SPI
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, DEDICATED_SPI, SPI_CLOCK)
#else  // HAS_SDIO_CLASS
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, SHARED_SPI, SPI_CLOCK)
#endif  // HAS_SDIO_CLASS

#if SD_FAT_TYPE == 0
SdFat sd;
File file;
File root;
#elif SD_FAT_TYPE == 1
SdFat32 sd;
File32 file;
File32 root;
#elif SD_FAT_TYPE == 2
SdExFat sd;
ExFile file;
ExFile root;
#elif SD_FAT_TYPE == 3
SdFs sd;
FsFile file;
FsFile root;
#endif  // SD_FAT_TYPE

const float pi = 3.14159267;
int  clkCentre_x;
int  clkCentre_y;
long milliSecs;
bool SndPlayed = false;
tmElements_t tm;
time_t       t;

#define innerRadius 100
#define outerRadius 115
#define HOUR   1
#define MINUTE 0

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 5000);
  Serial.println("Setting up\n");

  tm.Year   = 53;   //This is just a basic time setting. Add a GPS or get time from UTP if using PicoW
  tm.Month  = 2;
  tm.Day    = 17;
  tm.Hour   = 7;
  tm.Minute = 44;
  tm.Second = 0;
  t = makeTime(tm);
  setTime(t);

  audioInit();
  audioSetRate(8000);   //This gets changed if a wave file on the SD card is accessed
  delay(1000);
  playSound(&sd, "CHEERIO.WAV"); 

  SPI1.setRX(12);   //SPI_rx
  SPI1.setTX(11);   //SPI_tx
  SPI1.setSCK(10);  //SPI_clk

  // Initialize the SD card.
  if (!sd.begin(SD_CONFIG)) {
    Serial.println("SD card problem");
  } else {
    Serial.println("File System mounted successfully");
  }

  switch (sd.card()->type()) {
    case SD_CARD_TYPE_SD1:
      Serial.println("Card is SD1");
      break;
    case SD_CARD_TYPE_SD2:
      Serial.println("Card is SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      Serial.println("Card is SDHC");
      break;
    default:
      Serial.println("Card type is Unknown");
      break;
  }

  Serial.println("List of files on the SD.");
  sd.ls("/", LS_R | LS_DATE | LS_SIZE);

  tft.begin();

#if defined(TFT_BL)
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH); // Backlight on
#endif // end TFT_BL

  clkCentre_x = tft.width() / 2;
  clkCentre_y = tft.height() / 2;
  
  tft.fillScreen(GC9A01A_BLACK);
  draw_clock_face();
  Serial.println(F("Done!"));

  milliSecs = millis();
}

void loop() {
  static int min;

  if((SndPlayed == true) && (min != minute())) {
    SndPlayed = false;
  }

  showTime(now());

  switch(minute()) {
    case 0:  if(SndPlayed == false) {
               playSound(&sd, "ITS.WAV"); sayNumber(HOUR); playSound(&sd, "OC.WAV");
               SndPlayed = true;
               min = minute();
             }
             break;
    case 15: if(SndPlayed == false) {
               playSound(&sd, "ITS.WAV"); sayNumber(HOUR); sayNumber(MINUTE);
               SndPlayed = true;
               min = minute();
             }
             break;
    case 30: if(SndPlayed == false) {
               playSound(&sd, "ITS.WAV"); sayNumber(HOUR); sayNumber(MINUTE);
               SndPlayed = true;
               min = minute();
             }       
             break;
    case 45: if(SndPlayed == false) {
               playSound(&sd, "ITS.WAV"); sayNumber(HOUR); playSound(&sd, "40.WAV"); playSound(&sd, "5.WAV"); 
               SndPlayed = true;
               min = minute();
             }       
             break;
  }
}

void sayNumber(int hr) {
  char s[32];

  while(audioBytes() > 0) {    //Wait for current sound to finish
    yield();
  }

  if(hr) {
    sprintf(s, "%d.WAV", hour());
  } else {
    sprintf(s, "%d.WAV", minute());
  }

  playSound(&sd, s); 

  while(audioBytes() > 0) {    //Wait for current sound to finish
    yield();
  }
}

void draw_clock_face(void) {
  int x, y, x1, y1;

  // draw the center of the clock
  tft.drawCircle(clkCentre_x, clkCentre_y, 118, GC9A01A_YELLOW);
  tft.fillCircle(clkCentre_x, clkCentre_y, 100, GC9A01A_BLACK);
  tft.fillCircle(clkCentre_x, clkCentre_y, 5, GC9A01A_GREEN);

  // draw hour pointers around the face of a clock
  for (int i = 0; i < 12; i++) {
    y = (outerRadius * cos(pi - (2 * pi) / 12 * i)) + clkCentre_y;
    x = (outerRadius * sin(pi - (2 * pi) / 12 * i)) + clkCentre_x;
    y1 = (innerRadius * cos(pi - (2 * pi) / 12 * i)) + clkCentre_y;
    x1 = (innerRadius * sin(pi - (2 * pi) / 12 * i)) + clkCentre_x;
    tft.drawLine(x1, y1, x, y, GC9A01A_GREEN);
  }
}

void showTime(time_t t) {
  static int sec = 0;
  static int secsX, secsY, secsXOld, secsYOld;
  static int minsX, minsY, minsXOld, minsYOld;
  static int hourX, hourY, hourXOld, hourYOld, hourX1, hourY1, hourX1Old, hourY1Old;

  secsXOld  = secsX;  //Save the values for blanking out the old hands
  secsYOld  = secsY;
  minsXOld  = minsX;
  minsYOld  = minsY;
  hourXOld  = hourX;
  hourYOld  = hourY;
  hourX1Old = hourX1;
  hourY1Old = hourY1;

  tft.drawCircle(clkCentre_x, clkCentre_y, 5, GC9A01A_GREEN);    //Redraw the centre as it gets progressively overwritten

  if(sec != second()) {
    sec = second();

    // Seconds hand display
    secsY = (innerRadius * cos(pi - (2 * pi) / 60 * second(t))) + clkCentre_y;
    secsX = (innerRadius * sin(pi - (2 * pi) / 60 * second(t))) + clkCentre_x;
    tft.drawLine(clkCentre_x, clkCentre_y, secsXOld, secsYOld, GC9A01A_BLACK);  //Wipe out last hand
    tft.drawLine(clkCentre_x, clkCentre_y, secsX, secsY, GC9A01A_GREEN);        //Draw new hand

    //Minutes hand display
    minsY = (innerRadius * cos(pi - (2 * pi) / 60 * minute(t))) + clkCentre_y;
    minsX = (innerRadius * sin(pi - (2 * pi) / 60 * minute(t))) + clkCentre_x;
    tft.drawLine(clkCentre_x - 1, clkCentre_y - 1, minsXOld, minsYOld, GC9A01A_BLACK);  //Wipe out last hand
    tft.drawLine(clkCentre_x, clkCentre_y, minsXOld, minsYOld, GC9A01A_BLACK);
    tft.drawLine(clkCentre_x + 1, clkCentre_y + 1, minsXOld, minsYOld, GC9A01A_BLACK);
    tft.drawLine(clkCentre_x - 1, clkCentre_y - 1, minsX, minsY, GC9A01A_YELLOW);       //Draw new hand
    tft.drawLine(clkCentre_x, clkCentre_y, minsX, minsY, GC9A01A_YELLOW);
    tft.drawLine(clkCentre_x + 1, clkCentre_y + 1, minsX, minsY, GC9A01A_YELLOW);

    //Hour hand display
    hourY = ((innerRadius - 20) * cos(pi - (2 * pi) / 12 * hour(t) - (2 * pi) / 720 * minute(t))) + clkCentre_y;
    hourX = ((innerRadius - 20) * sin(pi - (2 * pi) / 12 * hour(t) - (2 * pi) / 720 * minute(t))) + clkCentre_x;
    hourY1 = ((innerRadius - 20) * cos(pi - (2 * pi) / 12 * hour(t) - (2 * pi) / 720 * minute(t))) + clkCentre_y;
    hourX1 = ((innerRadius - 20) * sin(pi - (2 * pi) / 12 * hour(t) - (2 * pi) / 720 * minute(t))) + clkCentre_x;
    tft.drawLine(clkCentre_x - 1, clkCentre_y - 1, hourXOld, hourYOld, GC9A01A_BLACK);    //Wipe out last hand
    tft.drawLine(clkCentre_x, clkCentre_y, hourX1Old, hourY1Old, GC9A01A_BLACK);
    tft.drawLine(clkCentre_x + 1, clkCentre_y + 1, hourX1Old, hourY1Old, GC9A01A_BLACK);
    tft.drawLine(clkCentre_x - 1, clkCentre_y - 1, hourX, hourY, GC9A01A_WHITE);          //Draw new hand
    tft.drawLine(clkCentre_x, clkCentre_y, hourX1, hourY1, GC9A01A_WHITE);
    tft.drawLine(clkCentre_x + 1, clkCentre_y + 1, hourX1, hourY1, GC9A01A_WHITE);
  }
}

