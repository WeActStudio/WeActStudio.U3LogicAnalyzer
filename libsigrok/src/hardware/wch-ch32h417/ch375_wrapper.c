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
 * CH375DLL dynamic loading wrapper implementation
 * Loads the DLL functions at runtime through LoadLibrary
 */

#include <config.h>
#include "ch375_wrapper.h"
#include <libsigrok/libsigrok.h>
#include "libsigrok-internal.h"
#include <glib.h>

/* Windows API headers - mingw-w64 cross compilation */
#ifdef _WIN32
#include <windows.h>
#else
/* mingw-w64 cross compilation still uses the Windows headers */
#include <windef.h>
#include <winbase.h>
#include <libloaderapi.h>
#endif

#define LOG_PREFIX "ch375"

/* ============================================================================
 * DLL handle and function pointers
 * ============================================================================ */

static HMODULE ch375_dll_handle = NULL;

/* Device management function pointers */
static CH375OpenDevice_func pCH375OpenDevice = NULL;
static CH375CloseDevice_func pCH375CloseDevice = NULL;
static CH375GetVersion_func pCH375GetVersion = NULL;
static CH375GetDrvVersion_func pCH375GetDrvVersion = NULL;
static CH375GetUsbID_func pCH375GetUsbID = NULL;
static CH375GetDeviceName_func pCH375GetDeviceName = NULL;

/* Device descriptor function pointers */
static CH375GetDeviceDescr_func pCH375GetDeviceDescr = NULL;
static CH375GetConfigDescr_func pCH375GetConfigDescr = NULL;

/* Device control function pointers */
static CH375ResetDevice_func pCH375ResetDevice = NULL;
static CH375SetExclusive_func pCH375SetExclusive = NULL;

/* Timeout setting function pointers */
static CH375SetTimeout_func pCH375SetTimeout = NULL;
static CH375SetTimeoutEx_func pCH375SetTimeoutEx = NULL;

/* Endpoint 1 direct transfer function pointers */
static CH375WriteRead_func pCH375WriteRead = NULL;

/* Endpoint read/write function pointers */
static CH375WriteEndP_func pCH375WriteEndP = NULL;
static CH375ReadEndP_func pCH375ReadEndP = NULL;

/* Endpoint control function pointers */
static CH375AbortEndPRead_func pCH375AbortEndPRead = NULL;
static CH375AbortEndPWrite_func pCH375AbortEndPWrite = NULL;
static CH375ResetInEndP_func pCH375ResetInEndP = NULL;
static CH375ResetOutEndP_func pCH375ResetOutEndP = NULL;

/* Data endpoint read/write function pointers */
static CH375ReadData_func pCH375ReadData = NULL;
static CH375WriteData_func pCH375WriteData = NULL;
static CH375AbortRead_func pCH375AbortRead = NULL;
static CH375AbortWrite_func pCH375AbortWrite = NULL;

/* Buffered upload mode function pointers */
static CH375SetBufUpload_func pCH375SetBufUpload = NULL;
static CH375QueryBufUpload_func pCH375QueryBufUpload = NULL;
static CH375SetBufUploadEx_func pCH375SetBufUploadEx = NULL;
static CH375QueryBufUploadEx_func pCH375QueryBufUploadEx = NULL;
static CH375ClearBufUpload_func pCH375ClearBufUpload = NULL;

/* Buffered download mode function pointers */
static CH375SetBufDownload_func pCH375SetBufDownload = NULL;
static CH375QueryBufDownload_func pCH375QueryBufDownload = NULL;
static CH375SetBufDownloadEx_func pCH375SetBufDownloadEx = NULL;

/* Driver command function pointers */
static CH375DriverCommand_func pCH375DriverCommand = NULL;

/* Device event notification function pointers */
static CH375SetDeviceNotify_func pCH375SetDeviceNotify = NULL;

/* ============================================================================
 * Helper macro for dynamic loading
 * ============================================================================ */

#define LOAD_FUNC(name) \
    do { \
        p##name = (name##_func)GetProcAddress(ch375_dll_handle, #name); \
        if (!p##name) { \
            sr_err("Failed to get " #name " function address"); \
            return SR_ERR; \
        } \
    } while(0)

#define LOAD_FUNC_OPTIONAL(name) \
    do { \
        p##name = (name##_func)GetProcAddress(ch375_dll_handle, #name); \
    } while(0)

/* ============================================================================
 * DLL load/unload
 * ============================================================================ */

int ch375_init(void)
{
    if (ch375_dll_handle != NULL) {
        /* Already loaded */
        return SR_OK;
    }

    /* Try to load CH375DLL64.DLL */
    ch375_dll_handle = LoadLibraryA("CH375DLL64.DLL");
    if (ch375_dll_handle == NULL) {
        /* Try to load CH375DLL.DLL (32-bit build) */
        ch375_dll_handle = LoadLibraryA("CH375DLL.DLL");
        if (ch375_dll_handle == NULL) {
            sr_err("Failed to load CH375DLL64.DLL or CH375DLL.DLL");
            return SR_ERR;
        }
        sr_warn("Loaded CH375DLL.DLL (32-bit), consider using 64-bit version");
    }

    /* Load all required functions */
    LOAD_FUNC(CH375OpenDevice);
    LOAD_FUNC(CH375CloseDevice);
    LOAD_FUNC(CH375GetUsbID);
    LOAD_FUNC(CH375SetTimeout);
    LOAD_FUNC(CH375WriteRead);
    LOAD_FUNC(CH375ReadEndP);
    LOAD_FUNC(CH375SetBufUploadEx);
    LOAD_FUNC(CH375QueryBufUploadEx);
    LOAD_FUNC(CH375ClearBufUpload);
    LOAD_FUNC(CH375AbortEndPRead);

    /* Load the optional functions */
    LOAD_FUNC_OPTIONAL(CH375GetVersion);
    LOAD_FUNC_OPTIONAL(CH375GetDrvVersion);
    LOAD_FUNC_OPTIONAL(CH375GetDeviceName);
    LOAD_FUNC_OPTIONAL(CH375GetDeviceDescr);
    LOAD_FUNC_OPTIONAL(CH375GetConfigDescr);
    LOAD_FUNC_OPTIONAL(CH375ResetDevice);
    LOAD_FUNC_OPTIONAL(CH375SetExclusive);
    LOAD_FUNC_OPTIONAL(CH375SetTimeoutEx);
    LOAD_FUNC_OPTIONAL(CH375WriteEndP);
    LOAD_FUNC_OPTIONAL(CH375AbortEndPWrite);
    LOAD_FUNC_OPTIONAL(CH375ResetInEndP);
    LOAD_FUNC_OPTIONAL(CH375ResetOutEndP);
    LOAD_FUNC_OPTIONAL(CH375ReadData);
    LOAD_FUNC_OPTIONAL(CH375WriteData);
    LOAD_FUNC_OPTIONAL(CH375AbortRead);
    LOAD_FUNC_OPTIONAL(CH375AbortWrite);
    LOAD_FUNC_OPTIONAL(CH375SetBufUpload);
    LOAD_FUNC_OPTIONAL(CH375QueryBufUpload);
    LOAD_FUNC_OPTIONAL(CH375SetBufDownload);
    LOAD_FUNC_OPTIONAL(CH375QueryBufDownload);
    LOAD_FUNC_OPTIONAL(CH375SetBufDownloadEx);
    LOAD_FUNC_OPTIONAL(CH375DriverCommand);
    LOAD_FUNC_OPTIONAL(CH375WriteData);
    LOAD_FUNC_OPTIONAL(CH375SetDeviceNotify);
    
    unsigned long version = ch375_get_version();
    sr_info("CH375DLL loaded successfully, version: %08x", version);

    return SR_OK;
}

void ch375_cleanup(void)
{
    if (ch375_dll_handle != NULL) {
        FreeLibrary(ch375_dll_handle);
        ch375_dll_handle = NULL;

        /* Clear all function pointers */
        pCH375OpenDevice = NULL;
        pCH375CloseDevice = NULL;
        pCH375GetVersion = NULL;
        pCH375GetDrvVersion = NULL;
        pCH375GetUsbID = NULL;
        pCH375GetDeviceName = NULL;
        pCH375GetDeviceDescr = NULL;
        pCH375GetConfigDescr = NULL;
        pCH375ResetDevice = NULL;
        pCH375SetExclusive = NULL;
        pCH375SetTimeout = NULL;
        pCH375SetTimeoutEx = NULL;
        pCH375WriteRead = NULL;
        pCH375WriteEndP = NULL;
        pCH375ReadEndP = NULL;
        pCH375AbortEndPRead = NULL;
        pCH375AbortEndPWrite = NULL;
        pCH375ResetInEndP = NULL;
        pCH375ResetOutEndP = NULL;
        pCH375ReadData = NULL;
        pCH375WriteData = NULL;
        pCH375AbortRead = NULL;
        pCH375AbortWrite = NULL;
        pCH375SetBufUpload = NULL;
        pCH375QueryBufUpload = NULL;
        pCH375SetBufUploadEx = NULL;
        pCH375QueryBufUploadEx = NULL;
        pCH375ClearBufUpload = NULL;
        pCH375SetBufDownload = NULL;
        pCH375QueryBufDownload = NULL;
        pCH375SetBufDownloadEx = NULL;
        pCH375DriverCommand = NULL;
        pCH375SetDeviceNotify = NULL;

        sr_info("CH375DLL unloaded");
    }
}

int ch375_is_loaded(void)
{
    return (ch375_dll_handle != NULL);
}

unsigned long ch375_get_version(void)
{
    if (!pCH375GetVersion) {
        sr_err("CH375GetVersion not loaded");
        return 0;
    }
    return pCH375GetVersion();
}

/* ============================================================================
 * Device management
 * ============================================================================ */

void* ch375_open_device(unsigned long index)
{
    if (!pCH375OpenDevice) {
        sr_err("CH375OpenDevice not loaded");
        return CH375_INVALID_HANDLE;
    }

    void* handle = pCH375OpenDevice(index);
    if (handle == CH375_INVALID_HANDLE || handle == NULL) {
        sr_dbg("Failed to open device at index %lu", index);
        return CH375_INVALID_HANDLE;
    }

    sr_dbg("Opened device at index %lu", index);
    return handle;
}

void ch375_close_device(unsigned long index)
{
    if (pCH375CloseDevice) {
        pCH375CloseDevice(index);
        sr_dbg("Closed device at index %lu", index);
    }
}

unsigned long ch375_get_usb_id(unsigned long index)
{
    if (!pCH375GetUsbID) {
        sr_err("CH375GetUsbID not loaded");
        return 0;
    }
    return pCH375GetUsbID(index);
}

const char* ch375_get_device_name(unsigned long index)
{
    if (!pCH375GetDeviceName) {
        return NULL;
    }
    return (const char*)pCH375GetDeviceName(index);
}

int ch375_get_device_descr(unsigned long index, void *buffer, unsigned long *length)
{
    if (!pCH375GetDeviceDescr) {
        return CH375_FALSE;
    }
    return pCH375GetDeviceDescr(index, buffer, length);
}

int ch375_get_config_descr(unsigned long index, void *buffer, unsigned long *length)
{
    if (!pCH375GetConfigDescr) {
        return CH375_FALSE;
    }
    return pCH375GetConfigDescr(index, buffer, length);
}

int ch375_reset_device(unsigned long index)
{
    if (!pCH375ResetDevice) {
        return CH375_FALSE;
    }
    return pCH375ResetDevice(index);
}

/* ============================================================================
 * Timeout setting
 * ============================================================================ */

int ch375_set_timeout(unsigned long index, unsigned long writeTimeout, unsigned long readTimeout)
{
    if (!pCH375SetTimeout) {
        sr_err("CH375SetTimeout not loaded");
        return CH375_FALSE;
    }
    return pCH375SetTimeout(index, writeTimeout, readTimeout);
}

int ch375_set_timeout_ex(unsigned long index, unsigned long writeTimeout, unsigned long readTimeout,
                         unsigned long auxTimeout, unsigned long interTimeout)
{
    if (!pCH375SetTimeoutEx) {
        /* Fall back to the basic timeout setter */
        return ch375_set_timeout(index, writeTimeout, readTimeout);
    }
    return pCH375SetTimeoutEx(index, writeTimeout, readTimeout, auxTimeout, interTimeout);
}

/* ============================================================================
 * Endpoint 1 direct transfer (command transfer)
 * ============================================================================ */

int ch375_write_read(unsigned long index, void *iBuffer, void *oBuffer, unsigned long *ioLength)
{
    if (!pCH375WriteRead) {
        sr_err("CH375WriteRead not loaded");
        return CH375_FALSE;
    }

    int result = pCH375WriteRead(index, iBuffer, oBuffer, ioLength);
    if (!result) {
        sr_dbg("CH375WriteRead failed at index %lu", index);
    }
    return result;
}

/* ============================================================================
 * Endpoint read/write (separate write and read)
 * ============================================================================ */

int ch375_write_endpoint(unsigned long index, unsigned long endpoint, void *buffer, unsigned long *length)
{
    if (!pCH375WriteEndP) {
        sr_err("CH375WriteEndP not loaded");
        return CH375_FALSE;
    }
    return pCH375WriteEndP(index, endpoint, buffer, length);
}

int ch375_read_endpoint(unsigned long index, unsigned long endpoint, void *buffer, unsigned long *length)
{
    if (!pCH375ReadEndP) {
        sr_err("CH375ReadEndP not loaded");
        return CH375_FALSE;
    }
    return pCH375ReadEndP(index, endpoint, buffer, length);
}

/* ============================================================================
 * Endpoint control
 * ============================================================================ */

int ch375_abort_endpoint_read(unsigned long index, unsigned long endpoint)
{
    if (!pCH375AbortEndPRead) {
        return CH375_FALSE;
    }
    return pCH375AbortEndPRead(index, endpoint);
}

int ch375_abort_endpoint_write(unsigned long index, unsigned long endpoint)
{
    if (!pCH375AbortEndPWrite) {
        return CH375_FALSE;
    }
    return pCH375AbortEndPWrite(index, endpoint);
}

int ch375_reset_in_endpoint(unsigned long index, unsigned long endpoint)
{
    if (!pCH375ResetInEndP) {
        return CH375_FALSE;
    }
    return pCH375ResetInEndP(index, endpoint);
}

int ch375_reset_out_endpoint(unsigned long index, unsigned long endpoint)
{
    if (!pCH375ResetOutEndP) {
        return CH375_FALSE;
    }
    return pCH375ResetOutEndP(index, endpoint);
}

/* ============================================================================
 * Buffered upload mode (endpoint 2/3 data transfer)
 * ============================================================================ */

int ch375_set_buf_upload_ex(unsigned long index, unsigned long enable, unsigned long endpoint, unsigned long transferSize)
{
    if (!pCH375SetBufUploadEx) {
        sr_err("CH375SetBufUploadEx not loaded");
        return CH375_FALSE;
    }

    int result = pCH375SetBufUploadEx(index, enable, endpoint, transferSize);
    if (result) {
        sr_dbg("Buffer upload %s for endpoint 0x%02lx, transfer size=%lu",
               enable ? "enabled" : "disabled", endpoint, transferSize);
    } else {
        sr_warn("Failed to set buffer upload for endpoint 0x%02lx", endpoint);
    }
    return result;
}

int ch375_query_buf_upload_ex(unsigned long index, unsigned long endpoint, unsigned long *transferCount, unsigned long *totalDataLen)
{
    if (!pCH375QueryBufUploadEx) {
        sr_err("CH375QueryBufUploadEx not loaded");
        return CH375_FALSE;
    }
    return pCH375QueryBufUploadEx(index, endpoint, transferCount, totalDataLen);
}

int ch375_clear_buf_upload(unsigned long index, unsigned long endpoint)
{
    if (!pCH375ClearBufUpload) {
        return CH375_FALSE;
    }
    return pCH375ClearBufUpload(index, endpoint);
}

/* ============================================================================
 * Buffered download mode
 * ============================================================================ */

int ch375_set_buf_download_ex(unsigned long index, unsigned long enable, unsigned long endpoint, unsigned long transferSize)
{
    if (!pCH375SetBufDownloadEx) {
        sr_err("CH375SetBufDownloadEx not loaded");
        return CH375_FALSE;
    }

    int result = pCH375SetBufDownloadEx(index, enable, endpoint, transferSize);
    if (result) {
        sr_dbg("Buffer download %s for endpoint 0x%02lx, transfer size=%lu",
               enable ? "enabled" : "disabled", endpoint, transferSize);
    } else {
        sr_warn("Failed to set buffer download for endpoint 0x%02lx", endpoint);
    }
    return result;
}

/* ============================================================================
 * IO mode setting
 * ============================================================================ */

// CH375SetIOMode implementation (delegates to CH375DriverCommand)
int ch375_set_io_mode(unsigned long index, unsigned long sync)
{
    if (!pCH375DriverCommand) {
        sr_err("CH375DriverCommand not available");
        return CH375_FALSE;
    }

    mWIN32_COMMAND cmd;
    cmd.mFunction = mFuncSetIOMode;
    cmd.mLength = 1;
    cmd.mBuffer[0] = sync ? 1 : 0;
    return pCH375DriverCommand(index, &cmd);
}

/* ============================================================================
 * Data endpoint read/write
 * ============================================================================ */

int ch375_read_data(unsigned long index, void *buffer, unsigned long *length)
{
    if (!pCH375ReadData) {
        sr_err("CH375ReadData not loaded");
        return CH375_FALSE;
    }
    return pCH375ReadData(index, buffer, length);
}

int ch375_write_data(unsigned long index, void *buffer, unsigned long *length)
{
    if (!pCH375WriteData) {
        sr_err("CH375WriteData not loaded");
        return CH375_FALSE;
    }
    return pCH375WriteData(index, buffer, length);
}

int ch375_abort_read(unsigned long index)
{
    if (!pCH375AbortRead) {
        return CH375_FALSE;
    }
    return pCH375AbortRead(index);
}

int ch375_abort_write(unsigned long index)
{
    if (!pCH375AbortWrite) {
        return CH375_FALSE;
    }
    return pCH375AbortWrite(index);
}

/* ============================================================================
 * Device event notification
 * ============================================================================ */

int ch375_set_device_notify(unsigned long index, char *deviceID, CH375NotifyCallback callback)
{
    if (!pCH375SetDeviceNotify) {
        sr_err("CH375SetDeviceNotify not loaded");
        return CH375_FALSE;
    }

    int result = pCH375SetDeviceNotify(index, deviceID, callback);
    if (result) {
        if (callback) {
            sr_info("Device notify callback registered for index %lu", index);
        } else {
            sr_info("Device notify callback unregistered for index %lu", index);
        }
    } else {
        sr_warn("Failed to set device notify for index %lu", index);
    }
    return result;
}
