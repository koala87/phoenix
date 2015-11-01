/**************************************************************************************
* Copyright (c) 2011 Beijing Senselock Software Technology Co.,Ltd.
* All rights reserved.
*
* filename: elite5.h
*
* brief: Library interface declaration, return value and some constant definition.
* 
***************************************************************************************/
#ifndef _ELITE5_H
#define _ELITE5_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifdef _MSC_VER
#pragma pack(push)
#pragma pack(1)
#endif // _MSC_VER

#ifndef EVAPI
#if defined WIN32 || defined _WIN32 || defined _WIN64
#define EVAPI __stdcall
#else
#define EVAPI
#endif
#endif

/*******************************************************************************
 generic error code definition
******************************************************************************/
#define EV_ERROR_SUCCESS                                    0x00000000                  // Success.
#define EV_ERROR_INVALID_HANDLE                             0x00000001                  // Handle may be NULL.
#define EV_ERROR_INVALID_PARAMETER                          0x00000002                  // Invalid parameter.
#define EV_ERROR_NOT_ENOUGH_MEMORY                          0x00000003                  // Not enough storage is available to process this command.
#define EV_ERROR_NO_DEVICE                                  0x00000004                  // No device or no specific device on the pc.
#define EV_ERROR_TIMEOUT                                    0x00000005                  // Time out.
#define EV_ERROR_UNSUPPORTED_FLAG                           0x00000006                  // A unsupported flag was given to the APIs.
#define EV_ERROR_INSUFFICIENT_BUFFER                        0x00000007                  // Buffer is too small to hold the data.
#define EV_ERROR_EXCHG_MEMORY_NOT_FOUND                     0x00000008                  // Specific share memory is not found.
#define EV_ERROR_SYSTEM_FILE_NOT_FOUND                      0x00000009                  // File in system not found.
#define EV_ERROR_SYSTEM_FILE_INVALID_ACCESS                 0x0000000a                  // Can not access system file.
#define EV_ERROR_FILE_EXISTS                                0x0000000b                  // Specific file is exist.
#define EV_ERROR_FILE_NOT_FOUND                             0x0000000c                  // Specific file is not found.
#define EV_ERROR_NO_PRIVILEGE                               0x0000000d                  // The operation is required a high level privilege.
#define EV_ERROR_WRONG_PASSWORD                             0x0000000e                  // Password is incorrect.
#define EV_ERROR_PASSWORD_LOCKED                            0x0000000f                  // Password is locked.
#define EV_ERROR_BAD_UPDATE_PKG                             0x00000010                  // Update package data is incorrect.
#define EV_ERROR_COMMUNICATION                              0x00000015                  // Communication error.
#define EV_ERROR_UNSUPPORTED_PASSWORD_TYPE                  0x00000016                  // A unsupported password type was given to APIs.
#define EV_ERROR_WRONG_PASSWORD_LENGTH                      0x00000017                  // Password length is not correct, developer password is 24 bytes, user password is 8 bytes.
#define EV_ERROR_ALREADY_EXCLUSIVELY_LOGIN                  0x00000018                  // The device is already exclusively login, can not login with share mode.
#define EV_ERROR_ALREADY_SHARED_LOGIN                       0x00000019                  // The device is already shared login, can not login with exclusively mode.                 
#define EV_ERROR_ALREADY_TEMP_EXCLUSIVELY_USING             0x0000001a                  // The handle is already temporary using.
#define EV_ERROR_NOT_TEMP_EXCLUSIVELY_USING                 0x0000001b                  // The handle is not temporary using.
#define EV_ERROR_TOO_MUCH_DATA                              0x0000001c                  // The massage reply function has a data length limit.                 
#define EV_ERROR_INSUFFICIENT_DEVICE_SPACE                  0x0000001e                  // The device space is insufficient.
#define EV_ERROR_DEVICE_FILESYSTEM_ERROR                    0x0000001f                  // Device file system error.
#define EV_ERROR_FILE_OUT_RANGE                             0x00000020                  // Device file is out range.
#define EV_ERROR_UNSUPPORTED_FILE_TYPE                      0x00000021                  // an unsupported file type is given to EV APIs.
#define EV_ERROR_FILE_OFFSET_MUST_BE_ZERO                   0x00000022                  // When read or write a key file, offset must be 0.  
#define EV_ERROR_BAD_EXECUTIVE_FILE_FORMAT                  0x00000023                  // the executive file format is incorrect.
#define EV_ERROR_OPEN_TOO_MANY_DEVICE_FILES                 0x00000024                  // Open too many device files.
#define EV_ERROR_INVALID_DEVICE_FILE_HANDLE                 0x00000025                  // device file handle is incorrect.
#define EV_ERROR_FILE_NOT_FINISHED                          0x00000026                  // The file is not finish.
#define EV_ERROR_BAD_FILENAME                               0x00000027                  // Filename is not incorrect.
#define EV_ERROR_BAD_NAME                                   0x00000028                  // The filename, directory name, or volume label syntax is incorrect.
#define EV_ERROR_DEVICE_TIMER                               0x00000029                  // Device timer error.
#define EV_ERROR_NO_EXECUTIVE_FILE_RUNNING                  0x0000002a                  // No process is running in the device. 
#define EV_ERROR_DEVICE_USER_BUFFER_FULL                    0x0000002b                  // Can not send data when device user buffer is full.
#define EV_ERROR_DEVICE_USER_BUFFER_EMPTY                   0x0000002c                  // Can receive data when device user buffer is empty.
#define EV_ERROR_DEVICE_MSG_NOT_REPLIED                     0x0000002d                  // Device need a message reply first.
#define EV_ERROR_DEVICE_DUMMY_MSG                           0x0000002e                  // Device not need a message reply.
#define EV_ERROR_DEVICE_MEMORY_ADDR                         0x0000002f                  // Device memory address error.
#define EV_ERROR_DEVICE_MEMORY_LENGTH                       0x00000030                  // Device memory length error.
#define EV_ERROR_CONTROL_CODE                               0x00000031                  // Give an unsupported control code.
#define EV_ERROR_UNKNOW_COMMAND                             0x00000032                  // Give an unsupported command to device.
#define EV_ERROR_INVALID_COMMAND_PARAMETER                  0x00000033                  // Command parameter error.
#define EV_ERROR_COMMAND_DATA_LENGTH                        0x00000034                  // Command data length error.
#define EV_ERROR_DEVICE_BUFFER_FULL                         0x00000035                  // Device buffer is full.
#define EV_ERROR_EXECUTIVE_FILE_RUNNING                     0x00000036                  // When a process is running in the device, some operation unsupported.
#define EV_ERROR_NO_DEVICE_MESSAGE                          0x00000037                  // No device message.
#define EV_ERROR_LOGIN_COUNT                                0x00000038                  // Device Login count error.
#define EV_ERROR_KEYEXCHANGE_FAILED                         0x00000039                  // Communication key exchange error.
#define EV_ERROR_BAD_COMMUNICATION_KEY                      0x0000003a                  // Communication key is incorrect.
#define EV_ERROR_BAD_DEVICE_TIME                            0x0000003b                  // Device time error.
#define EV_ERROR_BAD_DEVICE_INFOMATION                      0x0000003c                  // Device information error.
#define EV_ERROR_BAD_COMMAND_SEQUENCE                       0x0000003d                  // Command sequence is not right.
#define EV_ERROR_COMMUNICATION_DATA_LENGTH                  0x0000003e                  // Communication data length error.
#define EV_ERROR_ECC                                        0x0000003f                  // Device ECC crypt error.
#define EV_ERROR_BAD_UPDATE_DATA_LENGTH                     0x00000040                  // Update data length is incorrect.
#define EV_ERROR_UPDATE_FAILD                               0x00000041                  // Update failed.
#define EV_ERROR_UPDATE_STATE                               0x00000042                  // Update state is incorrect.
#define EV_ERROR_UPDATE_KEY_NOT_ENABLE                      0x00000043                  // When use remote update, remote update key must set first.
#define EV_ERROR_LOCKED_FOREVER                             0x00000044                  // Device is locked forever.
#define EV_ERROR_LOCKED_TEMPORARY                           0x00000045                  // Device is locked temporary.
#define EV_ERROR_DEVICE_UNLOCKED                            0x00000046                  // Device is not locked.
#define EV_ERROR_DEVICE_FLASH                               0x00000047                  // Device flash error.
#define EV_ERROR_DEVICE_ERROR                               0x00000048                  // Device error.
#define EV_ERROR_TOO_MANY_DEVICE                            0x00000049                  // Device numbers error, should not be larger than 128.
#define EV_ERROR_DEVICE_NOT_ENOUGH_USER_MEMORY              0x0000004a                  // There is no enough memory for user code.
#define EV_ERROR_FAKE_DEVICE                                0x0000004b                  // Device is fake.
#define EV_ERROR_DEVICE_BLK_OUT_RANGE                       0x0000004c                  // Device bulk read or write out range.
#define EV_ERROR_DEVICE_FS_ERR_BAK_ERROR                    0x0000004d                  // Device backup error.
#define EV_ERROR_DEVICE_TMR_ERR_ADJUST_TIME_TIMEOUT         0x0000004e                  // Adjust time is time out.
#define EV_ERROR_DEVICE_EXCH_ERROR                          0x0000004f                  // Exchange memory error.
#define EV_ERROR_DEVICE_AES_ERR                             0x00000050                  // Device AES error.
#define EV_ERROR_DEVICE_HASH_ERR_UNSUPPORTED_ALGO           0x00000051                  // Unsupported hash algo.
#define EV_ERROR_DEVICE_BAD_PRIVATE_KEY                     0x00000052                  // Bad private key.
#define EV_ERROR_DEVICE_BAD_PUBLIC_KEY                      0x00000053                  // Bad public key.
#define EV_ERROR_DEVICE_BAD_SYMMETRIC_KEY                   0x00000054                  // Bad symmetric key.
#define EV_ERROR_DEVICE_BAD_SIGNATURE                       0x00000055                  // Bad signature.
#define EV_ERROR_DEVICE_KEY_ERR_BAD_ALGO                    0x00000056                  // Bad algo.
#define EV_ERROR_DEVICE_LOGO_ERR_LOGO                       0x00000057                  // Bad logo.
#define EV_ERROR_EXEC_PARAM_TOO_LONG                        0x00000058                  // Execute parameter data is too long.
#define EV_ERROR_OPEN_ERROR                                 0x00000059                  // Open device error.
#define EV_ERRPR_LOAD_SYS_MODULE_ERROR                      0x0000005A                  // Load system module error.
#define EV_ERRPR_SYS_MODULE_FUNCTION_ERROR                  0x0000005B                  // System module function address error.
#define EV_ERROR_RSA                                        0x0000005C                  // Device RSA crypt error.
#define EV_ERROR_KEY                                        0x0000005D                  // Crypt Key error.
#define EV_ERROR_DEVICE_EXEC_ERR_UNALIGNED_MEM_ADDR         0x0000005E                  // Unaligned memory address.
#define EV_ERROR_DEVICE_EXEC_ERR_STACK_OVERFLOW             0x0000005F                  // User stack overflow.
#define EV_ERROR_DEVELOPER_ID_MISMATCH                      0x00000060                  // Developer ID not match.
/*******************************************************************************
 type definition
******************************************************************************/
typedef     char                EVINT8;
typedef     unsigned char       EVUINT8;
typedef     short               EVINT16;
typedef     unsigned short      EVUINT16;
typedef     int                 EVINT32;
typedef     unsigned  int       EVUINT32;
typedef     void*               EVHANDLE;

/*******************************************************************************

******************************************************************************/
#define EV_INFINITE                                         0xFFFFFFFF

/*******************************************************************************
 device type definition
******************************************************************************/
#define EV_DEVICE_TYPE_USER                                 0x00000000
#define EV_DEVICE_TYPE_MASTER                               0x00000001
#define EV_DEVICE_TYPE_CLOCK                                0x00000002

/*******************************************************************************
 login mode definition
******************************************************************************/
#define EV_LOGIN_MODE_SHARE                                 0x00000000
#define EV_LOGIN_MODE_EXCLUSIVE                             0x00000001

/*******************************************************************************
 password type definition
******************************************************************************/
#define EV_PASSWORD_TYPE_USER                               0x00000001
#define EV_PASSWORD_TYPE_DEVELOPER                          0x00000002
#define EV_PASSWORD_TYPE_REMOTE_UPDATE                      0x00000003

/*******************************************************************************
 password length definition
******************************************************************************/
#define EV_PASSWORD_LENGTH_USER                             0x00000008
#define EV_PASSWORD_LENGTH_DEVELOPER                        0x00000018
#define EV_PASSWORD_LENGTH_REMOTE_UPDATE                    0x00000018

/*******************************************************************************
 device information item definition for EVGetDeviceInfo and EVSetDeviceInfo
******************************************************************************/
#define EV_DI_DEVICE_TYPE                                   0x00000000 // get
#define EV_DI_FIRMWARE_VERSION                              0x00000001 // get
#define EV_DI_HARDWARE_VERSION                              0x00000002 // get
#define EV_DI_SERIAL_NUMBER                                 0x00000003 // get
#define EV_DI_DEVELOPER_ID                                  0x00000004 // get
#define EV_DI_PRODUCT_ID                                    0x00000005 // get, set
#define EV_DI_MANUFACTURE_DATE                              0x00000007 // get 
#define EV_DI_TOTAL_SPACE                                   0x00000008 // get
#define EV_DI_AVAILABLE_SPACE                               0x00000009 // get
#define EV_DI_COMMUNICATION_PROTOCOL                        0x0000000A // get
#define EV_DI_SLAVE_ADDR                                    0x0000000B // get
#define EV_DI_ANTIDEBUG_STATE                               0x0000000C // get, set
#define EV_DI_USB_SN_STATE                                  0x0000000D // get, set
#define EV_DI_DEBUG_PUNISH_TYPE                             0x0000000F // get, set
#define EV_DI_DEBUG_PUNISH_TIME                             0x00000010 // get, set
#define EV_DI_DEBUG_PUNISH_TYPE_CURRENT                     0x00000011 // get
#define EV_DI_DEBUG_PUNISH_TIME_CURRENT                     0x00000012 // get
#define EV_DI_HEARTBEAT_INTERVAL                            0x00000013 // get, set
#define EV_DI_HEARTBEAT_ERROR_COUNT                         0x00000014 // get, set
#define EV_DI_ANTIDEBUG_TIME_GATE_INTERVAL                  0x00000015 // get, set
#define EV_DI_ANTIDEBUG_TIME_GATE_ERROR_COUNT               0x00000016 // get, set
#define EV_DI_ANTIDEBUG_HEARTBEAT                           0x00000017 // get, set
#define EV_DI_ANTIDEBUG_TIME_GATE                           0x00000018 // get, set
#define EV_DI_ANTIDEBUG_TRADITIONAL                         0x00000019 // get, set
#define EV_DI_SELF_DESTROY                                  0x0000001A // get      when success ,InfoData store self-destroy state.1 means device is self-destroy state, 0 means not.
#define EV_DI_RU_KEY_ALONE                                  0x0000001B // get, set That must fail if set RU_KEY alone directly.                                                                                  
/*******************************************************************************
 device info value definition
******************************************************************************/
#define EV_DISABLE                                          0x00000000
#define EV_ENABLE                                           0x00000001

#define EV_PROTOCOL_HID                                     0x00000000
#define EV_PROTOCOL_CCID                                    0x00000001
#define EV_PROTOCOL_BULK                                    0x00000002

#define EV_ANTIDEBUG_TYPE_NONE                              0x00000000
#define EV_ANTIDEBUG_TYPE_LOCK_FOVEVER                      0x00000001
#define EV_ANTIDEBUG_TYPE_LOCK_TEMP                         0x00000002
/*******************************************************************************
control code definition
******************************************************************************/
#define EV_CONTROL_CODE_UNLOCK_DEVICE                       0xF0000001
#define EV_CONTROL_CODE_GET_ADJUST_REQ_CODE                 0xF0000002
#define EV_CONTROL_CODE_ADJUST_TIME                         0xF0000003

#define EV_CONTROL_CODE_RESET                               0x00000002
#define EV_CONTROL_CODE_LED                                 0x00000003
#define EV_CONTROL_CODE_COM_PROTOCOL                        0x0000000A
#define EV_CONTROL_CODE_SLAVE_ADDR                          0x0000000B              // I2C address
    
#define EV_COM_PROTOCOL_HID                                 0x00000000
#define EV_COM_PROTOCOL_CCID                                0x00000001

typedef struct tagEVLED
{
    EVUINT8 ucIndex;                // LED index. 0 or 1, elte5 support 2 LEDs. 
    EVUINT8 ucState;                // LED State. 0 or 1, 1 represents on, 0 means off
}EVLED;

// EVAdjustBlock struct definition
typedef struct tagEVAdjustBlock
{
    EVUINT32    uiRand;             // Random
    EVUINT32    uiLockTime;         // Lock time     
    EVUINT8     abMac[16];          // mac
    EVUINT32    uiPCTime;           // PC time
} EVAdjustBlock;

/*******************************************************************************
file type definition
******************************************************************************/
#define EV_FILE_TYPE_DATA                                   0x00000000
#define EV_FILE_TYPE_EXE                                    0x00000001
#define EV_FILE_TYPE_KEY                                    0x00000002

/*******************************************************************************
file attribute items definition
******************************************************************************/
#define EV_FILE_ATTR_LENGTH                                 2
#define EV_FILE_ATTR_TYPE                                   3
#define EV_FILE_ATTR_TIME                                   4
#define EV_FILE_ATTR_NAME                                   8
/*******************************************************************************
 struct definition
******************************************************************************/

// EVtime struct definition
typedef struct tagEVTime
{
    EVUINT16 usYear;          // Year.
    EVUINT16 usMonth;         // Month(0 ~ 11).
    EVUINT16 usDay;           // Day.
    EVUINT16 usHour;          // Hour.
    EVUINT16 usMinute;        // Minute.
    EVUINT16 usSecond;        // Second.
} EVTime;

/*******************************************************************************
 ev file name length definition
******************************************************************************/
#define EV_MAX_FILENAME_LENGTH                              0x00000010
#define EV_SERIAL_NUMBER_LENGTH                             0x00000008

// update package information struct definition
typedef struct tagEVUpdatePKGInfo
{
    EVUINT32 uiUpdatePkgVersion;    // Update package version.
    EVUINT32 uiDeveloperID;         // Developer ID.
    EVUINT32 uiProductID;           // Product ID.
    EVUINT32 uiFileOperation;       // File operation.
    EVUINT32 uiFileType;            // File type.
    EVUINT32 uiFileOffset;          // File Offset.
    EVUINT32 uiFileLength;          // File length.
    EVUINT32 uiFileVersion;         // File version.
    EVUINT8  abSerialNumber[EV_SERIAL_NUMBER_LENGTH];     // Serial number
    EVINT8  acFilename[EV_MAX_FILENAME_LENGTH + 1]; // Filename.
}EVUpdatePKGInfo;

// EV library version struct.
typedef struct tagEVLibVersion
{
    EVUINT32 uiMajorVersion;    // Major version.
    EVUINT32 uiMinorVersion;    // Minor version.
    EVUINT32 uiBuildNumber;     // Build number.
    EVUINT32 uiCustomerNumber;  // Customer number
}EVLibVersion;

/*******************************************************************************
file operation of tagEVUpdateInfo
******************************************************************************/
#define EV_RU_FILE_CREATE                                   0x00000000
#define EV_RU_FILE_UPDATE                                   0x00000001
#define EV_RU_FILE_DELETE                                   0x00000002

// EV update information struct.
typedef struct tagEVUpdateOption
{
    EVUINT32 uiProductID;                                       // Product ID
    EVUINT8 abSerialNumber[EV_SERIAL_NUMBER_LENGTH];            // Serial number

    EVUINT32 uiFileOperation;                                   // File operation of updating.
    EVUINT32 uiFileType;                                        // File type.
    EVUINT32 uiFileOffset;                                      // File Offset 
    EVINT8  acFilename[EV_MAX_FILENAME_LENGTH + 1];             // Filename
} EVUpdateOption;

// EV Enum file information struct.
typedef struct tagEVEnumFileInfo
{
    EVUINT32 uiFileIndex;                           // File index in the device.
    EVINT8  acFileName[EV_MAX_FILENAME_LENGTH + 1]; // File name.
}EVEnumFileInfo;

// EV device state struct.
typedef struct tagEVDeviceStates
{
    EVUINT16 usReserved;
    EVUINT8  ubShareMode[2];                        // Share or Exclusive(only valid when key exchanged)
    EVUINT8  ubKeyExchgStatus[2];                   // Check if Key is Exchanged
    EVUINT8  ubLoginStatus[2];                      // Currently login user
    EVUINT32 uiReserved;
    EVUINT32 uiCurrentTime;                         // Current Time (RTC time or Timestamp + timer ticks)
}EVDeviceState;

/*******************************************************************************
device state definition
******************************************************************************/
#define STATES_SHAREMODE_SHARE                                  0
#define STATES_SHAREMODE_EXCLUSIVE                              1
//////////////////////////////////////////////////////////////////////////
#define STATES_KEY_NOT_EXCHANGED                                0
#define STATES_KEY_EXCHANGED                                    0x80    // 0x80 means the highest bit is 1. 
//////////////////////////////////////////////////////////////////////////
#define STATES_PRIVILEGE_ANONYMOUS                              0       // not login
#define STATES_PRIVILEGE_USER                                   1       // user login
#define STATES_PRIVILEGE_DEVELOPER                              2       // developer login

//***************************************************************************************************************
// Method:    EVOpen
// FullName:  EVOpen 
// Returns:   EVUINT32
// Qualifier: EV_ERROR_SUCCESS for success, related Error Code when failed.
// Parameter: IN EVUINT32 uiDeveloperID         :Developer ID.
// Parameter: IN EVUINT32 uiProductID           :Product ID.
// Parameter: IN EVUINT8 * pbSerialNumber       :Pointer to serial number.
// Parameter: IN EVUINT32 uiIndex               :Device index.
// Parameter: OUT EVHANDLE * phEVHandle         :Handle to the opened device.
// Description:Open a elite v device
//***************************************************************************************************************
EVUINT32 EVAPI EVOpen(IN  EVUINT32      uiDeveloperID,
                      IN  EVUINT32      uiProductID,
                      IN  EVUINT8*      pbSerialNumber,
                      IN  EVUINT32      uiIndex,
                      OUT EVHANDLE*     phEVHandle);

//***************************************************************************************************************
// Method:    EVClose
// FullName:  EVClose
// Returns:   EVUINT32
// Qualifier: EV_ERROR_SUCCESS for success, related Error Code when failed.
// Parameter: IN EVHANDLE hEVHandle             :Handle to the device, the handle is created by EVOpen.
// Description:Close the device.
//***************************************************************************************************************
EVUINT32 EVAPI EVClose(IN EVHANDLE  hEVHandle);

//***************************************************************************************************************
// Method:    EVGetDeviceState
// FullName:  EVGetDeviceState
// Returns:   EVUINT32
// Qualifier: EV_ERROR_SUCCESS for success, related Error Code when failed.
// Parameter: IN EVHANDLE hEVHandle         :Handle to the device, the handle is created by EVOpen.
// Parameter: OUT EVDeviceState * pstState  :Pointer to EVDeviceState.
//***************************************************************************************************************
EVUINT32 EVAPI EVGetDeviceState(IN  EVHANDLE     hEVHandle,
                                OUT EVDeviceState*pstState);

//***************************************************************************************************************
// Method:    EVControl
// FullName:  EVControl
// Returns:   EVUINT32
// Qualifier: EV_ERROR_SUCCESS for success, related Error Code when failed.
// Parameter: IN EVHANDLE hEVHandle             :Handle to the device, the handle is created by EVOpen.
// Parameter: IN EVUINT32 uiControlCode         :Control code
// Parameter: IN OUT void * pvControlData           :Control data.
// Description:Reset or unlock the device.
//***************************************************************************************************************
EVUINT32 EVAPI EVControl(IN EVHANDLE  hEVHandle,
                         IN EVUINT32  uiControlCode,
                         IN OUT void*     pvControlData);

//***************************************************************************************************************
// Method:    EVCreateFile
// FullName:  EVCreateFile
// Returns:   EVUINT32
// Qualifier: EV_ERROR_SUCCESS for success, related Error Code when failed.
// Parameter: IN EVHANDLE hEVHandle             :Handle to the device, the handle is created by EVOpen.
// Parameter: IN EVINT8 * pcFileName            :Filename, max length is 16 characters
// Parameter: IN EVUINT32 uiFileType            :File type, see EVF_FILE_TYPE_XX macros
// Parameter: IN EVUINT32 uiFileLength          :Initial length of the file.
// Parameter: IN EVTime * pstEVTime             :Pointer to the creating time of the file, NULL use current time. 
// Description:Create a file in the device.
//***************************************************************************************************************
EVUINT32 EVAPI EVCreateFile(IN EVHANDLE   hEVHandle,
                            IN EVINT8*    pcFileName,
                            IN EVUINT32   uiFileType,
                            IN EVUINT32   uiFileLength,
                            IN EVTime*    pstEVTime);

//***************************************************************************************************************
// Method:    EVDeleteFile
// FullName:  EVDeleteFile
// Returns:   EVUINT32
// Qualifier: EV_ERROR_SUCCESS for success, related Error Code when failed.
// Parameter: IN EVHANDLE hEVHandle             :Handle to the device, the handle is created by EVOpen.
// Parameter: IN EVINT8 * pcFileName            :Filename, max length is 16 characters
// Description:Delete a file in the device.
//***************************************************************************************************************
EVUINT32 EVAPI EVDeleteFile(IN EVHANDLE   hEVHandle,
                            IN EVINT8*    pcFileName);

//***************************************************************************************************************
// Method:    EVReadFile
// FullName:  EVReadFile
// Returns:   EVUINT32
// Qualifier: EV_ERROR_SUCCESS for success, related Error Code when failed.
// Parameter: IN EVHANDLE hEVHandle             :Handle to the device, the handle is created by EVOpen.
// Parameter: IN EVINT8 * pcFileName            :Filename, max length is 16 characters.
// Parameter: IN EVUINT32 uiOffset              :Number of bytes from origin.
// Parameter: OUT EVUINT8 * pbBuffer            :Pointer to read buffer.
// Parameter: IN EVUINT32 uiReadLength          :Read length.
// Description:Read file in the device.
//***************************************************************************************************************
EVUINT32 EVAPI EVReadFile(IN EVHANDLE     hEVHandle,
                          IN EVINT8*      pcFileName,
                          IN EVUINT32     uiOffset,
                          OUT EVUINT8*    pbBuffer,
                          IN EVUINT32     uiReadLength);

//***************************************************************************************************************
// Method:    EVWriteFile
// FullName:  EVWriteFile
// Returns:   EVUINT32
// Qualifier: EV_ERROR_SUCCESS for success, related Error Code when failed.
// Parameter: IN EVHANDLE hEVHandle             :Handle to the device, the handle is created by EVOpen.
// Parameter: IN EVINT8 * pcFileName            :Filename, max length is 16 characters
// Parameter: IN EVUINT32 uiOffset              :Number of bytes from origin.
// Parameter: IN EVUINT8 * pbData               :Pointer to data to be written. 
// Parameter: IN EVUINT32 uiDataLength          :Length of data to be written.
// Parameter: OUT EVUINT32 * puiWritenLength    :The length of full data actually written
// Description:Write data to a file in the device.
//***************************************************************************************************************
EVUINT32 EVAPI EVWriteFile(IN  EVHANDLE   hEVHandle,
                           IN  EVINT8*    pcFileName,
                           IN  EVUINT32   uiOffset,
                           IN  EVUINT8*   pbData,
                           IN  EVUINT32   uiDataLength,
                           OUT EVUINT32*  puiWritenLength);

//***************************************************************************************************************
// Method:    EVEnumFile
// FullName:  EVEnumFile
// Returns:   EVUINT32 EVAPI
// Qualifier: EV_ERROR_SUCCESS for success, related Error Code when failed.
// Parameter: IN EVHANDLE hEVHandle                 :Handle to the device, the handle is created by EVOpen.
// Parameter: IN OUT EVEnumFileInfo * pstEVFileInfo :Pointer to the file information see EVFileInfo.
// Description:Find next file in the device.
//***************************************************************************************************************
EVUINT32 EVAPI EVEnumFile(IN EVHANDLE             hEVHandle,
                          IN OUT EVEnumFileInfo*  pstEVEnumFile);

//***************************************************************************************************************
// Method:    EVGetFileAttribute
// FullName:  EVGetFileAttribute
// Returns:   EVUINT32
// Qualifier: EV_ERROR_SUCCESS for success, related Error Code when failed.
// Parameter: IN EVHANDLE hEVHandle           :Handle to the device, the handle is created by EVOpen.
// Parameter: IN EVINT8 * pcFilename          :Filename.
// Parameter: IN EVUINT32 uiFileAttrFlag      :File attribute flag, should be EV_FILE_ATTR_LENGTH, EV_FILE_ATTR_TIME
//                                            or EV_FILE_ATTR_TYPE
// Parameter: OUT void * pvFileAttr           :File attribute buffer.
// Parameter: IN EVUINT32 uiFileAttrSize      :File attribute buffer size.
// Description:Get ev file attribute.
//***************************************************************************************************************
EVUINT32 EVAPI EVGetFileAttribute(IN EVHANDLE     hEVHandle,
                                  IN EVINT8*      pcFilename,
                                  IN EVUINT32     uiFileAttrType,
                                  OUT void*       pvFileAttrBuffer,
                                  IN EVUINT32     uiFileAttrSize);

//***************************************************************************************************************
// Method:    EVSetFileAttribute
// FullName:  EVSetFileAttribute
// Returns:   EVUINT32
// Qualifier: EV_ERROR_SUCCESS for success, related Error Code when failed.
// Parameter: IN EVHANDLE hEVHandle           :Handle to the device, the handle is created by EVOpen.
// Parameter: IN EVINT8 * pcFilename          :Filename.
// Parameter: IN EVUINT32 uiFileAttrFlag      :File attribute flag, should be EV_FILE_ATTR_LENGTH, EV_FILE_ATTR_TIME
//                                             EV_FILE_ATTR_NAME or EV_FILE_ATTR_ENABLE_EXE.
// Parameter: OUT void * pvFileAttr           :File attribute buffer.
// Parameter: IN EVUINT32 uiFileAttrSize      :File attribute buffer size.
// Description:Set ev file attribute.
//***************************************************************************************************************
EVUINT32 EVAPI EVSetFileAttribute(IN EVHANDLE     hEVHandle,
                                  IN EVINT8*      pcFileName, 
                                  IN EVUINT32     uiFileAttrType,
                                  IN void*        pvFileAttr,
                                  IN EVUINT32     uiFileAttrSize);

//***************************************************************************************************************
// Method:    EVExecute
// FullName:  EVExecute
// Returns:   EVUINT32
// Qualifier: EV_ERROR_SUCCESS for success, related Error Code when failed.
// Parameter: IN EVHANDLE     hEVHandle             :Handle to the device, the handle is created by EVOpen.
// Parameter: IN EVINT8 *     pcFileName            :File name of the exe file.
// Parameter: IN EVUINT8*      ucInBuffer            :Pointer to input buffer.
// Parameter: IN EVUINT32      uiInBufferSize        :Input buffer size.
// Parameter: OUT EVUINT8*     ucOutBuffer           :Pointer to output buffer.
// Parameter: IN EVUINT32      uiOutBufferSize       :Output buffer size.
// Parameter: OUT EVUINT32*    uiBytesReturned       :Pointer to a UINT32 to store returned buffer size.    
// Description:Execute an exe file in the device.
//***************************************************************************************************************
EVUINT32 EVAPI EVExecute(IN EVHANDLE      hEVHandle, 
                         IN EVINT8*       pcFileName,
                         IN EVUINT8*      pucInBuffer,
                         IN EVUINT32      uiInBufferSize,
                         OUT EVUINT8*     pucOutBuffer,
                         IN EVUINT32      uiOutBufferSize,
                         OUT EVUINT32*    puiBytesReturned
                         ); 

//***************************************************************************************************************
// Method:    EVLogin 
// FullName:  EVLogin
// Returns:   EVUINT32
// Qualifier: EV_ERROR_SUCCESS for success, related Error Code when failed.
// Parameter: IN EVHANDLE hEVHandle             :Handle to the device, the handle is created by EVOpen.
// Parameter: IN EVUINT32 uiLogonMode           :Log in mode.
// Parameter: IN EVUINT32 uiPasswordType        :Password type,EV_PASSWORD_TYPE_USER or
//                                               EV_PASSWORD_TYPE_DEVELOPER.
// Parameter: IN EVUINT8 * pbPassword           :Pointer to password.
// Parameter: IN EVUINT32 uiPasswordLength      :Password length, user password is 8 bytes, developer password is 
//                                               24 bytes.
// Description:Log in the device. 
//***************************************************************************************************************
EVUINT32 EVAPI EVLogin(IN EVHANDLE hEVHandle,
                       IN EVUINT32 uiLoginMode,
                       IN EVUINT32 uiPasswordType,
                       IN EVINT8  *pbPassword,
                       IN EVUINT32 uiPasswordLength);

//***************************************************************************************************************
// Method:    EVLogout
// FullName:  EVLogout
// Returns:   EVUINT32
// Qualifier: EV_ERROR_SUCCESS for success, related Error Code when failed.
// Parameter: IN EVHANDLE hEVHandle          :Handle to the device, the handle is created by EVOpen.
// Description:Log out the device. 
//***************************************************************************************************************
EVUINT32 EVAPI EVLogout(IN EVHANDLE hEVHandle);

//***************************************************************************************************************
// Method:    EVGetLibVersion
// FullName:  EVGetLibVersion
// Returns:   EVUINT32
// Qualifier: EV_ERROR_SUCCESS for success, related Error Code when failed.
// Parameter: OUT EVLibVersion * piLibVersion   :Pointer of the lib version.
// Description:Get the version of current lib.
//***************************************************************************************************************
EVUINT32 EVAPI EVGetLibVersion(OUT EVLibVersion* pstlibVersion);

//***************************************************************************************************************
// Method:    EVGetDeviceInfo
// FullName:  EVGetDeviceInfo
// Returns:   EVUINT32
// Qualifier: EV_ERROR_SUCCESS for success, related Error Code when failed.
// Parameter: IN EVHANDLE hEVHandle         :Handle to the device, the handle is created by EVOpen.
// Parameter: IN EVUINT32 uiInfoItem        :Specific the information you want to get.
// Parameter: OUT void * pvBuffer           :Pointer to the information data receiving buffer.
// Parameter: IN EVUINT32 uiBufferLength    :Length of data receiving buffer.
// Description:Get specific information of device.
//***************************************************************************************************************
EVUINT32 EVAPI EVGetDeviceInfo(IN  EVHANDLE   hEVHandle,
                               IN  EVUINT32   uiInfoItem,
                               OUT void*      pvBuffer,
                               IN  EVUINT32   uiBufferLength);

//***************************************************************************************************************
// Method:    EVSetDeviceInfo
// FullName:  EVSetDeviceInfo
// Returns:   EVUINT32
// Qualifier: EV_ERROR_SUCCESS for success, related Error Code when failed.
// Parameter: IN EVHANDLE hEVHandle             :Handle to the device, the handle is created by EVOpen.
// Parameter: IN EVUINT32 uiItem                :Item to be set.
// Parameter: IN void * pbData                  :Pinter to the item data.
// Parameter: IN EVUINT32 uiDataLength
// Description: Set the device information or make some functions usable or unusable
//***************************************************************************************************************
EVUINT32 EVAPI EVSetDeviceInfo(IN EVHANDLE  hEVHandle,
                               IN EVUINT32  uiItem,
                               IN void*     pbData,
                               IN EVUINT32  uiDataLength);

//***************************************************************************************************************
// Method:    EVGenerateUpdatePkg
// FullName:  EVGenerateUpdatePkg
// Returns:   EVUINT32
// Qualifier: EV_ERROR_SUCCESS for success, related Error Code when failed
// Parameter: IN EVHANDLE hEVHandle                     :Handle to the device, the handle is created by EVOpen.
// Parameter: IN EVUpdateOption * pstEVUpdateOption     :Pointer to update package information, see EVUpdateOption
// Parameter: IN EVUINT8 * pbSourceData                 :Pointer to the data to be update.
// Parameter: IN EVUINT32 uiSourceDataLength            :Length of the data to be update.
// Parameter: OUT EVUINT8 * pbUpdatePkgBuffer           :Pointer to update package.
// Parameter: IN EVUINT32 uiUpdatePkgBufferLength       :Length of update package buffer.
// Parameter: OUT EVUINT32 * puiUpdatePkgLength         :Pointer to the actual package length.
// Description:Generate a update package for specific device(s).
//***************************************************************************************************************
EVUINT32 EVAPI EVGenerateUpdatePkg( IN  EVHANDLE            hEVHandle,
                                    IN  EVUpdateOption*     pstEVUpdateOption,
                                    IN  EVUINT8*            pbSourceData,
                                    IN  EVUINT32            uiSourceDataLength,
                                    OUT EVUINT8*            pbUpdatePkgBuffer,
                                    IN  EVUINT32            uiUpdatePkgBufferLength,
                                    OUT EVUINT32*           puiUpdatePkgLength);

//***************************************************************************************************************
// Method:    EVGetUpdatePkgInfo
// FullName:  EVGetUpdatePkgInfo
// Returns:   EVUINT32 EVAPI
// Qualifier: EV_ERROR_SUCCESS for success, related Error Code when failed.
// Parameter: IN EVHANDLE hEVHandle                  :Handle to the device, the handle is created by EVOpen.
// Parameter: OUT EVUpdatePKGInfo * pEVUpdatePKGInfo :Pointer to EVUpdateInfo
// Description:Get update package information of specific update package.
//***************************************************************************************************************
EVUINT32 EVAPI EVGetUpdatePkgInfo(IN EVUINT8*           pbUpdatePkgData,
                                  OUT EVUpdatePKGInfo*  pEVUpdatePKGInfo);

//***************************************************************************************************************
// Method:    EVUpdate
// FullName:  EVUpdate
// Returns:   EVUINT32
// Qualifier: EV_ERROR_SUCCESS for success, related Error Code when failed.
// Parameter: IN EVHANDLE hEVHandle         :Handle to the device, the handle is created by EVOpen.
// Parameter: IN EVUINT8 * pbUpdateData     :Data to be updated.
// Parameter: IN EVUINT32 uiLength          :Length of updating data.
// Description:Update the device with update package.
//***************************************************************************************************************
EVUINT32 EVAPI EVUpdate(IN EVHANDLE   hEVHandle,
                        IN EVUINT8*   pbUpdateData,
                        IN EVUINT32   uiLength);

//***************************************************************************************************************
// Method:    EVLock
// FullName:  EVLock
// Returns:   EVUINT32
// Qualifier: EV_ERROR_SUCCESS for success, related Error Code when failed.
// Parameter: IN EVHANDLE hEVHandle             :Handle to the device, the handle is created by EVOpen.
// Parameter: IN EVUINT32 iMilliseconds         :Specifies the time-out interval, in milliseconds. 
// Description:Begin to using a device exclusively, before calling EVEndExclusivelyUsingDevice, only this handle
//            :have the right to communication with device.
//***************************************************************************************************************
EVUINT32 EVAPI EVLock(IN EVHANDLE     hEVHandle,
                      IN EVUINT32     uiMilliseconds);

//***************************************************************************************************************
// Method:    EVUnLock
// FullName:  EVUnLock
// Returns:   EVUINT32
// Qualifier: EV_ERROR_SUCCESS for success, related Error Code when failed.
// Parameter: IN EVHANDLE hEVHandle        :Handle to the device, the handle is created by EVOpen.
// Description:End exclusively using a device. 
//***************************************************************************************************************
EVUINT32 EVAPI EVUnLock(IN EVHANDLE hEVHandle);

//***************************************************************************************************************
// Method:    EVSetPassword
// FullName:  EVSetPassword
// Returns:   EVUINT32 EVAPI
// Qualifier: EV_ERROR_SUCCESS for success, related Error Code when failed.
// Parameter: IN EVHANDLE hEVHandle             :Handle to the device, the handle is created by EVOpen.
// Parameter: IN EVUINT32 uiPasswordType        :Password type, user, developer and remote update key.
// Parameter: IN EVUINT8 * pbPassword           :Pointer to password.
// Parameter: IN EVUINT32 uiPasswordLength      :Password length.
// Description:Set password or remote update key.
//***************************************************************************************************************
EVUINT32 EVAPI EVSetPassword(IN EVHANDLE  hEVHandle,
                             IN EVUINT32  uiPasswordType,
                             IN EVINT8*   pbPassword,
                             IN EVUINT32  uiPasswordLength);
#ifdef _MSC_VER
#pragma pack(pop)
#endif//_MSC_VER

#ifdef __cplusplus
}
#endif

#pragma comment(lib, "setupapi.lib")

#endif//_ELITE5_H
