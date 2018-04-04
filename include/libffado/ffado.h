/* ffado.h
 *
 * Copyright (C) 2005-2008 by Pieter Palmers
 * Copyright (C) 2005-2008 by Daniel Wagner
 *
 * This file is part of FFADO
 * FFADO = Free Firewire (pro-)audio drivers for linux
 *
 * FFADO is based upon FreeBoB
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef FFADO_H
#define FFADO_H

#define FFADO_API_VERSION 9

#define FFADO_MAX_NAME_LEN 256

#include <stdlib.h>

#define FFADO_STREAMING_MAX_URL_LENGTH 2048

#define FFADO_IGNORE_CAPTURE         (1<<0)
#define FFADO_IGNORE_PLAYBACK     (1<<1)

enum ffado_direction {
    FFADO_CAPTURE  = 0,
    FFADO_PLAYBACK = 1,
};

typedef struct ffado_handle* ffado_handle_t;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __APPLE__
#define WEAK_ATTRIBUTE weak_import
#else
#define WEAK_ATTRIBUTE __weak__
#endif

#ifdef __GNUC__
#define FFADO_WEAK_EXPORT __attribute__((WEAK_ATTRIBUTE))
#else
/* Add support for non-gcc platforms here */
#endif

/* ABI stuff */
const char*
ffado_get_version();

int
ffado_get_api_version();

/* various function */

/* The basic operation of the API is as follows:
 * 
 * ffado_streaming_init()
 * ffado_streaming_start()
 * while(running) {
 *   retval = ffado_streaming_wait();
 *   if (retval == -1) {
 *     ffado_streaming_reset();
 *     continue;
 *   }
 *
 *   ffado_streaming_transfer_capture_buffers(dev);
 *
 *   for(all channels) {
 *     // For both audio and MIDI channels, captured data is available
 *     // in the buffer previously set with a call to
 *     //   ffado_streaming_set_capture_stream_buffer(dev, channel, buffer)
 *     switch (channel_type) {
 *     case audio:
 *       // Process incoming audio as needed
 *     case midi:
 *       // Process incoming MIDI data as needed
 *     }
 *   }
 *
 *   for(all channels) {
 *     // For both audio and MIDI channels, data is written to buffers
 *     // previously associated with the playback channel streams using
 *     //   ffado_streaming_set_playback_stream_buffer(dev, channel, buffer)
 *     switch (channel_type) {
 *     case audio:
 *       // Set audio playback buffer contents
 *     case midi:
 *       // Set MIDI playback buffer contents
 *     }
 *   }
 *   ffado_streaming_transfer_playback_buffers(dev);
 *
 * }
 * ffado_streaming_stop();
 * ffado_streaming_finish();
 *
 */

typedef struct _ffado_device ffado_device_t;

/**
 * The sample format used by the ffado streaming API
 */

typedef unsigned int ffado_sample_t; // FIXME
typedef unsigned int ffado_nframes_t;

#define FFADO_MAX_SPECSTRING_LENGTH 256
#define FFADO_MAX_SPECSTRINGS       64
/**
 * This struct serves to define the devices that should be used by the library
 * device_spec_strings is an array of pointers that should contain nb_device_spec_strings
 * valid pointers to strings. 
 * 
 * The spec strings should be null terminated and can be no longer
 * than FFADO_MAX_SPECSTRINGS.
 *
 * nb_device_spec_strings < FFADO_MAX_SPECSTRING_LENGTH
 * nb_device_spec_strings >= 0
 *
 * If nb_device_spec_strings == 0, all busses are scanned for attached devices, and
 * all found devices that are supported are combined into one large pseudo-device. The
 * device order is defined by the GUID of the device. Devices with lower GUID's will
 * be the first ones.
 *
 * If multiple device specifications are present, the device order is defined as follows:
 *  - device(s) that correspond to a spec string with a lower index will be added before
 *    devices from higher indexes.
 *  - if a spec string results in multiple devices, they are sorted by GUID unless the
 *    spec format dictates otherwise.
 *
 * The actual meaning of the device specification should be one of the following:
 * - Format 1: "hw:x[,y[,z]]"
 *     x = the firewire bus to use ('port' in raw1394 terminology)
 *         (mandatory)
 *     y = the node id the device currently has (bus resets might change that, but FFADO 
 *         will track these changes and keep using the device specified on startup)
 *         (optional)
 *     z = the stream direction to use.
 *           0 => capture (record) channels only
 *           1 => playback channels only
 *           other/unspecified => both playback and capture
 *         (optional)
 *
 * - Format 2: the device alias as defined in the ffado config file (UNIMPLEMENTED)
 */
typedef struct ffado_device_info {
    unsigned int nb_device_spec_strings;
    char **device_spec_strings;

    /* add some extra space to allow for future API extention 
       w/o breaking binary compatibility */
    int32_t reserved[32];
} ffado_device_info_t;

/**
 * Structure to pass the options to the ffado streaming code.
 */
typedef struct ffado_options {
    /* driver related setup */
    int32_t sample_rate;         /*
                             * you can specify a value here or -1 to autodetect
                              */

    /* buffer setup */
    int32_t period_size;     /* one period is the amount of frames that
                 * has to be sent or received in order for
                 * a period boundary to be signalled.
                 * (unit: frames)
                 */
    int32_t nb_buffers;    /* the size of the frame buffer (in periods) */

    /* packetizer thread options */
    int32_t realtime;
    int32_t packetizer_priority;

    /* verbosity */
    int32_t verbose;

    /* slave mode */
    int32_t slave_mode;
    /* snoop mode */
    int32_t snoop_mode;

    /* add some extra space to allow for future API extention 
       w/o breaking binary compatibility */
    int32_t reserved[24];

} ffado_options_t;

/**
 * The types of streams supported by the API
 *
 * A ffado_audio type stream is a stream that consists of successive samples.
 * The format is a 24bit UINT in host byte order, aligned as the 24LSB's of the 
 * 32bit UINT of the read/write buffer.
 * The wait operation looks at this type of streams only.
 *
 * A ffado_midi type stream is a stream of midi bytes. The bytes are 8bit UINT,
 * aligned as the first 8LSB's of the 32bit UINT of the read/write buffer.
 * 
 * A ffado_control type stream is a stream that provides control information. The
 * format of this control information is undefined, and the stream should be ignored.
 *
 */
typedef enum {
      ffado_stream_type_invalid                      =   -1,
      ffado_stream_type_unknown                      =   0,
      ffado_stream_type_audio                        =   1,
      ffado_stream_type_midi                         =   2,
      ffado_stream_type_control                      =   3,
} ffado_streaming_stream_type;

/**
 *
 * Audio data types known to the API
 *
 */
typedef enum {
    ffado_audio_datatype_error           = -1,
    ffado_audio_datatype_int24           =  0,
    ffado_audio_datatype_float           =  1,
} ffado_streaming_audio_datatype;

/**
 *
 * Wait responses
 *
 */
typedef enum {
    ffado_wait_shutdown        = -3,
    ffado_wait_error           = -2,
    ffado_wait_xrun            = -1,
    ffado_wait_ok              =  0,
} ffado_wait_response;

/**
 * Initializes the streaming from/to a FFADO device. A FFADO device
 * is a virtual device composed of several BeBoB or compatible devices,
 * linked together in one sync domain.
 *
 * This prepares all IEEE1394 related stuff and sets up all buffering.
 * It elects a sync master if nescessary.
 * 
 * @param device_info provides a way to specify the virtual device
 * @param options options regarding buffers, ieee1394 setup, ...
 *
 * @return Opaque device handle if successful.  If this is NULL, the
 * init operation failed.
 *
 */
ffado_device_t *ffado_streaming_init(
                     ffado_device_info_t device_info,
                     ffado_options_t options);

/**
 * This permits the setting of the period size at some time after 
 * initialisation.  The primary use of this function is to support the
 * setbufsize functionality of JACK.
 *
 * @param dev the ffado device
 * @param period the new period size
 * @return 0 on success, non-zero if an error occurred
 */
int ffado_streaming_set_period_size(ffado_device_t *dev, 
                     unsigned int period) FFADO_WEAK_EXPORT;

/**
 * preparation should be done after setting all per-stream parameters
 * the way you want them. being buffer data type etc...
 *
 * @param dev the ffado device
 * @return 
 */
int ffado_streaming_prepare(ffado_device_t *dev);


/**
 * Finishes the FFADO streaming. Cleans up all internal data structures
 * and terminates connections.
 *
 * @param dev the ffado device to be closed.
 */
void ffado_streaming_finish(ffado_device_t *dev);

/**
 * Returns the amount of capture channels available
 *
 * @param dev the ffado device
 *
 * @return the number of capture streams present & active on the device. 
 *         can be 0. returns -1 upon error.
 */
int ffado_streaming_get_nb_capture_streams(ffado_device_t *dev);

/**
 * Returns the amount of playack channels available
 *
 * @param dev the ffado device
 *
 * @return the number of playback streams present & active on the device. 
 *         can be 0. returns -1 upon error.
 */
int ffado_streaming_get_nb_playback_streams(ffado_device_t *dev);

/**
 * Copies the capture channel name into the specified buffer
 *
 * @param dev the ffado device
 * @param number the stream number
 * @param buffer the buffer to copy the name into. has to be allocated.
 * @param buffersize the size of the buffer
 *
 * @return the number of characters copied into the buffer
 */
int ffado_streaming_get_capture_stream_name(ffado_device_t *dev, int number, char* buffer, size_t buffersize);

/**
 * Copies the playback channel name into the specified buffer
 *
 * @param dev the ffado device
 * @param number the stream number
 * @param buffer the buffer to copy the name into. has to be allocated.
 * @param buffersize the size of the buffer
 *
 * @return the number of characters copied into the buffer
 */
int ffado_streaming_get_playback_stream_name(ffado_device_t *dev, int number, char* buffer, size_t buffersize);

/**
 * Returns the type of a capture channel
 *
 * @param dev the ffado device
 * @param number the stream number
 *
 * @return the channel type 
 */
ffado_streaming_stream_type ffado_streaming_get_capture_stream_type(ffado_device_t *dev, int number);

/**
 * Returns the type of a playback channel
 *
 * @param dev the ffado device
 * @param number the stream number
 *
 * @return the channel type 
 */
ffado_streaming_stream_type ffado_streaming_get_playback_stream_type(ffado_device_t *dev, int number);
/*
 *
 * Note: buffer handling will change in order to allow setting the sample type for *_read and *_write
 * and separately indicate if you want to use a user buffer or a managed buffer.
 *
 */

/**
 * Sets the decode buffer for the stream. This allows for zero-copy decoding.
 * The call to ffado_streaming_transfer_buffers will decode one period of the stream to
 * this buffer. Make sure it is large enough. 
 * 
 * @param dev the ffado device
 * @param number the stream number
 * @param buff a pointer to the sample buffer, make sure it is large enough 
 *             i.e. sizeof(your_sample_type)*period_size
 * @param t   the type of the buffer. this determines sample type and the decode function used.
 *
 * @return -1 on error, 0 on success
 */

int ffado_streaming_set_capture_stream_buffer(ffado_device_t *dev, int number, char *buff);
int ffado_streaming_capture_stream_onoff(ffado_device_t *dev, int number, int on);

/**
 * Sets the encode buffer for the stream. This allows for zero-copy encoding (directly to the events).
 * The call to ffado_streaming_transfer_buffers will encode one period of the stream from
 * this buffer to the event buffer.
 * 
 * @param dev the ffado device
 * @param number the stream number
 * @param buff a pointer to the sample buffer
 * @param t   the type of the buffer. this determines sample type and the decode function used.
 *
 * @return -1 on error, 0 on success
 */
int ffado_streaming_set_playback_stream_buffer(ffado_device_t *dev, int number, char *buff);
int ffado_streaming_playback_stream_onoff(ffado_device_t *dev, int number, int on);

ffado_streaming_audio_datatype ffado_streaming_get_audio_datatype(ffado_device_t *dev);
int ffado_streaming_set_audio_datatype(ffado_device_t *dev, ffado_streaming_audio_datatype t);

/**
 * preparation should be done after setting all per-stream parameters
 * the way you want them. being buffer data type etc...
 *
 * @param dev 
 * @return 
 */
 
int ffado_streaming_prepare(ffado_device_t *dev);

/**
 * Starts the streaming operation. This initiates the connections to the FFADO devices and
 * starts the packet handling thread(s). This has to be called before any I/O can occur.
 *
 * @param dev the ffado device
 *
 * @return 0 on success, -1 on failure.
 */
int ffado_streaming_start(ffado_device_t *dev);

/**
 * Stops the streaming operation. This closes the connections to the FFADO devices and
 * stops the packet handling thread(s). 
 *
 * @param dev the ffado device
 *
 * @return 0 on success, -1 on failure.
 */
int ffado_streaming_stop(ffado_device_t *dev);

/**
 * Resets the streaming as if it was stopped and restarted. The difference is that the connections
 * are not nescessarily broken and restored.
 *
 * All buffers are reset in the initial state and all data in them is lost.
 *
 * @param dev the ffado device
 *
 * @return 0 on success, -1 on failure.
 */
int ffado_streaming_reset(ffado_device_t *dev);

/**
 * Waits until there is at least one period of data available on all capture connections and
 * room for one period of data on all playback connections
 *
 * @param dev the ffado device
 *
 * @return The number of frames ready. -1 when a problem occurred.
 */
ffado_wait_response ffado_streaming_wait(ffado_device_t *dev);

/**
 * Transfer & decode the events from the packet buffer to the sample buffers
 * 
 * This should be called after the wait call returns, before reading/writing the sample buffers
 * with ffado_streaming_[read|write].
 * 
 * The purpose is to allow more precise timing information. ffado_streaming_wait returns as soon as the 
 * period boundary is crossed, and can therefore be used to determine the time instant of this crossing (e.g. jack DLL).
 *
 * The actual decoding work is done in this function and can therefore be omitted in this timing calculation.
 * Note that you HAVE to call this function in order for the buffers not to overflow, and only call it when
 * ffado_streaming_wait doesn't indicate a buffer xrun (xrun handler resets buffer).
 * 
 * If user supplied playback buffers are specified with ffado_streaming_set_playback_buffers
 * their contents should be valid before calling this function.
 * If user supplied capture buffers are specified with ffado_streaming_set_capture_buffers
 * their contents are updated in this function.
 * 
 * Use either ffado_streaming_transfer_buffers to transfer all buffers at once, or use 
 * ffado_streaming_transfer_playback_buffers and ffado_streaming_transfer_capture_buffers 
 * to have more control. Don't use both.
 * 
 * @param dev the ffado device
 * @return  -1 on error.
 */
 
int ffado_streaming_transfer_buffers(ffado_device_t *dev);

/**
 * Transfer & encode the events from the sample buffers to the packet buffer
 * 
 * This should be called after the wait call returns, after writing the sample buffers
 * with ffado_streaming_write.
 * 
 * If user supplied playback buffers are specified with ffado_streaming_set_playback_buffers
 * their contents should be valid before calling this function.
 * 
 * Use either ffado_streaming_transfer_buffers to transfer all buffers at once, or use 
 * ffado_streaming_transfer_playback_buffers and ffado_streaming_transfer_capture_buffers 
 * to have more control. Don't use both.
 * 
 * @param dev the ffado device
 * @return  -1 on error.
 */
 
int ffado_streaming_transfer_playback_buffers(ffado_device_t *dev);

/**
 * Transfer & decode the events from the packet buffer to the sample buffers
 * 
 * This should be called after the wait call returns, before reading the sample buffers
 * with ffado_streaming_read.
 * 
 * If user supplied capture buffers are specified with ffado_streaming_set_capture_buffers
 * their contents are updated in this function.
 * 
 * Use either ffado_streaming_transfer_buffers to transfer all buffers at once, or use 
 * ffado_streaming_transfer_playback_buffers and ffado_streaming_transfer_capture_buffers 
 * to have more control. Don't use both.
 * 
 * @param dev the ffado device
 * @return  -1 on error.
 */

int ffado_streaming_transfer_capture_buffers(ffado_device_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* FFADO_STREAMING */
