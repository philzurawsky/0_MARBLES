#include <PID_v1.h>

#define PID_SENSITIVITY 25

double aggKp=3, aggKi=1, aggKd=0;
double kp=0.24, ki=0.01, kd=0;
PID myPID(&current_rpm, &power_request, &rpm_setpoint, kp, ki, kd, DIRECT);

void setup_pid() {
  myPID.SetMode(AUTOMATIC);
}

void calc_pid() {
  double gap = abs(rpm_setpoint - current_rpm);

  if (gap < PID_SENSITIVITY) {
    myPID.SetTunings(kp, ki, kd);
  } else {
    myPID.SetTunings(aggKp, aggKi, aggKd);
  }
  myPID.Compute();
}