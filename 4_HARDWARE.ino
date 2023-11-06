const TickType_t xBlockTime = pdMS_TO_TICKS(1);

Adafruit_NeoPixel strip(N_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

bool spiralOn = false; // Spiral on if true, off if false
bool flywheelOn = false; //Flywheel is on if true, off if false

uint32_t colorVals[N_LEDS] = {0};

int beamBreakPins[2] = {7, 8}; //Beam breaks connected to these pins

bool beamBreakArr[2] = {0,0}; // 0 for clear, 1 for broken

bool inSetup;


void write_motor_power() {
  analogWrite(MOTOR_PIN, power_request);
}

void pollBeamBreaks() {
  for(int i =0; i < 2; i++) {
    bool current = digitalRead(beamBreakPins[i]);
    //Serial.println(current);
    if (current == LOW && beamBreakArr[i] == HIGH) {
      //Send Event
      xQueueSend((i == 0) ? entranceEvent : exitEvent, &beamBreakArr, 0);
    }
    //Update state
    beamBreakArr[i] = current;
  }
}

void init_pins() {
  for (int i = 7; i <= 8; i++) {
    pinMode(i,INPUT_PULLUP);
  }

  pinMode(LED_PIN, OUTPUT);
}

String func = "Uninitialised";

void TaskHardwareInterface(void *pvParameters) {
  TickType_t xLastWakeTime;
  (void) pvParameters;

  //init_encoder();

  init_pins();

  strip.begin();
  strip.clear();

  bool valueFromQueue = true;

  //int tickCount = 0;

  Serial.begin(9600);
  while (!Serial) {
    vTaskDelay(1);
  }

  for(;;) {
    xLastWakeTime = xTaskGetTickCount();
    //handle RPM measurements
    handle_encoder();

    //Poll sensors
    pollBeamBreaks();

    //Control Spiral
    if (xQueueReceive(spiralMotorQueue, &spiralOn, 0) == pdPASS) {
      digitalWrite(spiralPin, spiralOn ? HIGH : LOW);
    }

    //Control PID
    //Serial.println(get_rpm());
    if (xQueueReceive(motorPowerQueue, &power_request, 0) == pdPASS) {
      write_motor_power();
    }

    if (power_request > 128) xQueueSend(exitEvent, &power_request, 0);

    //Output to Lighting
    //strip.clear();
    for(int i = 0; i < N_LEDS; i++) {
      strip.setPixelColor(i, colorVals[i]);
    }
    strip.show();
    //print_measurements();
  
    /*if (xSemaphoreTake(interruptSemaphore, portMAX_DELAY) == pdPASS) {
      Serial.println("Interrupt");
    }*/

    vTaskDelay(( TickType_t ) 1);
  }

}