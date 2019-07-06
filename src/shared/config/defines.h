#pragma once
#define NONE 0

//Tilt detection
#define MPU_6050 1
#define GRAVITY 2

//Input types
#define WII 1
#define DIRECT 2

//Joystick type for a guitar
#define DPAD 1
#define JOY 2

//Controller Types
#define XINPUT_GAMEPAD_SUBTYPE 1
#define XINPUT_WHEEL_SUBTYPE 2
#define XINPUT_ARCADE_STICK_SUBTYPE 3
#define XINPUT_FLIGHT_STICK_SUBTYPE 4
#define XINPUT_DANCE_PAD_SUBTYPE 5
#define XINPUT_GUITAR_SUBTYPE 6
#define XINPUT_GUITAR_ALTERNATE_SUBTYPE 7
#define XINPUT_DRUM_SUBTYPE 8
#define XINPUT_GUITAR_BASS_SUBTYPE 11
#define XINPUT_ARCADE_PAD_SUBTYPE 19
#define KEYBOARD_SUBTYPE 20
#define SWITCH_GAMEPAD_SUBTYPE 21
#define PS3_GAMEPAD_SUBTYPE 22
#define PS3_GUITAR_GH_SUBTYPE 23
#define PS3_GUITAR_RB_SUBTYPE 24
#define PS3_DRUM_GH_SUBTYPE 23
#define PS3_DRUM_RB_SUBTYPE 24

#define INVALID_PIN 0xFF

//Orientations for gyros
#define POSITIVE_Z 0
#define NEGATIVE_Z 1
#define POSITIVE_Y 2
#define NEGATIVE_Y 3
#define POSITIVE_X 4
#define NEGATIVE_X 5

//Firmware types
#define GUITAR_DEVICE_TYPE 0xfea123
#define ARDWIINO_DEVICE_TYPE 0xa2d414