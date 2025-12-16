#include <picobricks.h>

#define MOTOR_1 21  // Door
#define MOTOR_2 22  // Window

int windowClose = 60;
int doorClose   = 150;

ServoSimple ServoDoor(MOTOR_1);
ServoSimple ServoWindow(MOTOR_2);

void setup() {
  ServoDoor.begin();
  ServoWindow.begin();

  ServoDoor.setAngle(doorClose);       // servo 1
  ServoWindow.setAngle(windowClose);   // servo 2
}

void loop() {

}
