#include <Adafruit_NeoPixel.h>

// Initialize the NeoPixel library.
// Parameters: (number of pixels, pin number, pixel type)
Adafruit_NeoPixel LED_RGB(1, 38, NEO_GRBW + NEO_KHZ800);

// Define a structure to hold color values
struct Color {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t w;
};

// Create an array of colors to cycle through
const Color colors[] = {
  {255, 255, 255, 255}, // WHITE
  {255, 255, 0,   0},   // YELLOW
  {252, 98, 3,   0},   // ORANGE
  {255, 0,   0,   0},   // RED
  {128, 0, 128,   0},   // PURPLE
  {0,   0, 255,   0},   // BLUE
  {0, 255,   0,   0}    // GREEN
};

const int numColors = sizeof(colors) / sizeof(colors[0]);

void setup()
{
  LED_RGB.begin();             // Initialize the NeoPixel library.
  LED_RGB.setBrightness(150);  // Set brightness (0-255).
}

void loop()
{
  for(int i = 0; i < numColors; i++)
  {
    // Set the current color
    LED_RGB.setPixelColor(0, LED_RGB.Color(colors[i].r, colors[i].g, colors[i].b, colors[i].w));
    LED_RGB.show();
    delay(1000); // Wait for 1 second before the next color
  }
}
