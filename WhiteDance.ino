#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 6
#define STRIP_LENGTH 240

#define SEPERATION 7
#define MAX_BRIGHTNESS 128
#define WAIT 2
#define BRIGHTNESS_STEP 4


// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(STRIP_LENGTH, PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

uint8_t brighteningOffset = 0;
uint8_t tick = 0;

void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code

  uint8_t brighteningOffset = random(1, SEPERATION - 1);

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'  
}

void loop() {
  
  brightenConsistent();
  delay(WAIT);
  strip.show();  
}

void brightenConsistent() { 
  tick++;
  
  for(uint16_t i=0; i< STRIP_LENGTH; i++) {  
    if ((i % SEPERATION) == brighteningOffset) {
      brighten(i, BRIGHTNESS_STEP);
    } else {
      if ( tick % SEPERATION / 2 == 0) {
        fade(i, BRIGHTNESS_STEP);
      }
    }
  }
  
  uint8_t currentBrightness = red(strip.getPixelColor(brighteningOffset), 0, MAX_BRIGHTNESS);
  if ( currentBrightness >= MAX_BRIGHTNESS ) {
    brighteningOffset = (random(1, SEPERATION) + brighteningOffset) % SEPERATION;
  }
}

uint32_t brighten(uint16_t index, int8_t amount){
  uint32_t color = strip.getPixelColor(index);
  uint8_t r = red(color, 0, 255 - amount) + amount;   
  uint8_t g = green(color, 0, 255 - amount) + amount;
  uint8_t b = blue(color, 0, 255  - amount) + amount;
  color = strip.Color(r, g, b);
  strip.setPixelColor(index, color);
  return color;
}

uint32_t fade(uint16_t index, int8_t amount){
  uint32_t color = strip.getPixelColor(index);
  uint8_t currentBrightness = red(strip.getPixelColor(brighteningOffset), 0, MAX_BRIGHTNESS);
  if ( currentBrightness <= MAX_BRIGHTNESS / 4) {
    amount = ceil(amount / 4.0F);
  } else if ( currentBrightness <= MAX_BRIGHTNESS / 2) {
    amount = ceil(amount / 2.0F);
  }
  
  uint8_t r = red(color, 0 + amount, 255) - amount;   
  uint8_t g = green(color, 0 + amount, 255) - amount;
  uint8_t b = blue(color, 0 + amount, 255) - amount;
  color = strip.Color(r, g, b);
  strip.setPixelColor(index, color);
  return color;
}

uint8_t red(uint32_t color, uint8_t minimum, uint8_t maximum) {
  return colorPart(16, color, minimum, maximum);
}

uint8_t green(uint32_t color, uint8_t minimum, uint8_t maximum) {
  return colorPart(8, color, minimum, maximum);
}

uint8_t blue(uint32_t color, uint8_t minimum, uint8_t maximum) {
  return colorPart(0, color, minimum, maximum);
}

int8_t colorPart(uint8_t shift, uint32_t color, uint8_t minimum, uint8_t maximum) {
  uint8_t part = (uint8_t)((color >> shift) & 0xff);
  if ( part <= minimum) {
    return minimum;
  }
  if ( part >= maximum) {
    return maximum;
  }
  return part;
}





