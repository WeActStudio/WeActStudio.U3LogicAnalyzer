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

/* Windows API headers (CH375DLL is a Windows-only library) */
#ifdef _WIN32
#include <windows.h>
#include <setupapi.h>
#include <devguid.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#else
#error "ch375_wrapper.c only supports Windows (CH375DLL is a Windows-only library)"
#endif

#define LOG_PREFIX "ch375"

/* ============================================================================
 * DLL handle and function pointers
 * ============================================================================ */

static HMODULE ch375_dll_handle = NULL;

/* Serializes loading and unloading the DLL */
G_LOCK_DEFINE_STATIC(ch375_dll);

/*
 * Number of devices currently opened through ch375_open_device(). The DLL is
 * kept loaded while this is non-zero: ch375_cleanup() is also called from the
 * GUI thread, and unloading the DLL while libsigrok still holds device
 * handles would leave those handles dangling.
 */
static gint ch375_open_count = 0;

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

/*
 * Unload the DLL and drop every function pointer.
 * The caller must hold ch375_dll.
 */
static void ch375_unload_locked(void)
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

/*
 * Load the DLL and populate the function pointer table.
 * The caller must hold ch375_dll.
 */
static int ch375_load_locked(void)
{
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
    LOAD_FUNC_OPTIONAL(CH375WriteRead);    /* only ch375_write_read() uses it */
    /* ch32h417_send_command() cannot work without this one */
    LOAD_FUNC(CH375WriteEndP);
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
    LOAD_FUNC_OPTIONAL(CH375SetDeviceNotify);
    
    unsigned long version = ch375_get_version();
    sr_info("CH375DLL loaded successfully, version: %08x", version);

    return SR_OK;
}

int ch375_init(void)
{
    int ret;

    G_LOCK(ch375_dll);

    if (ch375_dll_handle != NULL) {
        /* Already loaded */
        G_UNLOCK(ch375_dll);
        return SR_OK;
    }

    ret = ch375_load_locked();
    if (ret != SR_OK) {
        /* Do not leave the DLL mapped with a half-populated function table */
        ch375_unload_locked();
        G_UNLOCK(ch375_dll);
        return ret;
    }

    G_UNLOCK(ch375_dll);

    return SR_OK;
}

void ch375_cleanup(void)
{
    G_LOCK(ch375_dll);

    if (ch375_dll_handle == NULL) {
        G_UNLOCK(ch375_dll);
        return;
    }

    if (g_atomic_int_get(&ch375_open_count) > 0) {
        sr_warn("CH375DLL still has %d open device(s), keeping it loaded",
                g_atomic_int_get(&ch375_open_count));
        G_UNLOCK(ch375_dll);
        return;
    }

    ch375_unload_locked();

    G_UNLOCK(ch375_dll);
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

    g_atomic_int_inc(&ch375_open_count);

    sr_dbg("Opened device at index %lu", index);
    return handle;
}

void ch375_close_device(unsigned long index)
{
    if (pCH375CloseDevice) {
        pCH375CloseDevice(index);
        if (g_atomic_int_get(&ch375_open_count) > 0)
            g_atomic_int_dec_and_test(&ch375_open_count);
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

/* "USB\VID_1A86&PID_5537\<instance>" against the "vid_1a86&pid_5537" piece of
 * the interface name the DLL handed out. */
static int same_vid_pid(const char *devnode_id, const char *idsig)
{
    if (idsig[0] == '\0')
        return 1;

    const char *first = strchr(devnode_id, '\\');
    const char *second = first ? strchr(first + 1, '\\') : NULL;
    if (!first || !second)
        return 0;

    const size_t len = (size_t)(second - first - 1);
    if (len == 0 || len >= 64)
        return 0;

    char vid_pid[64];
    memcpy(vid_pid, first + 1, len);
    vid_pid[len] = '\0';

    return _stricmp(vid_pid, idsig) == 0;
}

int ch375_get_usb_path(unsigned long index, char *path, int path_len)
{
    if (!pCH375GetDeviceName) {
        return -1;
    }

    /*
     * The DLL knows a device by its index, which is only the order it
     * enumerated them in - and that order moves as soon as another board
     * is unplugged. The name it hands out for the index is the Windows
     * device path, and its instance id is the very one the enumeration
     * below lists, which is what ties the device to the socket it sits in.
     */
    const char *device_name = (const char*)pCH375GetDeviceName(index);
    if (!device_name) {
        return -1;
    }

    /*
     * The path is an interface name: "\\?\usb#vid_1d50&pid_608c#<instance>
     * #{...}". Only the piece between the second and the third '#' is the
     * instance id; the rest - the prefix and the interface class GUID at
     * the end - is not part of what the enumeration reports.
     */
    char instance[512];
    char idsig[64] = { 0 };
    {
        const char *p1 = strchr(device_name, '#');
        const char *p2 = p1 ? strchr(p1 + 1, '#') : NULL;
        const char *p3 = p2 ? strchr(p2 + 1, '#') : NULL;
        if (!p2)
            return -1;

        const size_t idlen = (size_t)(p2 - p1 - 1);
        if (idlen > 0 && idlen < sizeof(idsig)) {
            memcpy(idsig, p1 + 1, idlen);
            idsig[idlen] = '\0';
        }

        p2++;
        const size_t len = p3 ? (size_t)(p3 - p2) : strlen(p2);
        if (len == 0 || len >= sizeof(instance))
            return -1;

        memcpy(instance, p2, len);
        instance[len] = '\0';
    }

    sr_dbg("index %lu name '%s'", index, device_name);
    sr_dbg("  instance '%s'", instance);

    HDEVINFO devs = SetupDiGetClassDevsA(NULL, NULL, NULL,
        DIGCF_ALLCLASSES | DIGCF_PRESENT);
    if (devs == INVALID_HANDLE_VALUE) {
        sr_dbg("  SetupDiGetClassDevs failed");
        return -1;
    }

    int hub = -1;
    int port = -1;
    SP_DEVINFO_DATA info;
    info.cbSize = sizeof(info);
    for (DWORD i = 0; SetupDiEnumDeviceInfo(devs, i, &info); i++) {
        char id[512];
        if (!SetupDiGetDeviceInstanceIdA(devs, &info, id, sizeof(id), NULL))
            continue;

        /* The id is "USB\VID_xxxx&PID_xxxx\<instance>" - the instance is
         * the last piece, and the only one the DLL reported. */
        const char *tail = strrchr(id, '\\');
        tail = tail ? tail + 1 : id;
        if (_stricmp(tail, instance) != 0)
            continue;

        /*
         * The serial alone is not enough: in its IAP bootloader the same board
         * is a HID (VID_1A86&PID_FE17) with the same serial, and that devnode
         * sits on another port. Matching it names the board by a socket it is
         * not in, and the page then holds an identity nothing can find again.
         */
        if (!same_vid_pid(id, idsig))
            continue;

        sr_dbg("  matched devnode '%s'", id);

        /*
         * "Port_#0003.Hub_#0004" - the socket on the hub the device is
         * plugged into, and the hub itself: together they are the place
         * on the bus, which is what libsigrok names a USB device by.
         */
        char loc[512];
        if (SetupDiGetDeviceRegistryPropertyA(devs, &info,
                SPDRP_LOCATION_INFORMATION, NULL, (PBYTE)loc, sizeof(loc), NULL)) {
            sr_dbg("  location '%s'", loc);
            const char *tag = strstr(loc, "Port_#");
            if (tag)
                port = atoi(tag + 6);
            tag = strstr(loc, "Hub_#");
            if (tag)
                hub = atoi(tag + 5);
        } else {
            sr_dbg("  devnode has no location information");
        }
        break;
    }

    SetupDiDestroyDeviceInfoList(devs);

    /*
     * A board that reports no location still says where it is when Windows
     * made the instance id up ("5&2f6a1b5&0&3"): the port number is the
     * last piece of it.
     */
    if (port <= 0) {
        const char *tail = strrchr(instance, '&');
        tail = tail ? tail + 1 : instance;
        if (*tail >= '0' && *tail <= '9') {
            port = atoi(tail);
            sr_dbg("  port %d taken from the instance id", port);
        }
    }

    if (port <= 0) {
        sr_dbg("  no location for instance '%s'", instance);
        return -1;
    }

    /* "ch375/<hub>-<port>" - the shape a USB driver names a device by,
     * with the transport it was reached over in front of it. */
    snprintf(path, path_len, "ch375/%d-%d", (hub > 0) ? hub : 0, port);
    sr_dbg("index %lu -> '%s'", index, path);

    return 0;
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
