#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 6
#define STRIP_LENGTH 240

#define SEPERATION 14
#define MAX_BRIGHTNESS 128
#define FLARE_COUNT 128
#define WAIT 2
#define BRIGHTNESS_STEP 2


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
uint32_t currentColor = strip.Color(22, 0, 64);
uint8_t baseColorChange = 0;

void setup() {
  // Turn on console logging
  // Serial.begin(9600);  
  
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
      fade(i, BRIGHTNESS_STEP);
    }
  }
  
  uint8_t currentBrightness = cgreen(strip.getPixelColor(brighteningOffset), 0, MAX_BRIGHTNESS);
  if ( currentBrightness >= MAX_BRIGHTNESS ) {
    brighteningOffset = (random(1, SEPERATION) + brighteningOffset) % SEPERATION;
    baseColorChange++;
  }
  
  if (baseColorChange >= FLARE_COUNT) {
    baseColorChange = 0;
    currentColor = randomPrettyColor();
  }
}

uint32_t randomPrettyColor() {
  uint32_t purple = strip.Color(22, 0, 64);
  uint32_t red = strip.Color(64, 0, 0);
  uint32_t blue = strip.Color(0, 0, 64);
  uint32_t colors[] = {purple, red, blue};
  
  uint32_t color = colors[random(0, 2)];
  return color;
}

uint32_t brighten(uint16_t index, int8_t amount){
  uint32_t color = strip.getPixelColor(index);
  uint8_t r = cred(color, 0, 255 - amount) + amount;   
  uint8_t g = cgreen(color, 0, 255 - amount) + amount;
  uint8_t b = cblue(color, 0, 255  - amount) + amount;
  color = strip.Color(r, g, b);
  strip.setPixelColor(index, color);
  return color;
}

uint32_t fade(uint16_t index, int8_t amount){
  
  uint32_t color = strip.getPixelColor(index);
  uint8_t currentBrightness = cgreen(strip.getPixelColor(brighteningOffset), 0, MAX_BRIGHTNESS);
  if ( currentBrightness <= MAX_BRIGHTNESS / 4) {
    amount = ceil(amount / 4.0F);
  } else if ( currentBrightness <= MAX_BRIGHTNESS / 2) {
    amount = ceil(amount / 2.0F);
  }
  
  uint8_t r = fadeToPoint(cred(color, 0, 255), currentRed(), amount); 
  uint8_t g = fadeToPoint(cgreen(color, 0, 255), currentGreen(), amount);
  uint8_t b = fadeToPoint(cblue(color, 0, 255), currentBlue(), amount);
  color = strip.Color(r, g, b);
  strip.setPixelColor(index, color);
  return color;
}

uint8_t fadeToPoint(uint8_t part, uint8_t target, uint8_t amount) {
  if ( part < target ) { 
    return part;
  } else  if ( part <= target + amount ) { 
    return target;
  } else if ( part < amount)  {
    return 0;    
  } else { 
    return part - amount; 
  }
}

uint8_t cred(uint32_t color, uint8_t minimum, uint8_t maximum) {
  return colorPart(16, color, minimum, maximum);
}
uint8_t cgreen(uint32_t color, uint8_t minimum, uint8_t maximum) {
  return colorPart(8, color, minimum, maximum);
}

uint8_t cblue(uint32_t color, uint8_t minimum, uint8_t maximum) {
  return colorPart(0, color, minimum, maximum);
}

uint8_t currentRed() {
  return cred(currentColor, 0, 255);
}

uint8_t currentGreen() {
  return cgreen(currentColor, 0, 255);
}

uint8_t currentBlue() {
  return cblue(currentColor, 0, 255);
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
