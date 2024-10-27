#include <stdint.h>

#include "defines.h"
#include "reports/controller_reports.h"
#include "state_translation/generic.h"

struct {
    union {
        signed int ltt : 6;
        struct {
            unsigned int ltt40 : 5;
            unsigned int ltt5 : 1;
        };
    };
} ltt_t;
struct {
    union {
        signed int rtt : 6;
        struct {
            unsigned int rtt0 : 1;
            unsigned int rtt21 : 2;
            unsigned int rtt43 : 2;
            unsigned int rtt5 : 1;
        };
    };
} rtt_t;

inline void wii_to_universal_report(const uint8_t *data, uint8_t len, uint8_t sub_type, USB_Host_Data_t *usb_host_data) {
    switch (sub_type) {
        case NUNCHUK: {
            WiiNunchukDataFormat_t *report = (WiiNunchukDataFormat_t *)data;
            usb_host_data->accelX = report->accelX10 | report->accelX92 << 2;
            usb_host_data->accelY = report->accelY10 | report->accelY92 << 2;
            usb_host_data->accelZ = report->accelZ10 | report->accelZ92 << 2;
            usb_host_data->nunchukC |= report->c;
            usb_host_data->nunchukZ |= report->z;
            break;
        }
        case CLASSIC_LOWRES: {
            WiiClassicDataFormat1_t *report = (WiiClassicDataFormat1_t *)data;
            usb_host_data->leftStickX = (report->leftStickX << 11) - INT16_MAX;
            usb_host_data->leftStickY = (report->leftStickY << 11) - INT16_MAX;
            usb_host_data->rightStickX = ((report->rightStickX0 | report->rightStickX21 << 1 | report->rightStickX43 << 3) << 12) - INT16_MAX;
            usb_host_data->rightStickY = (report->rightStickY << 12) - INT16_MAX;
            usb_host_data->leftTrigger = (report->leftTrigger20 | report->leftTrigger43 << 3) << 12;
            usb_host_data->rightTrigger = report->rightTrigger << 12;
            usb_host_data->leftShoulder = report->leftShoulder ? UINT16_MAX : 0;
            usb_host_data->rightShoulder = report->rightShoulder ? UINT16_MAX : 0;
            usb_host_data->start |= report->start;
            usb_host_data->guide |= report->guide;
            usb_host_data->back |= report->back;
            usb_host_data->dpadDown |= report->dpadDown;
            usb_host_data->dpadRight |= report->dpadRight;
            usb_host_data->dpadUp |= report->dpadUp;
            usb_host_data->dpadLeft |= report->dpadLeft;
            usb_host_data->a |= report->a;
            usb_host_data->b |= report->b;
            usb_host_data->x |= report->x;
            usb_host_data->y |= report->y;
            break;
        }
        case CLASSIC_HIGHRES: {
            WiiClassicDataFormat3_t *report = (WiiClassicDataFormat3_t *)data;
            usb_host_data->leftStickX = (report->leftStickX << 8) - INT16_MAX;
            usb_host_data->leftStickY = (report->leftStickY << 8) - INT16_MAX;
            usb_host_data->rightStickX = (report->rightStickX << 8) - INT16_MAX;
            usb_host_data->rightStickY = (report->rightStickY << 8) - INT16_MAX;
            usb_host_data->leftTrigger = report->leftTrigger << 8;
            usb_host_data->rightTrigger = report->rightTrigger << 8;

            usb_host_data->start |= report->start;
            usb_host_data->guide |= report->guide;
            usb_host_data->back |= report->back;
            usb_host_data->dpadDown |= report->dpadDown;
            usb_host_data->dpadRight |= report->dpadRight;
            usb_host_data->dpadUp |= report->dpadUp;
            usb_host_data->dpadLeft |= report->dpadLeft;
            usb_host_data->a |= report->a;
            usb_host_data->b |= report->b;
            usb_host_data->x |= report->x;
            usb_host_data->y |= report->y;
            break;
        }
        case UDRAW: {
            WiiUDrawDataFormat_t *report = (WiiUDrawDataFormat_t *)data;
            usb_host_data->mouse.x = ((report->x70 | report->x118 << 8) << 5) + INT16_MAX;
            usb_host_data->mouse.y = ((report->y70 | report->y118 << 8) << 5) + INT16_MAX;
            usb_host_data->leftTrigger = (report->pressure70 | report->pressure8 << 8) << 7;
            usb_host_data->a = report->up;
            usb_host_data->b = report->down;
            break;
        }
        case DRAWSOME: {
            WiiDrawsomeDataFormat_t *report = (WiiDrawsomeDataFormat_t *)data;
            usb_host_data->leftStickX = report->x + INT16_MAX;
            usb_host_data->leftStickY = report->y + INT16_MAX;
            usb_host_data->leftTrigger = report->pressure << 8;
            usb_host_data->a = report->up;
            usb_host_data->b = report->down;
            break;
        }
        case GUITAR: {
            WiiGuitarDataFormat3_t *report = (WiiGuitarDataFormat3_t *)data;
            usb_host_data->leftStickX = (report->leftStickX << 10) - INT16_MAX;
            usb_host_data->leftStickY = (report->leftStickY << 10) - INT16_MAX;
            usb_host_data->whammy = report->whammy << 3;
            if (report->slider == 0x0f) {
                usb_host_data->slider = 0x80;
            } else if (report->slider < 0x05) {
                usb_host_data->slider = 0x15;
            } else if (report->slider < 0x0A) {
                usb_host_data->slider = 0x30;
            } else if (report->slider < 0x0C) {
                usb_host_data->slider = 0x4D;
            } else if (report->slider < 0x12) {
                usb_host_data->slider = 0x66;
            } else if (report->slider < 0x14) {
                usb_host_data->slider = 0x9A;
            } else if (report->slider < 0x17) {
                usb_host_data->slider = 0xAF;
            } else if (report->slider < 0x1A) {
                usb_host_data->slider = 0xC9;
            } else if (report->slider < 0x1F) {
                usb_host_data->slider = 0xE6;
            } else {
                usb_host_data->slider = 0xFF;
            }

            usb_host_data->start |= report->start;
            usb_host_data->guide |= report->guide;
            usb_host_data->back |= report->back;
            usb_host_data->dpadDown |= report->dpadDown;
            usb_host_data->dpadRight |= report->dpadRight;
            usb_host_data->dpadUp |= report->dpadUp;
            usb_host_data->dpadLeft |= report->dpadLeft;
            usb_host_data->green |= report->green;
            usb_host_data->red |= report->red;
            usb_host_data->yellow |= report->yellow;
            usb_host_data->blue |= report->blue;
            usb_host_data->orange |= report->orange;
            break;
        }
        case DRUM: {
            // TODO: velocity maybe
            WiiDrumDataFormat3_t *report = (WiiDrumDataFormat3_t *)data;

            usb_host_data->leftStickX = (report->leftStickX << 10) - INT16_MAX;
            usb_host_data->leftStickY = (report->leftStickY << 10) - INT16_MAX;

            usb_host_data->start |= report->start;
            usb_host_data->guide |= report->guide;
            usb_host_data->back |= report->back;
            usb_host_data->dpadDown |= report->dpadDown;
            usb_host_data->dpadRight |= report->dpadRight;
            usb_host_data->dpadUp |= report->dpadUp;
            usb_host_data->dpadLeft |= report->dpadLeft;
            usb_host_data->green |= report->green;
            usb_host_data->red |= report->red;
            usb_host_data->yellow |= report->yellow;
            usb_host_data->blue |= report->blue;
            usb_host_data->orange |= report->orange;
            break;
        }
        case TURNTABLE: {
            WiiTurntableDataFormat3_t *report = (WiiTurntableDataFormat3_t *)data;
            usb_host_data->leftStickX = (report->leftStickX << 10) - INT16_MAX;
            usb_host_data->leftStickY = (report->leftStickY << 10) - INT16_MAX;
            ltt_t.ltt40 = report->leftTableVelocity40;
            ltt_t.ltt5 = report->leftTableVelocity5;
            rtt_t.rtt0 = report->rightTableVelocity0;
            rtt_t.rtt21 = report->rightTableVelocity21;
            rtt_t.rtt43 = report->rightTableVelocity43;
            rtt_t.rtt5 = report->rightTableVelocity5;
            usb_host_data->leftTableVelocity = ltt_t.ltt << 10;
            usb_host_data->rightTableVelocity = rtt_t.rtt << 10;
            usb_host_data->start |= report->start;
            usb_host_data->guide |= report->guide;
            usb_host_data->back |= report->back;
            usb_host_data->dpadDown |= report->dpadDown;
            usb_host_data->dpadRight |= report->dpadRight;
            usb_host_data->dpadUp |= report->dpadUp;
            usb_host_data->dpadLeft |= report->dpadLeft;
            usb_host_data->leftGreen |= report->leftGreen;
            usb_host_data->leftRed |= report->leftRed;
            usb_host_data->leftBlue |= report->leftBlue;
            usb_host_data->rightGreen |= report->rightGreen;
            usb_host_data->rightRed |= report->rightRed;
            usb_host_data->rightBlue |= report->rightBlue;
            break;
        }
        case TAIKO: {
            WiiTaTaConDataFormat_t *report = (WiiTaTaConDataFormat_t *)data;
            usb_host_data->green |= report->centerLeft;
            usb_host_data->red |= report->rimLeft;
            usb_host_data->yellow |= report->centerRight;
            usb_host_data->blue |= report->rimRight;
            break;
        }
    }
}

inline void universal_report_to_wii(uint8_t *data, uint8_t len, uint8_t sub_type, const USB_Host_Data_t *usb_host_data) {
    switch (sub_type) {
        case GUITAR_HERO_GUITAR: {
            WiiGuitarDataFormat3_t *report = (WiiGuitarDataFormat3_t *)data;
            memset(data, 0, sizeof(WiiGuitarDataFormat3_t));
            report->dpadUp = usb_host_data->dpadUp;
            report->dpadDown = usb_host_data->dpadDown;
            report->leftStickX = usb_host_data->leftStickX >> 2;
            report->leftStickY = usb_host_data->leftStickY >> 2;
            report->slider = 0x0F;
            // TODO: state translation for wii stuff as well?
            if ((usb_host_data->slider != PS3_STICK_CENTER)) {
                uint8_t slider_tmp = usb_host_data->slider;
                if (slider_tmp <= 0x15) {
                    report->slider = 0x04;
                } else if (slider_tmp <= 0x30) {
                    report->slider = 0x07;
                } else if (slider_tmp <= 0x4D) {
                    report->slider = 0x0a;
                } else if (slider_tmp <= 0x66) {
                    report->slider = 0x0c;
                } else if (slider_tmp <= 0x9A) {
                    report->slider = 0x12;
                } else if (slider_tmp <= 0xAF) {
                    report->slider = 0x14;
                } else if (slider_tmp <= 0xC9) {
                    report->slider = 0x17;
                } else if (slider_tmp <= 0xE6) {
                    report->slider = 0x1A;
                } else {
                    report->slider = 0x1F;
                }
            }
            report->green |= usb_host_data->green;
            report->red |= usb_host_data->red;
            report->yellow |= usb_host_data->yellow;
            report->blue |= usb_host_data->blue;
            report->orange |= usb_host_data->orange;
            report->whammy = usb_host_data->whammy >> 3;
            data[4] = ~data[4];
            data[5] = ~data[5];
        }
        case GUITAR_HERO_DRUMS: {
            WiiDrumDataFormat3_t *report = (WiiDrumDataFormat3_t *)data;
            memset(data, 0, sizeof(WiiDrumDataFormat3_t));
            report->leftStickX = usb_host_data->leftStickX >> 2;
            report->leftStickY = usb_host_data->leftStickY >> 2;
            report->dpadUp = usb_host_data->dpadUp;
            report->dpadDown = usb_host_data->dpadDown;
            report->dpadLeft = usb_host_data->dpadLeft;
            report->dpadRight = usb_host_data->dpadRight;
            report->green |= usb_host_data->green;
            report->red |= usb_host_data->red;
            report->yellow |= usb_host_data->yellow;
            report->blue |= usb_host_data->blue;
            report->orange |= usb_host_data->orange;
            // TODO: velocity
            data[4] = ~data[4];
            data[5] = ~data[5];
        }
        case DJ_HERO_TURNTABLE: {
            WiiTurntableDataFormat3_t *report = (WiiTurntableDataFormat3_t *)data;
            memset(data, 0, sizeof(WiiTurntableDataFormat3_t));
            report->leftStickX = (usb_host_data->leftStickX + INT16_MAX) >> 10;
            report->leftStickY = (usb_host_data->leftStickY + INT16_MAX) >> 10;
            ltt_t.ltt = usb_host_data->leftTableVelocity >> 10;
            rtt_t.rtt = usb_host_data->rightTableVelocity >> 10;
            report->leftTableVelocity40 = ltt_t.ltt40;
            report->leftTableVelocity5 = ltt_t.ltt5;
            report->rightTableVelocity0 = rtt_t.rtt0;
            report->rightTableVelocity21 = rtt_t.rtt21;
            report->rightTableVelocity43 = rtt_t.rtt43;
            report->rightTableVelocity5 = rtt_t.rtt5;
            report->start |= usb_host_data->start;
            report->guide |= usb_host_data->guide;
            report->back |= usb_host_data->back;
            report->dpadDown |= usb_host_data->dpadDown;
            report->dpadRight |= usb_host_data->dpadRight;
            report->dpadUp |= usb_host_data->dpadUp;
            report->dpadLeft |= usb_host_data->dpadLeft;
            report->leftGreen |= usb_host_data->leftGreen;
            report->leftRed |= usb_host_data->leftRed;
            report->leftBlue |= usb_host_data->leftBlue;
            report->rightGreen |= usb_host_data->rightGreen;
            report->rightRed |= usb_host_data->rightRed;
            report->rightBlue |= usb_host_data->rightBlue;
        }
        default: {
            // TODO: we can just go stragiht to the correct report format
            // TODO: could probably make TRANSLATE_* functions specifically for wii stuff so we dont have to repeat things
            // Report format 3 is reasonable, so we can convert from that
            WiiClassicDataFormat3_t temp_report;
            WiiClassicDataFormat3_t *report = &temp_report;
            memset(report, 0, sizeof(temp_report));
            memset(data, 0, sizeof(WiiClassicDataFormat3_t));
            // Center sticks
            report->leftStickX = PS3_STICK_CENTER;
            report->leftStickY = PS3_STICK_CENTER;
            report->rightStickX = PS3_STICK_CENTER;
            report->rightStickY = PS3_STICK_CENTER;

            report->guide |= usb_host_data->guide;
            TRANSLATE_GAMEPAD_NO_CLICK(PS3_JOYSTICK, PS3_XBOX_TRIGGER, usb_host_data, report);
            // button bits are inverted
            report->buttonsLow = ~report->buttonsLow;
            report->buttonsHigh = ~report->buttonsHigh;
            uint8_t format = wii_data_format();
            if (format == 3) {
                memcpy(data, report, sizeof(WiiClassicDataFormat3_t));
            } else if (format == 2) {
                WiiClassicDataFormat2_t *real_report = (WiiClassicDataFormat2_t *)data;
                real_report->buttonsLow = report->buttonsLow;
                real_report->buttonsHigh = report->buttonsHigh;
                real_report->leftStickX92 = report->leftStickX;
                real_report->leftStickY92 = report->leftStickY;
                real_report->rightStickX92 = report->rightStickX;
                real_report->rightStickY92 = report->rightStickY;
                real_report->leftTrigger = report->leftTrigger;
                real_report->rightTrigger = report->rightTrigger;
            } else if (format == 1) {
                // Similar to the turntable, classic format 1 is awful so we need to translate to an intermediate format first
                WiiIntermediateClassicDataFormat_t intermediate_report;
                intermediate_report.rightStickX = report->rightStickX >> 3;
                intermediate_report.leftTrigger = report->leftTrigger >> 3;
                WiiClassicDataFormat1_t *real_report = (WiiClassicDataFormat1_t *)data;
                real_report->buttonsLow = report->buttonsLow;
                real_report->buttonsHigh = report->buttonsHigh;
                real_report->leftStickX = report->leftStickX >> 2;
                real_report->leftStickY = report->leftStickY >> 2;
                real_report->rightTrigger = report->rightTrigger >> 3;
                real_report->rightStickX0 = intermediate_report.rightStickX0;
                real_report->rightStickX21 = intermediate_report.rightStickX21;
                real_report->rightStickX43 = intermediate_report.rightStickX43;
                real_report->leftTrigger20 = intermediate_report.leftTrigger20;
                real_report->leftTrigger43 = intermediate_report.leftTrigger43;
            }
            // TODO: allow swapping classic face buttons too
            break;
        }
    }
}