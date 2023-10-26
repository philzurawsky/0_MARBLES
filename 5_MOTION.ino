#define SPIRAL_STOP_MS 600
#define SPIRAL_SPIN_MS 400

void toggleSpiral() {
  spiralOn = !spiralOn;
  //Send Message
  xQueueSend(spiralMotorQueue, &spiralOn, 0);
}

void motion_update_pid() {
  calc_pid();
  xQueueSend(motorPowerQueue, &power_request, 0);
}

void start_motor() {
  double current_rpm = get_rpm();
  if (current_rpm < 800 && power_request < 255) {
    power_request = 255;
    xQueueSend(motorPowerQueue, &power_request, 0);
  }
  if (current_rpm >= 800) flywheelOn = true;
  else flywheelOn = false;
}

void handle_spiral(unsigned long curr_millis, unsigned long lastSpiralChange) {
  curr_millis = millis();
  unsigned long delta = curr_millis - lastSpiralChange;

  if (spiralOn && delta >= SPIRAL_SPIN_MS) {
    //Time to turn off
    toggleSpiral();
    lastSpiralChange = curr_millis;
  } else if (!spiralOn && delta >= SPIRAL_STOP_MS) {
    //Time to turn on
    toggleSpiral();
    lastSpiralChange = curr_millis;
    xQueueSend(spiralEvent, &power_request, 0);
  }
}

void TaskMotionCtl(void *pvParameters) {
  (void) pvParameters;
  pinMode(spiralPin, OUTPUT);

  int delta = 0;
  unsigned long curr_millis = millis();
  unsigned long lastSpiralChange = millis();
  bool valueFromQueue = true;

  double rpm = 0;

  setup_pid();

  for(;;) {
    
    handle_spiral(curr_millis, lastSpiralChange);

    if (flywheelOn) {
      motion_update_pid();
      func = "Running";
    }
    else {
      start_motor();
      func = "motor start";
    }

    vTaskDelay(( TickType_t ) 10);
  }
}