#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 6
#define STRIP_LENGTH 240
#define SPARKLES 20

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

uint8_t currentSparkle = 0;
uint8_t sparklerArray[SPARKLES];


void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code

  for(uint16_t i=0; i< sizeof(sparklerArray); i++) {    
    sparklerArray[i] = random(0, STRIP_LENGTH-1);        
  }

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'  
}

void loop() {
  // TODO find a softer light. 
  sparkler(strip.Color(128, 128, 128), 10);
}

void sparkler(uint32_t c, uint8_t wait) {
  fade(5);
  brighten(c, 10);      
  strip.show();  
  delay(wait);
}

void brighten(uint32_t c, uint8_t sparkleSpeed ){
  for(uint16_t i=0; i< sizeof(sparklerArray); i++) {    
    uint8_t randomSparkeSpeed = random(ceil(sparkleSpeed/2), sparkleSpeed);
    uint32_t color = strip.getPixelColor(sparklerArray[i]);
    uint8_t r = red(color, 0, 255 - randomSparkeSpeed) + randomSparkeSpeed;   
    uint8_t g = green(color, 0, 255 - randomSparkeSpeed) + randomSparkeSpeed;
    uint8_t b = blue(color, 0, 255  - randomSparkeSpeed) + randomSparkeSpeed;
    strip.setPixelColor(sparklerArray[i], r, g, b);
    if (r >= 254 || g >= 254 || b >= 254) {
      sparklerArray[i] = random(0, STRIP_LENGTH-1);
    }    
  }
}

void fade(uint8_t fadeAmount) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    uint32_t color = strip.getPixelColor(i);
    
    uint8_t r = red(color, 1, 255);
    uint8_t g = green(color, 1, 255);
    uint8_t b = blue(color, 1, 255);

    uint8_t downR = 1;//ceil((r / 128.0) * fadeAmount);
    uint8_t downG = 1;//ceil((g / 128.0) * fadeAmount);
    uint8_t downB = 1;//ceil((b / 128.0) * fadeAmount);

    boolean inSparklerArray = false;
    for(uint16_t j=0; j< sizeof(sparklerArray); j++) {
      if (sparklerArray[j] == i) {
        inSparklerArray = true;
      }
    }
    if ( !inSparklerArray ) {
      strip.setPixelColor(i, r - downR, g - downG, b - downB);
    }
  }  
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



