#include "config.h"
#ifdef BLUETOOTH_TX
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ble/gatt-service/battery_service_server.h"
#include "ble/gatt-service/device_information_service_server.h"
#include "ble/gatt-service/hids_device.h"
#include "bt.h"
#include "bt_profile.h"
#include "btstack.h"
#include "config.h"
#include "descriptors.h"
#include "endpoints.h"
#include "hid.h"
#include "shared_main.h"
#if DEVICE_TYPE_IS_KEYBOARD
    // Appearance HID - Keyboard (Category 15, Sub-Category 1)
    #define REPORT keyboard_mouse_descriptor
    #define APPEARANCE 0xC1
#else
    // Appearance HID - Gamepad (Category 15, Sub-Category 4)
    #define REPORT pc_descriptor
    #define APPEARANCE 0xC4
#endif
// static btstack_timer_source_t heartbeat;
static btstack_packet_callback_registration_t hci_event_callback_registration;
static btstack_packet_callback_registration_t sm_event_callback_registration;
static uint8_t battery = 100;
static hci_con_handle_t con_handle = HCI_CON_HANDLE_INVALID;
static uint8_t protocol_mode = 1;

static void packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);

const uint8_t adv_data[] = {
    // Flags general discoverable, BR/EDR not supported
    0x02,
    BLUETOOTH_DATA_TYPE_FLAGS,
    0x06,
    // Name
    0x0d,
    BLUETOOTH_DATA_TYPE_COMPLETE_LOCAL_NAME,
    'S',
    'a',
    'n',
    't',
    'r',
    'o',
    'l',
    'l',
    'e',
    'r',
    'B',
    'T',
    // 16-bit Service UUIDs
    0x03,
    BLUETOOTH_DATA_TYPE_COMPLETE_LIST_OF_16_BIT_SERVICE_CLASS_UUIDS,
    ORG_BLUETOOTH_SERVICE_HUMAN_INTERFACE_DEVICE & 0xff,
    ORG_BLUETOOTH_SERVICE_HUMAN_INTERFACE_DEVICE >> 8,
    0x03,
    BLUETOOTH_DATA_TYPE_APPEARANCE,
    APPEARANCE,
    0x03,
};
bool check_bluetooth_ready() {
    return con_handle != HCI_CON_HANDLE_INVALID;
}
int get_bt_address(uint8_t *addr) {
    bd_addr_t local_addr;
    gap_local_bd_addr(local_addr);
    memcpy(addr, bd_addr_to_str(local_addr), SIZE_OF_BD_ADDRESS);
    return SIZE_OF_BD_ADDRESS;
}
void send_report(uint8_t size, uint8_t *report) {
    if (con_handle != HCI_CON_HANDLE_INVALID) {
        hids_device_send_input_report(con_handle, report+1, size-1);
        // hids_device_request_can_send_now_event(con_handle);
    }
}
const uint8_t adv_data_len = sizeof(adv_data);

void set_battery_state(uint8_t state) {
    battery_service_server_set_battery_value(state);
}

static void le_keyboard_setup(void) {
    l2cap_init();

    sm_init();
    sm_set_authentication_requirements(SM_AUTHREQ_SECURE_CONNECTION | SM_AUTHREQ_MITM_PROTECTION | SM_AUTHREQ_BONDING);

    // setup ATT server
    att_server_init(profile_data, NULL, NULL);

    // setup battery service
    battery_service_server_init(battery);

    // setup device information service
    device_information_service_server_init();
    device_information_service_server_set_pnp_id(DEVICE_ID_VENDOR_ID_SOURCE_USB, ARDWIINO_VID, ARDWIINO_PID, USB_VERSION_BCD(DEVICE_TYPE, 0, 0));

    hids_device_init(0, REPORT, sizeof(REPORT));

    // setup advertisements
    uint16_t adv_int_min = 0x0030;
    uint16_t adv_int_max = 0x0030;
    uint8_t adv_type = 0;
    bd_addr_t null_addr;
    memset(null_addr, 0, 6);
    gap_advertisements_set_params(adv_int_min, adv_int_max, adv_type, 0, null_addr, 0x07, 0x00);
    gap_advertisements_set_data(adv_data_len, (uint8_t *)adv_data);
    gap_advertisements_enable(1);

    // register for HCI events
    hci_event_callback_registration.callback = &packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);

    // register for SM events
    sm_event_callback_registration.callback = &packet_handler;
    sm_add_event_handler(&sm_event_callback_registration);

    // register for HIDS
    hids_device_register_packet_handler(packet_handler);
}

static void packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size) {
    UNUSED(channel);
    UNUSED(size);

    if (packet_type != HCI_EVENT_PACKET) return;

    switch (hci_event_packet_get_type(packet)) {
        case HCI_EVENT_DISCONNECTION_COMPLETE:
            con_handle = HCI_CON_HANDLE_INVALID;
            printf("Disconnected\r\n");
            break;
        case SM_EVENT_JUST_WORKS_REQUEST:
            printf("Just Works requested\r\n");
            sm_just_works_confirm(sm_event_just_works_request_get_handle(packet));
            break;
        case SM_EVENT_NUMERIC_COMPARISON_REQUEST:
            printf("Confirming numeric comparison: %" PRIu32 "\r\n", sm_event_numeric_comparison_request_get_passkey(packet));
            sm_numeric_comparison_confirm(sm_event_passkey_display_number_get_handle(packet));
            break;
        case SM_EVENT_PASSKEY_DISPLAY_NUMBER:
            printf("Display Passkey: %" PRIu32 "\r\n", sm_event_passkey_display_number_get_passkey(packet));
            break;
        case SM_EVENT_PAIRING_COMPLETE:
            switch (sm_event_pairing_complete_get_status(packet)) {
                case ERROR_CODE_SUCCESS:
                    printf("Pairing complete, success\r\n");
                    break;
                case ERROR_CODE_CONNECTION_TIMEOUT:
                    printf("Pairing failed, timeout\r\n");
                    break;
                case ERROR_CODE_REMOTE_USER_TERMINATED_CONNECTION:
                    printf("Pairing failed, disconnected\r\n");
                    break;
                case ERROR_CODE_AUTHENTICATION_FAILURE:
                    printf("Pairing failed, authentication failure with reason = %u\r\n", sm_event_pairing_complete_get_reason(packet));
                    break;
                default:
                    break;
            }
            break;
        case HCI_EVENT_LE_META:
            switch (hci_event_le_meta_get_subevent_code(packet)) {
                case HCI_SUBEVENT_LE_CONNECTION_COMPLETE: {
                    // print connection parameters (without using float operations)
                    uint16_t conn_interval = hci_subevent_le_connection_complete_get_conn_interval(packet);
                    printf("LE Connection Complete:\r\n");
                    printf("- Connection Interval: %u.%02u ms\r\n", conn_interval * 125 / 100, 25 * (conn_interval & 3));
                    printf("- Connection Latency: %u\r\n", hci_subevent_le_connection_complete_get_conn_latency(packet));
                    break;
                }
                case HCI_SUBEVENT_LE_CONNECTION_UPDATE_COMPLETE: {
                    // print connection parameters (without using float operations)
                    uint16_t conn_interval = hci_subevent_le_connection_update_complete_get_conn_interval(packet);
                    printf("LE Connection Update:\r\n");
                    printf("- Connection Interval: %u.%02u ms\r\n", conn_interval * 125 / 100, 25 * (conn_interval & 3));
                    printf("- Connection Latency: %u\r\n", hci_subevent_le_connection_update_complete_get_conn_latency(packet));
                    break;
                }
                default:
                    break;
            }
            break;
        case HCI_EVENT_HIDS_META:
            switch (hci_event_hids_meta_get_subevent_code(packet)) {
                case HIDS_SUBEVENT_INPUT_REPORT_ENABLE:
                    con_handle = hids_subevent_input_report_enable_get_con_handle(packet);
                    gap_request_connection_parameter_update(con_handle, 6, 7, 0, 100);
                    printf("Input Report Characteristic Subscribed %u\r\n", hids_subevent_input_report_enable_get_enable(packet));
                    break;
                case HIDS_SUBEVENT_OUTPUT_REPORT_ENABLE:
                    con_handle = hids_subevent_output_report_enable_get_con_handle(packet);
                    printf("Output Report Characteristic Subscribed %u\r\n", hids_subevent_output_report_enable_get_enable(packet));
                    break;
                case HIDS_SUBEVENT_FEATURE_REPORT_ENABLE:
                    con_handle = hids_subevent_feature_report_enable_get_con_handle(packet);
                    printf("Feature Report Characteristic Subscribed %u\r\n", hids_subevent_feature_report_enable_get_enable(packet));
                    break;
                case HIDS_SUBEVENT_BOOT_KEYBOARD_INPUT_REPORT_ENABLE:
                    con_handle = hids_subevent_boot_keyboard_input_report_enable_get_con_handle(packet);
                    printf("Boot Keyboard Characteristic Subscribed %u\r\n", hids_subevent_boot_keyboard_input_report_enable_get_enable(packet));
                    break;
                case HIDS_SUBEVENT_PROTOCOL_MODE:
                    protocol_mode = hids_subevent_protocol_mode_get_protocol_mode(packet);
                    printf("Protocol Mode: %s mode\r\n", hids_subevent_protocol_mode_get_protocol_mode(packet) ? "Report" : "Boot");
                    break;
                case HIDS_SUBEVENT_SET_REPORT: {
                    uint16_t len = hids_subevent_set_report_get_report_length(packet);
                    uint8_t type = hids_subevent_set_report_get_report_type(packet);
                    uint8_t id = hids_subevent_set_report_get_report_id(packet);
                    const uint8_t* output = hids_subevent_set_report_get_report_data(packet);
                    hid_set_report(output, len, type, BLUETOOTH_REPORT);
                    break;
                }
                default:
                    break;
            }
            break;

        default:
            break;
    }
}

int btstack_main(void);
int btstack_main(void) {
    printf("Bt tx init\r\n");
    le_keyboard_setup();

    // turn on!
    hci_power_control(HCI_POWER_ON);

    return 0;
}
#endif