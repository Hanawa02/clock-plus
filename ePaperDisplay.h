#define ENABLE_GxEPD2_GFX 0

#include <GxEPD2_4C.h> 

#include <Fonts/FreeSans9pt7b.h>      // font 9px
#include <Fonts/FreeSans12pt7b.h>     // font 12px
#include <Fonts/FreeMonoBold9pt7b.h>  // Default from examples
#include <Fonts/FreeSansBold24pt7b.h>


// -- Constants --

#define PORTRAIT 0
#define LANDSCAPE 1

// -- Variables --

// ePaper Pins mapped to ESP32-H2
#define EPD_CS 3
#define EPD_DC 2
#define EPD_RST 1
#define EPD_BUSY 0
#define EPD_SCK 4
#define EPD_MOSI 5

// Display Properties
#define HEIGHT 400
#define WIDTH 300

// #define MAX_HEIGHT(ROTATION) ROTATION = PORTRAIT ? HEIGHT : WIDTH

// Display Driver
#define GxEPD2_DRIVER_CLASS GxEPD2_420c_GDEY0420F51  // GDEY0420F51 400x300, HX8717 (no inking)

// Defines the display model(4.2inch (G) model - This uses the 4-color class)
GxEPD2_4C<GxEPD2_420c_GDEY0420F51, 400> display(GxEPD2_420c_GDEY0420F51(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY));  // GDEY0420F51 400x300, HX8717 (no inking)

void setupDisplay() {
  Serial.println("Setting up display");

  // Connect the pins structure
  SPI.begin(EPD_SCK, -1, EPD_MOSI, EPD_CS);

  display.init(115200, false, 20, false);
  display.setRotation(LANDSCAPE);
 display.fillScreen(GxEPD_WHITE); // set the background to white (fill the buffer with value for white)


}


// void updateTemperature(char temp[]) {
//     // Specify the coordinates of ONLY the temperature number box
//     uint16_t x = 100, y = 50, w = 60, h = 20;
//     display.setPartialWindow(x, y, w, h);

//     display.firstPage(); // Even with partial, GxEPD2 uses the loop for safety
//     do {
//         display.fillRect(x, y, w, h, GxEPD_WHITE); // Clear old number
//         display.setCursor(x, y + 15);

//         display.print(temp);
//     } while (display.nextPage());
// }

void printData(char label[], uint16_t value) {
  Serial.print(label);
  Serial.println(value);
}

void printDataText(char label[], char value[]) {
  Serial.print(label);
  Serial.println(value);
}


void updateTime(char time[]) {
  Serial.println("Starting time update on display!");

  // Specify the coordinates of ONLY the time
  uint16_t x = 0, y = 0, w = 400, h = 300;
  // display.setPartialWindow(x, y, w, h);
  display.setFullWindow();
  display.firstPage();  // Even with partial, GxEPD2 uses the loop for safety

  display.setTextColor(GxEPD_BLACK);
  display.setFont(&FreeSans12pt7b);
  display.setTextSize(5);

  // Adafruit_GFX has a handy method getTextBounds() to determine the boundary box for a text for the actual font
  int16_t tbx = x, tby = y;
  uint16_t tbw, tbh;  // boundary box window

  display.getTextBounds(time, 0, 0, &tbx, &tby, &tbw, &tbh);  // it works for origin 0, 0, fortunately (negative tby!)

  // center bounding box by transposition of origin:
  uint16_t text_x = x + ((w - tbw) / 2);
  uint16_t text_y = y + ((h - tbh) / 2);

  printData("x: ", x);
  printData("y: ", y);
  printData("tbx: ", tbx);
  printData("tby: ", tby);
  printData("tbw: ", tbw);
  printData("tbh: ", tbh);
  printData("text_x: ", text_x);
  printData("text_y: ", text_y);

  display.firstPage();

  do {
    display.fillScreen(GxEPD_WHITE);    // set the background to white (fill the buffer with value for white)
    display.setCursor(text_x, text_y);  // set the postition to start printing text

    display.print(time);  // print some text
    // tell the graphics class to transfer the buffer content (page) to the controller buffer
    // and to refresh to the screen
    display.display(false); // full update
    printDataText("Time updated:", time);
  } while (display.nextPage());
}


// void getCenteredTextCoordinates() {

//   int16_t tbx, tby;
//   uint16_t tbw, tbh;  // boundary box window
//   display.getTextBounds(HelloArduino, 0, 0, &tbx, &tby, &tbw, &tbh);
// }
