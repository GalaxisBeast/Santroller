#define PRO_KEY_COUNT 25
#define PRO_KEY_FIRST_NOTE 24
#define DEFAULT_VELOCITY 64
// Translate a Pro keys style report to midi + usb host
#define TRANSLATE_PRO_KEYS                                                                 \
    usb_host_data->a |= report->a;                                                         \
    usb_host_data->b |= report->b;                                                         \
    usb_host_data->x |= report->x;                                                         \
    usb_host_data->y |= report->y;                                                         \
    usb_host_data->back |= report->back;                                                   \
    usb_host_data->start |= report->start;                                                 \
    usb_host_data->guide |= report->guide;                                                 \
    if (report->pedalDigital) {                                                            \
        onControlChange(0, MIDI_CONTROL_COMMAND_SUSTAIN_PEDAL, 0x7F);                      \
    } else if (~(report->pedalAnalog)) {                                                   \
        onControlChange(0, MIDI_CONTROL_COMMAND_SUSTAIN_PEDAL, ~(report->pedalAnalog));    \
    } else {                                                                               \
        onControlChange(0, MIDI_CONTROL_COMMAND_SUSTAIN_PEDAL, 0);                         \
    }                                                                                      \
    onControlChange(0, MIDI_CONTROL_COMMAND_MOD_WHEEL, report->overdrive ? 0xFF : 0x00);   \
    /* uint8 -> int14 (but only the positive half!) */                                     \
    onPitchBend(0, report->touchPad << 5);                                                 \
    uint32_t keyMask =                                                                     \
        (((uint32_t)report->keys[0]) << 17) |                                              \
        (((uint32_t)report->keys[1]) << 9) |                                               \
        (((uint32_t)report->keys[2]) << 1) |                                               \
        (((uint32_t)report->velocities[0] & 0x80) >> 7);                                   \
                                                                                           \
    int pressed = 0; /* Number of keys pressed */                                          \
    for (uint32_t i = 0; i < PRO_KEY_COUNT; i++) {                                         \
        /* Check if the key is pressed */                                                  \
        if (keyMask & (1 << (PRO_KEY_COUNT - 1 - i))) {                                    \
            /* There are only 5 key velocities stored*/                                    \
            if (pressed < 5) {                                                             \
                /* Retrieve velocity (ranges from 0-127)*/                                 \
                onNote(1, PRO_KEY_FIRST_NOTE + i, (report->velocities[pressed++] & 0x7F)); \
            } else {                                                                       \
                onNote(1, PRO_KEY_FIRST_NOTE + i, DEFAULT_VELOCITY);                       \
            }                                                                              \
        } else {                                                                           \
            offNote(1, PRO_KEY_FIRST_NOTE + i, 0);                                         \
        }                                                                                  \
    }
