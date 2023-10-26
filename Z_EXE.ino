void setup() {
  noInterrupts();
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  pinMode(ENCODER_PIN, INPUT_PULLUP);

  count_since_last_calc = 0;
  last_encoder_calc = micros();

  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN), encoder_interrupt, FALLING);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB, on LEONARDO, MICRO, YUN, and other 32u4 based boards.
  }

  entranceEvent=xQueueCreate(10, sizeof(int));
  exitEvent=xQueueCreate(10, sizeof(int));
  spiralMotorQueue=xQueueCreate(10, sizeof(int));
  motorPowerQueue=xQueueCreate(10, sizeof(int));
  spiralEvent=xQueueCreate(10, sizeof(int));
  
  if (entranceEvent != NULL && exitEvent != NULL && spiralMotorQueue != NULL && motorPowerQueue != NULL ) {
  xTaskCreate(
    TaskLighting
    ,  "Lighting"   // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );

    xTaskCreate(
    TaskHardwareInterface
    ,  "HardwareInterface"
    ,  128  // Stack size
    ,  NULL
    ,  3  // Priority
    ,  NULL );

    xTaskCreate(
      TaskMotionCtl
      , "MotionCtl"
      , 128
      , NULL
      , 3
      , NULL);
  }

  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
  interrupts();
}

void loop()
{
  // Empty. Things are done in Tasks.
}