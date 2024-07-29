#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <semphr.h>
#include <Adafruit_NeoPixel.h>
#include <util/atomic.h>

// define two tasks for Blink & AnalogRead
void TaskBlink( void *pvParameters );
void TaskAnalogRead( void *pvParameters );
void TaskHardwareInterface( void *pvParameters );

#define LED_PIN 9

#define N_LEDS 5
#define OFFSET 1

#define ENCODER_PIN 2
#define MOTOR_PIN 5

#define MIN_RPM 1000 // Minimum operational Flywheel RPM

//PINS

int spiralPin = 3;
int spiralPower = 128;


QueueHandle_t entranceEvent;
QueueHandle_t exitEvent;
QueueHandle_t spiralEvent;
QueueHandle_t ledUpdateQueue;
QueueHandle_t spiralMotorQueue;
QueueHandle_t spiralLightingQueue;

QueueHandle_t motorPowerQueue;




double rpm_setpoint = 1450;

