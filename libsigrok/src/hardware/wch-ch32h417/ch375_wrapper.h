/*
 * This file is part of the LogicAnalyzer project.
 * LogicAnaylzer is based on libsigrok.
 *
 * Copyright (C) 2026 Q2H2
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * CH375DLL dynamic loading wrapper
 * Loads the DLL functions with LoadLibrary, no .lib file needed
 * Uses C standard types so it also builds with mingw-w64 cross compilation
 */

#ifndef LIBSIGROK_HARDWARE_WCH_CH32H417_CH375_WRAPPER_H
#define LIBSIGROK_HARDWARE_WCH_CH32H417_CH375_WRAPPER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * Constants
 * ============================================================================ */

/* CH375 packet length */
#define CH375_PACKET_LENGTH     64

/* CH375 endpoint addresses */
#define CH375_ENDP_AUX_UP       0x81    /* Interrupt upload endpoint */
#define CH375_ENDP_AUX_DOWN     0x01    /* Auxiliary download endpoint */
#define CH375_ENDP_DATA_UP      0x82    /* Data block upload endpoint */
#define CH375_ENDP_DATA_DOWN    0x02    /* Data block download endpoint */

/* Maximum number of devices */
#define CH375_MAX_NUMBER        16

/* Invalid handle value */
#define CH375_INVALID_HANDLE    ((void *)-1)

/* Return values */
#define CH375_TRUE              1
#define CH375_FALSE             0

/* Device events (used by the CH375SetDeviceNotify callback) */
#define CH375_DEVICE_ARRIVAL        3   /* Device arrival event */
#define CH375_DEVICE_REMOVE_PEND    1   /* Device removal pending */
#define CH375_DEVICE_REMOVE         0   /* Device removal event */

/* Default buffered upload size (1 MB) */
#define CH375_DEFAULT_TRANSFER_SIZE    (1024 * 1024)

/* ============================================================================
 * Function pointer typedefs
 * ============================================================================ */

/* Device management */
typedef void* (*CH375OpenDevice_func)(unsigned long iIndex);
typedef void (*CH375CloseDevice_func)(unsigned long iIndex);
typedef unsigned long (*CH375GetVersion_func)(void);
typedef unsigned long (*CH375GetDrvVersion_func)(void);
typedef unsigned long (*CH375GetUsbID_func)(unsigned long iIndex);
typedef void* (*CH375GetDeviceName_func)(unsigned long iIndex);

/* Device descriptor */
typedef int (*CH375GetDeviceDescr_func)(unsigned long iIndex, void *oBuffer, unsigned long *ioLength);
typedef int (*CH375GetConfigDescr_func)(unsigned long iIndex, void *oBuffer, unsigned long *ioLength);

/* Device control */
typedef int (*CH375ResetDevice_func)(unsigned long iIndex);
typedef int (*CH375SetExclusive_func)(unsigned long iIndex, unsigned long iExclusive);

/* Timeout settings */
typedef int (*CH375SetTimeout_func)(unsigned long iIndex, unsigned long iWriteTimeout, unsigned long iReadTimeout);
typedef int (*CH375SetTimeoutEx_func)(unsigned long iIndex, unsigned long iWriteTimeout, unsigned long iReadTimeout,
                                      unsigned long iAuxTimeout, unsigned long iInterTimeout);

/* Endpoint 1 direct up/down transfer */
typedef int (*CH375WriteRead_func)(unsigned long iIndex, void *iBuffer, void *oBuffer, unsigned long *ioLength);

/* Endpoint read/write */
typedef int (*CH375WriteEndP_func)(unsigned long iIndex, unsigned long iEndP, void *iBuffer, unsigned long *ioLength);
typedef int (*CH375ReadEndP_func)(unsigned long iIndex, unsigned long iEndP, void *oBuffer, unsigned long *ioLength);

/* Endpoint control */
typedef int (*CH375AbortEndPRead_func)(unsigned long iIndex, unsigned long iEndP);
typedef int (*CH375AbortEndPWrite_func)(unsigned long iIndex, unsigned long iEndP);
typedef int (*CH375ResetInEndP_func)(unsigned long iIndex, unsigned long iEndP);
typedef int (*CH375ResetOutEndP_func)(unsigned long iIndex, unsigned long iEndP);

/* Data endpoint read/write */
typedef int (*CH375ReadData_func)(unsigned long iIndex, void *oBuffer, unsigned long *ioLength);
typedef int (*CH375WriteData_func)(unsigned long iIndex, void *iBuffer, unsigned long *ioLength);
typedef int (*CH375AbortRead_func)(unsigned long iIndex);
typedef int (*CH375AbortWrite_func)(unsigned long iIndex);

/* Buffered upload mode */
typedef int (*CH375SetBufUpload_func)(unsigned long iIndex, unsigned long iEnableOrClear);
typedef long (*CH375QueryBufUpload_func)(unsigned long iIndex);
typedef int (*CH375SetBufUploadEx_func)(unsigned long iIndex, unsigned long iEnableOrClear,
                                        unsigned long iEndP, unsigned long TransferSize);
typedef int (*CH375QueryBufUploadEx_func)(unsigned long iIndex, unsigned long iEndP,
                                          unsigned long *oTransferCount, unsigned long *oTotalDataLen);
typedef int (*CH375ClearBufUpload_func)(unsigned long iIndex, unsigned long iEndP);

/* Buffered download mode */
typedef int (*CH375SetBufDownload_func)(unsigned long iIndex, unsigned long iEnableOrClear);
typedef long (*CH375QueryBufDownload_func)(unsigned long iIndex);
typedef int (*CH375SetBufDownloadEx_func)(unsigned long iIndex, unsigned long iEnableOrClear,
                                          unsigned long iEndP, unsigned long TransferSize);

/* IO mode setting */
typedef int (*CH375SetIOMode_func)(unsigned long iIndex, unsigned long iData);

/* Device event notification callback type */
typedef void (*CH375NotifyCallback)(unsigned long iEventStatus);

/* Device event notification */
typedef int (*CH375SetDeviceNotify_func)(unsigned long iIndex, char *iDeviceID, CH375NotifyCallback iNotifyRoutine);

/* CH375 driver commands */
typedef int (*CH375DriverCommand_func)(unsigned long iIndex, void *ioCommand);

/* CH375 driver command definitions */
#define mFuncSetIOMode          0x00000012    /* Set IO mode */

/* WIN32 command structure */
#pragma pack(push, 1)
typedef struct _mWIN32_COMMAND {
    unsigned long mFunction;        /* Function code */
    unsigned long mLength;          /* Data length */
    unsigned char mBuffer[64];      /* Data buffer */
} mWIN32_COMMAND;
#pragma pack(pop)

/* ============================================================================
 * Wrapper API declarations
 * ============================================================================ */

/* DLL load/unload */
int ch375_init(void);
void ch375_cleanup(void);
int ch375_is_loaded(void);

unsigned long ch375_get_version(void);

/* Device management */
void* ch375_open_device(unsigned long index);
void ch375_close_device(unsigned long index);
unsigned long ch375_get_usb_id(unsigned long index);
const char* ch375_get_device_name(unsigned long index);
int ch375_get_usb_path(unsigned long index, char *path, int path_len);
int ch375_get_device_descr(unsigned long index, void *buffer, unsigned long *length);
int ch375_get_config_descr(unsigned long index, void *buffer, unsigned long *length);
int ch375_reset_device(unsigned long index);

/* Timeout settings */
int ch375_set_timeout(unsigned long index, unsigned long writeTimeout, unsigned long readTimeout);
int ch375_set_timeout_ex(unsigned long index, unsigned long writeTimeout, unsigned long readTimeout,
                         unsigned long auxTimeout, unsigned long interTimeout);

/* Endpoint 1 direct up/down transfer (command downlink and response readback) */
int ch375_write_read(unsigned long index, void *iBuffer, void *oBuffer, unsigned long *ioLength);

/* Endpoint read/write (separate write and read) - pipe_num is the pipe number (1,2,3) */
int ch375_write_endpoint(unsigned long index, unsigned long pipe_num, void *buffer, unsigned long *length);
int ch375_read_endpoint(unsigned long index, unsigned long pipe_num, void *buffer, unsigned long *length);

/* Endpoint control - pipe_num is the pipe number (1,2,3) */
int ch375_abort_endpoint_read(unsigned long index, unsigned long pipe_num);
int ch375_abort_endpoint_write(unsigned long index, unsigned long pipe_num);
int ch375_reset_in_endpoint(unsigned long index, unsigned long pipe_num);
int ch375_reset_out_endpoint(unsigned long index, unsigned long pipe_num);

/* Buffered upload mode (endpoint 2/3 data transfer) - pipe_num is the pipe number (1,2,3) */
int ch375_set_buf_upload_ex(unsigned long index, unsigned long enable, unsigned long pipe_num, unsigned long transferSize);
int ch375_query_buf_upload_ex(unsigned long index, unsigned long pipe_num, unsigned long *transferCount, unsigned long *totalDataLen);
int ch375_clear_buf_upload(unsigned long index, unsigned long pipe_num);

/* Buffered download mode - pipe_num is the pipe number (1,2,3) */
int ch375_set_buf_download_ex(unsigned long index, unsigned long enable, unsigned long pipe_num, unsigned long transferSize);

/* IO mode setting - whether read and write are synchronous in buffered upload mode */
int ch375_set_io_mode(unsigned long index, unsigned long sync);

/* Device event notification - register/unregister the arrival and removal callbacks */
int ch375_set_device_notify(unsigned long index, char *deviceID, CH375NotifyCallback callback);

/* Data endpoint read/write */
int ch375_read_data(unsigned long index, void *buffer, unsigned long *length);
int ch375_write_data(unsigned long index, void *buffer, unsigned long *length);
int ch375_abort_read(unsigned long index);
int ch375_abort_write(unsigned long index);

#ifdef __cplusplus
}
#endif

#endif /* LIBSIGROK_HARDWARE_WCH_CH32H417_CH375_WRAPPER_H */
