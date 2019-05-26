#include "InputHandler.h"
#include "../../io/bootloader/Bootloader.h"
void InputHandler::process() {
  if (input != NULL) {
    controller.buttons = 0;
    input->read_controller(&controller);
    if (config.map_joy_to_dpad) {
      CHECK_JOY(l_x, XBOX_DPAD_LEFT, XBOX_DPAD_RIGHT);
      CHECK_JOY(l_y, XBOX_DPAD_DOWN, XBOX_DPAD_UP);
    }
    guitar.handle(&controller);
  }
}
void InputHandler::init() {
  if (config.input_type == WII) {
    input = new WiiExtension();
  } else if (config.input_type == DIRECT) {
    input = new Direct();
  } else {
    return;
  }
  input->init();
  if (config.input_type == WII || config.tilt_type == MPU_6050) {
    I2Cdev::TWIInit();
  }
  IO::enableADC();
  guitar.init();
}
