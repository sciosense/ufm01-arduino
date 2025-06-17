#ifndef SCIOSENSE_UFM01_DEFINES_C_H
#define SCIOSENSE_UFM01_DEFINES_C_H

#include <inttypes.h>

//// Output data structure adresses for the measurement data struct and sensor info struct
typedef uint8_t ResultAddress;

//// Addresses of the UFM01 for One Wire communication
#define UFM01_REGISTER_ADDRESS_INFLL                    (0x00)      // Lower byte of instantaneous flow measurement
#define UFM01_REGISTER_ADDRESS_INFLM                    (0x02)      // Medium byte of instantaneous flow measurement
#define UFM01_REGISTER_ADDRESS_INFLH                    (0x04)      // Higher byte of instantaneous flow measurement
#define UFM01_REGISTER_ADDRESS_INFLCRC8                 (0x06)      // Instantaneous flow measurement CRC
#define UFM01_REGISTER_ADDRESS_TL                       (0x08)      // Lower byte of temperature measurement
#define UFM01_REGISTER_ADDRESS_TM                       (0x0A)      // Medium byte of temperature measurement
#define UFM01_REGISTER_ADDRESS_TH                       (0x0C)      // Higher byte of temperature measurement
#define UFM01_REGISTER_ADDRESS_TCRC8                    (0x0E)      // Temprature measurement CRC
#define UFM01_REGISTER_ADDRESS_CUFLL                    (0x10)      // Lower byte of accumulated flow measurement
#define UFM01_REGISTER_ADDRESS_CUFLM                    (0x12)      // Medium byte of accumulated flow measurement
#define UFM01_REGISTER_ADDRESS_CUFLH                    (0x14)      // Higher byte of accumulated flow measurement
#define UFM01_REGISTER_ADDRESS_CUFLCRC8                 (0x16)      // Accumulated flow measurement CRC

//// Error codes
typedef uint16_t Ufm01_ErrorCode;
#define UFM01_ERROR_CODE_FLOW_RATE_OUT_OF_RANGE         (1 << 02)   // Big flow rate out of range continuously detected for 5s
#define UFM01_ERROR_CODE_FLOW_DIRECTION_WRONG           (1 << 03)   // Negative flow detected
#define UFM01_ERROR_CODE_UFC_CHIP_ERROR                 (1 << 05)   // UFC not detected or abnormal
#define UFM01_ERROR_CODE_EMPTY_TUBE                     (1 << 13)   // No water detected in the tube

//// Measurement Modes
typedef uint8_t Ufm01_OperatingMode;
#define UFM01_MEASUREMENT_MODE_PASSIVE                  (0x0)       // Device sends data on request
#define UFM01_MEASUREMENT_MODE_ACTIVE                   (0x1)       // Device sends data every second (Default)

//// Command protocol structure values
#define UFM01_COMMAND_LENGTH                            (7)
#define UFM01_COMMAND_SET_CONFIGURATION_LENGTH          (25)
typedef const uint8_t Ufm01_Command[UFM01_COMMAND_SET_CONFIGURATION_LENGTH];
//// Commands of the UFM01 for One Wire communication
#define UFM01_COMMAND_CODE_CLEAR_ACCUMULATED            (0x5A)          // Command to clear the accumulated flow when communicating through One-Wire
#define UFM01_COMMAND_CODE_READ_DATA                    (0x5B)          // Command to read data from the specified registers when communicating through One-Wire
#define UFM01_COMMAND_CODE_RESET_MODULE                 (0x5D)          // Command to reset the module when communicating through One-Wire

#define UFM01_COMMAND_READ_CLEAR_ACCUMULATED_ONEWIRE    {0x5A}          // Command to start reading the data from the device from address 0x30 through One-Wire
#define UFM01_COMMAND_READ_DATA_ONEWIRE                 {0x5B, 0x30}    // Command to start reading the data from the device from address 0x30 through One-Wire
#define UFM01_COMMAND_RESET_MODULE_ONEWIRE              {0x5D}          // Command to start reading the data from the device from address 0x30 through One-Wire

#define UFM01_COMMAND_CLEAR_ACCUMULATED_CODE_LENGTH     (1)
#define UFM01_COMMAND_READ_DATA_CODE_LENGTH             (2)
#define UFM01_COMMAND_CLEAR_ACCUMULATED_CODE_LENGTH     (1)

//// Commands of the UFM01 for Serial communication
#define UFM01_COMMAND_ADDRESS_START_BYTE_1              (0xFE)
#define UFM01_COMMAND_ADDRESS_START_BYTE_2              (0xFE)
#define UFM01_COMMAND_ADDRESS_START_BYTE_3              (0x11)
#define UFM01_COMMAND_ADDRESS_STOP_BYTE                 (0x16)

#define UFM01_COMMAND_STARTUP_FLOW_RATE_LENGTH          (3)         // Length in bytes of the Startup Flow Rate parameter

#define UFM01_COMMAND_CONFIGURATION_START_BYTE          (3)         // First byte of the payload in the configuration command
#define UFM01_COMMAND_A0_START_BYTE                     (5)         // First byte of the A0 configuration in the configuration command
#define UFM01_COMMAND_A1_START_BYTE                     (7)         // First byte of the A1 configuration in the configuration command
#define UFM01_COMMAND_A2_START_BYTE                     (14)        // First byte of the A2 configuration in the configuration command
#define UFM01_COMMAND_A3_START_BYTE                     (16)        // First byte of the A3 configuration in the configuration command
#define UFM01_COMMAND_A4_START_BYTE                     (20)        // First byte of the A4 configuration in the configuration command
#define UFM01_COMMAND_CHECKSUM_START_BYTE               (23)        // Checksum byte in the configuration command

#define UFM01_COMMAND_A0_LENGTH                         (1)         // Length in bytes of the A0 parameter
#define UFM01_COMMAND_A1_LENGTH                         (6)         // Length in bytes of the A1 parameter
#define UFM01_COMMAND_A2_LENGTH                         (1)         // Length in bytes of the A2 parameter
#define UFM01_COMMAND_A3_LENGTH                         (3)         // Length in bytes of the A3 parameter
#define UFM01_COMMAND_A4_LENGTH                         (3)         // Length in bytes of the A4 parameter

#define UFM01_COMMAND_CLEAR_ACCUMULATED_FLOW            {0xFE, 0xFE, 0x11, 0x5A, 0xFD, 0x57, 0x16}
#define UFM01_COMMAND_SET_PASSIVE_MODE                  {0xFE, 0xFE, 0x11, 0x5C, 0x01, 0x5D, 0x16}
#define UFM01_COMMAND_SET_ACTIVE_MODE                   {0xFE, 0xFE, 0x11, 0x5C, 0x00, 0x5C, 0x16}
#define UFM01_COMMAND_READ_SENSOR_DATA_NO_ID            {0xFE, 0xFE, 0x11, 0x5B, 0x0F, 0x6A, 0x16}
#define UFM01_COMMAND_READ_SENSOR_DATA_WITH_ID          {0xFE, 0xFE, 0x11, 0x5B, 0xCB, 0x26, 0x16}
#define UFM01_COMMAND_RESET_MODULE                      {0xFE, 0xFE, 0x11, 0x5D, 0xFD, 0x5A, 0x16}
#define UFM01_COMMAND_SET_CONFIGURATION                 {0xFE, 0xFE, 0x11, 0x60, 0xA0, 0x00, 0xA1, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0xA2, 0x04, 0xA3, 0x15, 0x00, 0x00, 0xA4, 0x00, 0x00, 0x00, 0xFF, 0x16}
#define UFM01_COMMAND_GET_SOFTWARE_VERSION              {0xFE, 0xFE, 0x11, 0x5E, 0x62, 0xC0, 0x16}

//// Command response addresses ( answer protocol structure )
typedef size_t Ufm01_CommandResponse;
#define UFM01_COMMAND_RESPONSE_ACKNOWLEDGE                                  (0xE5)  // Response of the UFM01 module to commands where no data must be sent

#define UFM01_COMMAND_RESPONSE_ACKNOWLEDGE_LENGTH                           (1)     // Command response length to a command that requests no data
#define UFM01_COMMAND_RESPONSE_ACTIVE_MODE_LENGTH                           (32)    // Command response length of the data that gets printed periodically in active mode
#define UFM01_COMMAND_RESPONSE_PASSIVE_MODE_WITH_ID_LENGTH                  (39)    // Command response length for the "ReadDataWithId" command
#define UFM01_COMMAND_RESPONSE_PASSIVE_MODE_NO_ID_LENGTH                    (23)    // Command response length for the "ReadDataNoId" command
#define UFM01_COMMAND_RESPONSE_SET_CONFIGURATION_LENGTH                     (22)    // Command response length for the "SetFrequencyStartFlow" command
#define UFM01_COMMAND_RESPONSE_GET_SOFTWARE_VERSION_LENGTH                  (7)     // Command response length for the "GetSoftwareVersion" command

#define UFM01_COMMAND_RESPONSE_START_BYTE_ADDRESS_1                         (0)
#define UFM01_COMMAND_RESPONSE_START_BYTE_ADDRESS_2                         (1)
#define UFM01_COMMAND_RESPONSE_ACKNOWLEDGE_BYTE_ADDRESS                     (0)

#define UFM01_COMMAND_RESPONSE_ACTIVE_DEVICE_ID_ADDRESS                     (2)     // Byte address of the start of the ID in the active mode
#define UFM01_COMMAND_RESPONSE_ACTIVE_ACC_FLOW_FLAG_ADDRESS                 (8)     // Byte address of the accumulated flow data flag in the active mode
#define UFM01_COMMAND_RESPONSE_ACTIVE_ACC_FLOW_ADDRESS                      (9)     // Byte address of the start of the accumulated flow data in the active mode
#define UFM01_COMMAND_RESPONSE_ACTIVE_INSTANT_FLOW_FLAG_ADDRESS             (15)    // Byte address of the instant flow data flag in the active mode
#define UFM01_COMMAND_RESPONSE_ACTIVE_INSTANT_FLOW_ADDRESS                  (16)    // Byte address of the start of the instant flow data in the active mode
#define UFM01_COMMAND_RESPONSE_ACTIVE_INSTANT_FLOW_SIGN_ADDRESS             (20)    // Byte address of the sign of the instant flow data flag in the active mode
#define UFM01_COMMAND_RESPONSE_ACTIVE_TEMP_FLAG_ADDRESS                     (24)    // Byte address of the temperature data flag in the active mode
#define UFM01_COMMAND_RESPONSE_ACTIVE_TEMP_ADDRESS                          (25)    // Byte address of the start of the temperature data in the active mode
#define UFM01_COMMAND_RESPONSE_ACTIVE_ST1_ADDRESS                           (28)    // Byte address of the status byte 1 in the active mode
#define UFM01_COMMAND_RESPONSE_ACTIVE_ST2_ADDRESS                           (29)    // Byte address of the status byte 2 in the active mode
#define UFM01_COMMAND_RESPONSE_ACTIVE_CHECKSUM_ADDRESS                      (30)    // Byte address of the checksum byte in the active mode
#define UFM01_COMMAND_RESPONSE_ACTIVE_STOP_ADDRESS                          (31)    // Byte address of the stop byte in the active mode

#define UFM01_COMMAND_RESPONSE_PASSIVE_WITH_ID_DEVICE_ID_ADDRESS            (2)     // Byte address of the start of the ID in the passive mode with ID
#define UFM01_COMMAND_RESPONSE_PASSIVE_WITH_ID_ACC_FLOW_FLAG_ADDRESS        (8)     // Byte address of the accumulated flow data flag in the passive mode with ID
#define UFM01_COMMAND_RESPONSE_PASSIVE_WITH_ID_ACC_FLOW_ADDRESS             (9)     // Byte address of the start of the accumulated flow data in the passive mode with ID
#define UFM01_COMMAND_RESPONSE_PASSIVE_WITH_ID_INSTANT_FLOW_FLAG_ADDRESS    (22)    // Byte address of the instant flow data flag in the passive mode with ID
#define UFM01_COMMAND_RESPONSE_PASSIVE_WITH_ID_INSTANT_FLOW_ADDRESS         (23)    // Byte address of the start of the instant flow data in the passive mode with ID
#define UFM01_COMMAND_RESPONSE_PASSIVE_WITH_ID_INSTANT_SIGN_ADDRESS         (27)    // Byte address of the sign of the instant flow data in the passive mode with ID
#define UFM01_COMMAND_RESPONSE_PASSIVE_WITH_ID_TEMP_FLAG_ADDRESS            (31)    // Byte address of the temperature flag in the passive mode with ID
#define UFM01_COMMAND_RESPONSE_PASSIVE_WITH_ID_TEMP_ADDRESS                 (32)    // Byte address of the start of the temperature data in the passive mode with ID
#define UFM01_COMMAND_RESPONSE_PASSIVE_WITH_ID_ST1_ADDRESS                  (35)    // Byte address of the status Byte 1 in the passive mode with ID
#define UFM01_COMMAND_RESPONSE_PASSIVE_WITH_ID_ST2_ADDRESS                  (36)    // Byte address of the status Byte 2 in the passive mode with ID
#define UFM01_COMMAND_RESPONSE_PASSIVE_WITH_ID_CHECKSUM_ADDRESS             (37)    // Byte address of the checksum byte in the passive mode with ID
#define UFM01_COMMAND_RESPONSE_PASSIVE_WITH_ID_STOP_ADDRESS                 (38)    // Byte address of the stop byte in the passive mode with ID

#define UFM01_COMMAND_RESPONSE_PASSIVE_NO_ID_ACC_FLOW_FLAG_ADDRESS          (2)     // Byte address of the accumulated flow data flag in the passive mode without ID
#define UFM01_COMMAND_RESPONSE_PASSIVE_NO_ID_ACC_FLOW_ADDRESS               (3)     // Byte address of the start of the accumulated flow data in the passive mode without ID
#define UFM01_COMMAND_RESPONSE_PASSIVE_NO_ID_INSTANT_FLOW_FLAG_ADDRESS      (9)     // Byte address of the intant flow data flag in the passive mode without ID
#define UFM01_COMMAND_RESPONSE_PASSIVE_NO_ID_INSTANT_FLOW_ADDRESS           (10)    // Byte address of the start of the instant flow data in the passive mode without ID
#define UFM01_COMMAND_RESPONSE_PASSIVE_NO_ID_INSTANT_FLOW_SIGN_ADDRESS      (14)    // Byte address of the of the sign of the intant flow data flag in the passive mode without ID
#define UFM01_COMMAND_RESPONSE_PASSIVE_NO_ID_TEMP_FLAG_ADDRESS              (15)    // Byte address of the temperature data flag in the passive mode without ID
#define UFM01_COMMAND_RESPONSE_PASSIVE_NO_ID_TEMP_ADDRESS                   (16)    // Byte address of the start of the temperature data in the passive mode without ID
#define UFM01_COMMAND_RESPONSE_PASSIVE_NO_ID_ST1_ADDRESS                    (19)    // Byte address of the status Byte 1 in the passive mode without ID
#define UFM01_COMMAND_RESPONSE_PASSIVE_NO_ID_ST2_ADDRESS                    (20)    // Byte address of the status Byte 2 in the passive mode without ID
#define UFM01_COMMAND_RESPONSE_PASSIVE_NO_ID_CHECKSUM_ADDRESS               (21)    // Byte address of the checksum byte in the passive mode without ID
#define UFM01_COMMAND_RESPONSE_PASSIVE_NO_ID_STOP_ADDRESS                   (22)    // Byte address of the stop byte in the passive mode without ID

#define UFM01_COMMAND_RESPONSE_CONFIGURE_A0_FLAG_BYTE_ADDRESS               (1)     // Address of the byte of flag of the A0 configuration in the reply of the configuration command
#define UFM01_COMMAND_RESPONSE_CONFIGURE_A0_BYTE_ADDRESS                    (2)     // Address of the first byte of the A0 configuration in the reply of the configuration command
#define UFM01_COMMAND_RESPONSE_CONFIGURE_A1_FLAG_BYTE_ADDRESS               (3)     // Address of the byte of flag of the A1 configuration in the reply of the configuration command
#define UFM01_COMMAND_RESPONSE_CONFIGURE_A1_BYTE_ADDRESS                    (4)     // Address of the first byte of the A1 configuration in the reply of the configuration command
#define UFM01_COMMAND_RESPONSE_CONFIGURE_A2_FLAG_BYTE_ADDRESS               (10)    // Address of the byte of flag of the A2 configuration in the reply of the configuration command
#define UFM01_COMMAND_RESPONSE_CONFIGURE_A2_BYTE_ADDRESS                    (11)    // Address of the first byte of the A2 configuration in the reply of the configuration command
#define UFM01_COMMAND_RESPONSE_CONFIGURE_A3_FLAG_BYTE_ADDRESS               (12)    // Address of the byte of flag of the A3 configuration in the reply of the configuration command
#define UFM01_COMMAND_RESPONSE_CONFIGURE_A3_BYTE_ADDRESS                    (13)    // Address of the first byte of the A3 configuration in the reply of the configuration command
#define UFM01_COMMAND_RESPONSE_CONFIGURE_A4_FLAG_BYTE_ADDRESS               (16)    // Address of the byte of flag of the A4 configuration in the reply of the configuration command
#define UFM01_COMMAND_RESPONSE_CONFIGURE_A4_BYTE_ADDRESS                    (17)    // Address of the first byte of the A4 configuration in the reply of the configuration command
#define UFM01_COMMAND_RESPONSE_CONFIGURE_CHECKSUM_BYTE_ADDRESS              (20)    // Address of the checksum byte in the reply of the configuration command
#define UFM01_COMMAND_RESPONSE_CONFIGURE_STOP_BYTE_ADDRESS                  (21)    // Address of the stop byte in the reply of the configuration command

#define UFM01_COMMAND_RESPONSE_SOFTWARE_VERSION_START_BYTE_ADDRESS          (1)     // Address of the first byte of the software version response
#define UFM01_COMMAND_RESPONSE_SOFTWARE_CHECKSUM_BYTE_ADDRESS               (5)     // Address of the checksum byte of the software version response
#define UFM01_COMMAND_RESPONSE_SOFTWARE_STOP_BYTE_ADDRESS                   (6)     // Address of the stop byte of the software version response

#define UFM01_COMMAND_RESPONSE_DATA_OUT_START_BYTE_1                        (0x3C)  // First byte of the data output response
#define UFM01_COMMAND_RESPONSE_DATA_OUT_ACTIVE_START_BYTE_2                 (0x32)  // Second byte of the data output response for the active mode
#define UFM01_COMMAND_RESPONSE_PASSIVE_WITH_ID_DATA_OUT_START_BYTE_2        (0x96)  // Second byte of the data output response for the passive mode with ID
#define UFM01_COMMAND_RESPONSE_PASSIVE_NO_ID_DATA_OUT_START_BYTE_2          (0x64)  // Second byte of the data output response for the passive mode without ID
#define UFM01_COMMAND_RESPONSE_FLAG_A0                                      (0xA0)  // Flag for parameter A0
#define UFM01_COMMAND_RESPONSE_FLAG_A1                                      (0xA1)  // Flag for parameter A1
#define UFM01_COMMAND_RESPONSE_FLAG_A2                                      (0xA2)  // Flag for parameter A2
#define UFM01_COMMAND_RESPONSE_FLAG_A3                                      (0xA3)  // Flag for parameter A3
#define UFM01_COMMAND_RESPONSE_FLAG_A4                                      (0xA4)  // Flag for parameter A4
#define UFM01_COMMAND_RESPONSE_STOP_BYTE                                    (0x16)  // Stop byte of the data output responsenses

//// IO Protocol
typedef uint8_t Ufm01_Protocol;
#define UFM01_PROTOCOL_UART                                 (0)
#define UFM01_PROTOCOL_ONE_WIRE                             (1)

//// SystemTiming in ms
#define UFM01_SYSTEM_TIMING_STANDARD_MEASURE                (1000)

//// Acquisition frequencies in Hz
typedef uint8_t Ufm01_Frequency;
#define UFM01_MEASUREMENT_FREQUENCY_1HZ                     (0x01)
#define UFM01_MEASUREMENT_FREQUENCY_2HZ                     (0x02)
#define UFM01_MEASUREMENT_FREQUENCY_4HZ                     (0x04)
#define UFM01_MEASUREMENT_FREQUENCY_8HZ                     (0x08)

//// Values of the accumulated flow flag
#define UFM01_ACCUMULATED_FLOW_LITERS                       (0x0A)
#define UFM01_ACCUMULATED_FLOW_M3                           (0x1A)

//// Bitmasks
#define UFM01_INSTANT_FLOW_SIGN_MASK                        (0x80)

//// BufferInfo defines internally used buffer and data indices and sizes (in bytes)
#define UFM01_BUFFER_SOFTWARE_VERSION_LENGTH                (4)
#define UFM01_BUFFER_ID_LENGTH                              (5)
#define UFM01_BUFFER_ID_STRING_LENGTH                       (10)
#define UFM01_BUFFER_MANUFACTURING_DATE_LENGTH              (3)
#define UFM01_BUFFER_MANUFACTURING_DATE_ADDRESS             (2)
#define UFM01_BUFFER_SERIAL_NUMBER_LENGTH                   (2)
#define UFM01_BUFFER_SERIAL_NUMBER_ADDRESS                  (0)
#define UFM01_BUFFER_ACCUMULATED_FLOW_LENGTH                (6)
#define UFM01_BUFFER_INSTANT_FLOW_LENGTH                    (5)
#define UFM01_BUFFER_INSTANT_FLOW_VALUE_LENGTH              (4)
#define UFM01_BUFFER_INSTANT_FLOW_SIGN_ADDRESS              (4)
#define UFM01_BUFFER_TEMPERATURE_LENGTH                     (3)
#define UFM01_BUFFER_STATUS_LENGTH                          (2)

#define UFM01_BUFFER_ONEWIRE_DATA_LENGTH                    (12)
#define UFM01_BUFFER_INSTANT_FLOW_ONEWIRE_ADDRESS           (0)
#define UFM01_BUFFER_INSTANT_FLOW_ONEWIRE_CRC_ADDRESS       (3)
#define UFM01_BUFFER_TEMPERATURE_ONEWIRE_ADDRESS            (4)
#define UFM01_BUFFER_TEMPERATURE_ONEWIRE_CRC_ADDRESS        (7)
#define UFM01_BUFFER_ACCUMULATED_FLOW_ONEWIRE_ADDRESS       (8)
#define UFM01_BUFFER_ACCUMULATED_FLOW_ONEWIRE_CRC_ADDRESS   (11)

#define UFM01_BUFFER_MEASUREMENT_ONEWIRE_LENGTH             (3)

#ifndef SCIOSENSE_RESULT_CODES
#define SCIOSENSE_RESULT_CODES
typedef int8_t Result;
#define RESULT_NOT_ALLOWED                                  (4)     // The requested command is not allowed.
#define RESULT_CHECKSUM_ERROR                               (3)     // The value was read, but the checksum over the payload (valid and data) does not match.
#define RESULT_INVALID                                      (2)     // The value was read, but the data is invalid.
#define RESULT_IO_ERROR                                     (1)     // There was an IO communication error, read/write the stream failed.
#define RESULT_OK                                           (0)     // All OK; The value was read, the checksum matches, and data is valid.
#endif

#endif // SCIOSENSE_UFM01_DEFINES_C_H