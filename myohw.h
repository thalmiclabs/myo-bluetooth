#pragma once

#include <stdint.h>

#if defined(_MSC_VER) || defined(_WIN32)
# pragma pack(push, 1)
# define MYOHW_PACKED
#else
# define MYOHW_PACKED __attribute__ ((__packed__))
#endif // _MSC_VER

/// Check, at compile time, that the size of \a type is \a size bytes.
#ifdef __cplusplus
# define MYOHW_STATIC_ASSERT_SIZED(type, size) \
         static_assert(sizeof(type) == size, # type " does not have size " # size)
#else
# define MYOHW_STATIC_ASSERT_SIZED(type, size)
#endif

#define MYO_SERVICE_INFO_UUID { \
    0x42, 0x48, 0x12, 0x4a,     \
    0x7f, 0x2c, 0x48, 0x47,     \
    0xb9, 0xde, 0x04, 0xa9,     \
    0x01, 0x00, 0x06, 0xd5      \
}
static const uint8_t kMyoServiceInfoUuid[] = MYO_SERVICE_INFO_UUID;

/// The number of EMG sensors that a Myo has.
static const int myohw_num_emg_sensors = 8;

/// @defgroup myo_hardware Myo Hardware Data Structures
/// These types and enumerations describe the format of data sent to and from a Myo device using Bluetooth Low Energy.
/// All values are big-endian.
/// @{

/// The following enum lists the 16bit short UUIDs of Myo services and characteristics. To construct a full 128bit
/// UUID, replace the two 0x00 hex bytes of MYO_SERVICE_BASE_UUID with a short UUID from myohw_standard_services. 
/// The byte sequence of MYO_SERVICE_BASE_UUID is in network order. Keep this in mind when doing the replacement.
/// For example, the full service UUID for GCControlService would be d5060001-a904-deb9-4748-2c7f4a124842. 
#define MYO_SERVICE_BASE_UUID { \
    0x42, 0x48, 0x12, 0x4a,     \
    0x7f, 0x2c, 0x48, 0x47,     \
    0xb9, 0xde, 0x04, 0xa9,     \
    0x00, 0x00, 0x06, 0xd5      \
}

typedef enum {
    ControlService                = 0x0001, ///< Myo info service
    MyoInfoCharacteristic         = 0x0101, ///< Serial number for this Myo and various parameters which
                                            ///< are specific to this firmware. Read-only attribute. 
                                            ///< See myohw_fw_info_t.
    FirmwareVersionCharacteristic = 0x0201, ///< Current firmware version. Read-only characteristic.
                                            ///< See myohw_fw_version_t.
    CommandCharacteristic         = 0x0401, ///< Issue commands to the Myo. Write-only characteristic.
                                            ///< See myohw_command_t.

    ImuDataService                = 0x0002, ///< IMU service
    IMUDataCharacteristic         = 0x0402, ///< See myohw_imu_data_t. Notify-only characteristic.
    MotionEventCharacteristic     = 0x0a02, ///< Motion event data. Indicate-only characteristic.

    ClassifierService             = 0x0003, ///< Classifier event service.
    ClassifierEventCharacteristic = 0x0103, ///< Classifier event data. Indicate-only characteristic. See myohw_pose_t.

    EmgDataService                = 0x0005, ///< Raw EMG data service.
    EmgData0Characteristic        = 0x0105, ///< Raw EMG data. Notify-only characteristic.
    EmgData1Characteristic        = 0x0205, ///< Raw EMG data. Notify-only characteristic.
    EmgData2Characteristic        = 0x0305, ///< Raw EMG data. Notify-only characteristic.
    EmgData3Characteristic        = 0x0405, ///< Raw EMG data. Notify-only characteristic.
} myohw_services;

// Standard Bluetooth device services.
typedef enum{
    BatteryService                = 0x180f, ///< Battery service
    BatteryLevelCharacteristic    = 0x2a19, ///< Current battery level information. Read/notify characteristic.

    DeviceName                    = 0x2a00, ///< Device name data. Read/write characteristic.
} myohw_standard_services;

/// Supported poses.
typedef enum {
    myohw_pose_rest           = 0x0000,
    myohw_pose_fist           = 0x0001,
    myohw_pose_wave_in        = 0x0002,
    myohw_pose_wave_out       = 0x0003,
    myohw_pose_fingers_spread = 0x0004,
    myohw_pose_double_tap     = 0x0005,
    myohw_pose_unknown        = 0xffff
} myohw_pose_t;

/// Various parameters that may affect the behaviour of this Myo armband.
/// The Myo library reads this attribute when a connection is established.
/// Value layout for the myohw_att_handle_fw_info attribute.
typedef struct MYOHW_PACKED
{
    uint8_t serial_number[6];        ///< Unique serial number of this Myo.
    uint16_t unlock_pose;            ///< Pose that should be interpreted as the unlock pose. See myohw_pose_t.
    uint8_t active_classifier_type;  ///< Whether Myo is currently using a built-in or a custom classifier.
                                     ///< See myohw_classifier_model_type_t.
    uint8_t active_classifier_index; ///< Index of the classifier that is currently active.
    uint8_t has_custom_classifier;   ///< Whether Myo contains a valid custom classifier. 1 if it does, otherwise 0.
    uint8_t stream_indicating;       ///< Set if the Myo uses BLE indicates to stream data, for reliable capture.
    uint8_t sku;                     ///< SKU value of the device. See myohw_sku_t
    uint8_t reserved[7];             ///< Reserved for future use; populated with zeros.
} myohw_fw_info_t;
MYOHW_STATIC_ASSERT_SIZED(myohw_fw_info_t, 20);

// Known Myo SKUs
typedef enum {
    myohw_sku_unknown   = 0, ///< Unknown SKU (default value for old firmwares)
    myohw_sku_black_myo = 1, ///< Black Myo
    myohw_sku_white_myo = 2  ///< White Myo
} myohw_sku_t;

/// Known Myo hardware revisions.
typedef enum {
    myohw_hardware_rev_unknown = 0, ///< Unknown hardware revision.
    myohw_hardware_rev_revc    = 1, ///< Myo Alpha (REV-C) hardware.
    myohw_hardware_rev_revd    = 2, ///< Myo (REV-D) hardware.
    myohw_num_hardware_revs         ///< Number of hardware revisions known; not a valid hardware revision.
} myohw_hardware_rev_t;

/// Version information for the Myo firmware.
/// Value layout for the myohw_att_handle_fw_version attribute.
/// Minor version is incremented for changes in this interface.
/// Patch version is incremented for firmware changes that do not introduce changes in this interface.
typedef struct MYOHW_PACKED
{
    uint16_t major;
    uint16_t minor;
    uint16_t patch;
    uint16_t hardware_rev; ///< Myo hardware revision. See myohw_hardware_rev_t.
} myohw_fw_version_t;
MYOHW_STATIC_ASSERT_SIZED(myohw_fw_version_t, 8);

#define MYOHW_FIRMWARE_VERSION_MAJOR 1
#define MYOHW_FIRMWARE_VERSION_MINOR 2
static const uint16_t myohw_firmware_version_major = MYOHW_FIRMWARE_VERSION_MAJOR;
static const uint16_t myohw_firmware_version_minor = MYOHW_FIRMWARE_VERSION_MINOR;

/// @defgroup myohw_control_commands Control Commands
/// @{

/// Kinds of commands.
typedef enum {
    myohw_command_set_mode               = 0x01, ///< Set EMG and IMU modes. See myohw_command_set_mode_t.
    myohw_command_set_mode_custom        = 0x02, ///< Set EMG and IMU modes with custom parameters.
                                                 ///< See myohw_command_set_mode_custom_t.
    myohw_command_vibrate                = 0x03, ///< Vibrate. See myohw_command_vibrate_t.
    myohw_command_deep_sleep             = 0x04, ///< Put Myo into deep sleep. See myohw_command_deep_sleep_t.
    myohw_command_vibrate2               = 0x07, ///< Extended vibrate. See myohw_command_vibrate2_t.
    myohw_command_set_sleep_mode         = 0x09, ///< Set sleep mode. See myohw_command_set_sleep_mode_t.
    myohw_command_unlock                 = 0x0a, ///< Unlock Myo. See myohw_command_unlock_t.
    myohw_command_user_action            = 0x0b, ///< Notify user that an action has been recognized / confirmed.
                                                 ///< See myohw_command_user_action_t.
} myohw_command_t;

/// Header that every command begins with.
typedef struct MYOHW_PACKED {
    uint8_t command;        ///< Command to send. See myohw_command_t.
    uint8_t payload_size;   ///< Number of bytes in payload.
} myohw_command_header_t;
MYOHW_STATIC_ASSERT_SIZED(myohw_command_header_t, 2);

/// EMG modes.
typedef enum {
    myohw_emg_mode_none         = 0x00, ///< Do not send EMG data.
    myohw_emg_mode_send_emg     = 0x02, ///< Send filtered EMG data.
    myohw_emg_mode_send_emg_raw = 0x03, ///< Send raw (unfiltered) EMG data.
} myohw_emg_mode_t;

/// IMU modes.
typedef enum {
    myohw_imu_mode_none        = 0x00, ///< Do not send IMU data or events.
    myohw_imu_mode_send_data   = 0x01, ///< Send IMU data streams (accelerometer, gyroscope, and orientation).
    myohw_imu_mode_send_events = 0x02, ///< Send motion events detected by the IMU (e.g. taps).
    myohw_imu_mode_send_all    = 0x03, ///< Send both IMU data streams and motion events.
    myohw_imu_mode_send_raw    = 0x04, ///< Send raw IMU data streams.
} myohw_imu_mode_t;

/// Classifier modes.
typedef enum {
    myohw_classifier_mode_disabled = 0x00, ///< Disable and reset the internal state of the onboard classifier.
    myohw_classifier_mode_enabled  = 0x01, ///< Send classifier events (poses and arm events).
} myohw_classifier_mode_t;

/// Command to set EMG and IMU modes.
typedef struct MYOHW_PACKED {
    myohw_command_header_t header; ///< command == myohw_command_set_mode. payload_size = 3.
    uint8_t emg_mode;              ///< EMG sensor mode. See myohw_emg_mode_t.
    uint8_t imu_mode;              ///< IMU mode. See myohw_imu_mode_t.
    uint8_t classifier_mode;       ///< Classifier mode. See myohw_classifier_mode_t.
} myohw_command_set_mode_t;
MYOHW_STATIC_ASSERT_SIZED(myohw_command_set_mode_t, 5);

/// Kinds of vibrations.
typedef enum {
    myohw_vibration_none   = 0x00, ///< Do not vibrate.
    myohw_vibration_short  = 0x01, ///< Vibrate for a short amount of time.
    myohw_vibration_medium = 0x02, ///< Vibrate for a medium amount of time.
    myohw_vibration_long   = 0x03, ///< Vibrate for a long amount of time.
} myohw_vibration_type_t;

/// Vibration command.
typedef struct MYOHW_PACKED {
    myohw_command_header_t header; ///< command == myohw_command_vibrate. payload_size == 1.
    uint8_t type;                  ///< See myohw_vibration_type_t.
} myohw_command_vibrate_t;
MYOHW_STATIC_ASSERT_SIZED(myohw_command_vibrate_t, 3);

/// Deep sleep command.
typedef struct MYOHW_PACKED {
    myohw_command_header_t header; ///< command == myohw_command_deep_sleep. payload_size == 0.
} myohw_command_deep_sleep_t;
MYOHW_STATIC_ASSERT_SIZED(myohw_command_deep_sleep_t, 2);

/// Extended vibration command.
#define MYOHW_COMMAND_VIBRATE2_STEPS 6
typedef struct MYOHW_PACKED {
    myohw_command_header_t header; ///< command == myohw_command_vibrate2. payload_size == 18.
    struct MYOHW_PACKED {
        uint16_t duration;         ///< duration (in ms) of the vibration
        uint8_t strength;          ///< strength of vibration (0 - motor off, 255 - full speed)
    } steps[MYOHW_COMMAND_VIBRATE2_STEPS];
} myohw_command_vibrate2_t;
MYOHW_STATIC_ASSERT_SIZED(myohw_command_vibrate2_t, 20);

/// Sleep modes.
typedef enum {
    myohw_sleep_mode_normal      = 0, ///< Normal sleep mode; Myo will sleep after a period of inactivity.
    myohw_sleep_mode_never_sleep = 1, ///< Never go to sleep.
} myohw_sleep_mode_t;

/// Set sleep mode command.
typedef struct MYOHW_PACKED {
    myohw_command_header_t header; ///< command == myohw_command_set_sleep_mode. payload_size == 1.
    uint8_t sleep_mode;            ///< Sleep mode. See myohw_sleep_mode_t.
} myohw_command_set_sleep_mode_t;
MYOHW_STATIC_ASSERT_SIZED(myohw_command_set_sleep_mode_t, 3);

/// Unlock types.
typedef enum {
    myohw_unlock_lock  = 0x00, ///< Re-lock immediately.
    myohw_unlock_timed = 0x01, ///< Unlock now and re-lock after a fixed timeout.
    myohw_unlock_hold  = 0x02, ///< Unlock now and remain unlocked until a lock command is received.
} myohw_unlock_type_t;

/// Unlock Myo command.
/// Can also be used to force Myo to re-lock.
typedef struct MYOHW_PACKED {
    myohw_command_header_t header; ///< command == myohw_command_unlock. payload_size == 1.
    uint8_t type;                  ///< Unlock type. See myohw_unlock_type_t.
} myohw_command_unlock_t;
MYOHW_STATIC_ASSERT_SIZED(myohw_command_unlock_t, 3);

/// User action types.
typedef enum {
    myohw_user_action_single = 0, ///< User did a single, discrete action, such as pausing a video.
} myohw_user_action_type_t;

/// User action command.
typedef struct MYOHW_PACKED {
    myohw_command_header_t header; ///< command == myohw_command_user_action. payload_size == 1.
    uint8_t type;                  ///< Type of user action that occurred. See myohw_user_action_type_t.
} myohw_command_user_action_t;
MYOHW_STATIC_ASSERT_SIZED(myohw_command_user_action_t, 3);

/// Classifier model types
typedef enum {
    myohw_classifier_model_builtin = 0, ///< Model built into the classifier package.
    myohw_classifier_model_custom  = 1  ///< Model based on personalized user data.
} myohw_classifier_model_type_t;

/// @}

/// Integrated motion data.
typedef struct MYOHW_PACKED {
    /// Orientation data, represented as a unit quaternion. Values are multiplied by MYOHW_ORIENTATION_SCALE.
    struct MYOHW_PACKED {
        int16_t w, x, y, z;
    } orientation;

    int16_t accelerometer[3]; ///< Accelerometer data. In units of g. Range of + -16.
                              ///< Values are multiplied by MYOHW_ACCELEROMETER_SCALE.
    int16_t gyroscope[3];     ///< Gyroscope data. In units of deg/s. Range of + -2000.
                              ///< Values are multiplied by MYOHW_GYROSCOPE_SCALE.
} myohw_imu_data_t;
MYOHW_STATIC_ASSERT_SIZED(myohw_imu_data_t, 20);

/// Default IMU sample rate in Hz.
#define MYOHW_DEFAULT_IMU_SAMPLE_RATE 50

/// Scale values for unpacking IMU data
#define MYOHW_ORIENTATION_SCALE   16384.0f ///< See myohw_imu_data_t::orientation
#define MYOHW_ACCELEROMETER_SCALE 2048.0f  ///< See myohw_imu_data_t::accelerometer
#define MYOHW_GYROSCOPE_SCALE     16.0f    ///< See myohw_imu_data_t::gyroscope

/// Types of motion events.
typedef enum {
    myohw_motion_event_tap = 0x00,
} myohw_motion_event_type_t;

/// Motion event data received in a myohw_att_handle_motion_event attribute.
typedef struct MYOHW_PACKED {
    uint8_t type; /// Type type of motion event that occurred. See myohw_motion_event_type_t.

    /// Event-specific data.
    union MYOHW_PACKED {
        /// For myohw_motion_event_tap events.
        struct MYOHW_PACKED {
            uint8_t tap_direction;
            uint8_t tap_count;
        };
    };
} myohw_motion_event_t;
MYOHW_STATIC_ASSERT_SIZED(myohw_motion_event_t, 3);

/// Types of classifier events.
typedef enum {
    myohw_classifier_event_arm_synced   = 0x01,
    myohw_classifier_event_arm_unsynced = 0x02,
    myohw_classifier_event_pose         = 0x03,
    myohw_classifier_event_unlocked     = 0x04,
    myohw_classifier_event_locked       = 0x05,
    myohw_classifier_event_sync_failed  = 0x06,
} myohw_classifier_event_type_t;

/// Enumeration identifying a right arm or left arm.
typedef enum {
    myohw_arm_right   = 0x01,
    myohw_arm_left    = 0x02,
    myohw_arm_unknown = 0xff
} myohw_arm_t;

/// Possible directions for Myo's +x axis relative to a user's arm.
typedef enum {
    myohw_x_direction_toward_wrist = 0x01,
    myohw_x_direction_toward_elbow = 0x02,
    myohw_x_direction_unknown      = 0xff
} myohw_x_direction_t;

/// Possible outcomes when the user attempts a sync gesture.
typedef enum {
    myohw_sync_failed_too_hard = 0x01, ///< Sync gesture was performed too hard.
} myohw_sync_result_t;

/// Classifier event data received in a myohw_att_handle_classifier_event attribute.
typedef struct MYOHW_PACKED {
    uint8_t type; ///< See myohw_classifier_event_type_t

    /// Event-specific data
    union MYOHW_PACKED {
        /// For myohw_classifier_event_arm_synced events.
        struct MYOHW_PACKED {
            uint8_t arm; ///< See myohw_arm_t
            uint8_t x_direction; ///< See myohw_x_direction_t
        };

        /// For myohw_classifier_event_pose events.
        uint16_t pose; ///< See myohw_pose_t

        /// For myohw_classifier_event_sync_failed events.
        uint8_t sync_result; ///< See myohw_sync_result_t.
    };
} myohw_classifier_event_t;
MYOHW_STATIC_ASSERT_SIZED(myohw_classifier_event_t, 3);

/// The rate that EMG events are streamed over Bluetooth.
#define MYOHW_EMG_DEFAULT_STREAMING_RATE 200

/// Raw EMG data received in a myohw_att_handle_emg_data_# attribute.
/// Value layout for myohw_att_handle_emg_data_#.
typedef struct MYOHW_PACKED {
    int8_t sample1[8];       ///< 1st sample of EMG data.
    int8_t sample2[8];       ///< 2nd sample of EMG data.
} myohw_emg_data_t;
MYOHW_STATIC_ASSERT_SIZED(myohw_emg_data_t, 16);

/// @}

#if defined(_MSC_VER) || defined(_WIN32)
# pragma pack(pop)
#endif // _MSC_VER
