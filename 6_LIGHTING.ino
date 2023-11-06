#define DIR -1

#define ZONE1_START             0
#define ZONE1_END               11
#define ZONE1_ANIMATION_LENGTH  20

#define ZONE2_START             12
#define ZONE2_END               15
#define ZONE2_ANIMATION_LENGTH  50

#define ZONE3_START             15
#define ZONE3_END               18
#define ZONE3_ANIMATION_LENGTH  100

enum zone {
  ZONE_1,
  ZONE_2,
  ZONE_3
};

void setupAnimation() {
  for (int i = 0; i < N_LEDS; i++) {
    colorVals[i] = millis() % 1000 < 500 ? 0 : strip.ColorHSV(0, 255, 255);
  }
}

uint8_t getBrightness(uint8_t delay, int animationTicks) {
  int progress = animationTicks + DIR * delay - 50;
  return (exp(sin(progress / 100.0 * PI)) - 0.368) * 42.546; 
}

uint32_t getColor(uint8_t hue, uint8_t brightness) {
  uint16_t h = (2 << 16) * (hue / 255.00);
  uint8_t sat = 255;
  uint8_t br = (brightness/100)*255;
  uint32_t rgbcolor = strip.ColorHSV(h, sat, br);
  return strip.gamma32(rgbcolor);
}

uint16_t getHue(int cycleTicks, int currTicks, int delay) {
  uint16_t val = (exp(sin((currTicks + DIR * delay) / (1.00 * cycleTicks) * PI)) - 0.368) * 42.546;
    val = (val * 1.50 / 100) * (65535);
    return val;
}

void genAnimationFrame(int animationTickCount, int firstIndex, int lastIndex) {

    for(int i = 0; i <= (lastIndex - firstIndex); i++) {
      //uint8_t br = getBrightness(i * OFFSET, animationTickCount) * 255.00 / 100.00;
      uint8_t br = chase_fade_brightness(ZONE1_ANIMATION_LENGTH, 0, i, animationTickCount);
      //uint32_t val = strip.ColorHSV(sin(PI * 2.00 * animationTicks / COLOR_CYCLE) * 255, 255, br);
      uint16_t hue = getHue(500, (millis() / 15) % 500,i * OFFSET);
      uint8_t sat = 255;
      uint32_t val = strip.ColorHSV(hue, sat, br);
      //uint32_t val = getColor(hue, br);
      colorVals[i + firstIndex] = val;
    }
}

int chase_fade_brightness(int duration, int delay, int index, int time) {
  int t = time - (index) * duration;
  if (t < 0) return 255;
  if (t >= duration) return 0;
  return (255 * cos(((2 * PI)/(4 * duration) )* t));
}

void getSpiralAnimation(int animationTickFrame, int firstIndex, int lastIndex) {
  if (animationTickFrame > 3000 || animationTickFrame < 0) return;
  for(int i = 0; i < (lastIndex - firstIndex); i++) {
    uint32_t val = strip.ColorHSV(1024, 255, chase_fade_brightness(ZONE2_ANIMATION_LENGTH, 50, i, animationTickFrame));
    colorVals[firstIndex + i] = val;
  }
}

void getExitAnimation(int animationTickFrame, int firstIndex, int lastIndex) {
  for(int i = 0; i < (lastIndex - firstIndex); i++) {
    uint32_t val = strip.ColorHSV(32000, 255, chase_fade_brightness(ZONE3_ANIMATION_LENGTH, 0, 1, animationTickFrame));
    colorVals[firstIndex + i] = val;
  }
}

void TaskLighting(void *pvParameters) {
  (void) pvParameters;
  TickType_t lastAnimationTrigger = xTaskGetTickCount();
  bool valueFromQueue = true;
  int delta;

  unsigned long lastEntranceActivation = millis();
  unsigned long lastExitActivation = millis();
  unsigned long lastSpiralActivation = millis();

  for(;;) {
    unsigned long now = millis();
    delta = (int) (xTaskGetTickCount() - lastAnimationTrigger);

    if (inSetup) {
      setupAnimation();
    } else {
      //On queue read, reset the ticks
      if(xQueueReceive(entranceEvent, &valueFromQueue, 0) == pdPASS) {
        lastEntranceActivation = millis();
      }

      if(xQueueReceive(exitEvent, &valueFromQueue, 0) == pdPASS) {
        lastExitActivation = millis();
      }

      if(xQueueReceive(spiralEvent, &valueFromQueue, 0) == pdPASS) {
        lastSpiralActivation = millis();
      }

      //Entrance Animation
      genAnimationFrame(now - lastEntranceActivation, ZONE1_START, ZONE1_END);
      // if (delta <= 200) {
      //   genAnimationFrame(millis(), 0, 11);
      //   //xQueueSend()
      // }
      //Spiral Animation
      getSpiralAnimation((millis()) % 1000, ZONE2_START, ZONE2_END);

      //Exit Animation
      getExitAnimation(now - lastExitActivation, ZONE3_START, ZONE3_END);
    }

    vTaskDelay(( TickType_t ) 1);
  }
  
}