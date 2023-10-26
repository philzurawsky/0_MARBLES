#define N_TRIGGERS 64
#define ENCODER_PIN 2
#define DEL 100
#define N_POLES 8

volatile uint16_t count_since_last_calc = 0;
unsigned long last_encoder_calc = 0;
double current_rpm = 0;
double power_request = 200;

void init_encoder() {
  pinMode(ENCODER_PIN, INPUT_PULLUP);

  count_since_last_calc = 0;
  last_encoder_calc = micros();

  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN), encoder_interrupt, FALLING);
}

double get_rpm() {
  return current_rpm;
}

void encoder_interrupt() {
  count_since_last_calc++;
}

void calc_rpm() {
  //noInterrupts();
  unsigned long now = millis();
  unsigned long delta = now - last_encoder_calc;
  uint16_t cnt;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    cnt = count_since_last_calc; //Ensure no interrups during copy and that it is done in 1 instruction
  }
  if (cnt == 0) {
    last_encoder_calc = now;
    count_since_last_calc = 0;
    current_rpm = 0;
    return;
  }
  double meas = ( (cnt / (N_POLES * 1.0000))/(delta/1000.0000) ) * 60.0000;
  double d = meas - current_rpm;
  current_rpm += d / 4; 

  last_encoder_calc = now;
  count_since_last_calc = 0;
  //interrupts();
}

void handle_encoder() {
  calc_rpm();
}