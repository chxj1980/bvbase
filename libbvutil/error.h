/*
 * This file is part of BVbase.
 *
 * BVbase is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * BVbase is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with BVbase; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file
 * error code definitions
 */

#ifndef BVUTIL_ERROR_H
#define BVUTIL_ERROR_H

#include <errno.h>
#include <stddef.h>

/**
 * @addtogroup lavu_error
 *
 * @{
 */


/* error handling */
#if EDOM > 0
#define BVERROR(e) (-(e))   ///< Returns a negative error code from a POSIX error code, to return from library functions.
#define AVUNERROR(e) (-(e)) ///< Returns a POSIX error code from a library function error return value.
#else
/* Some platforms have E* and errno already negated. */
#define BVERROR(e) (e)
#define AVUNERROR(e) (e)
#endif

#define FFERRTAG(a, b, c, d) (-(int)MKTAG(a, b, c, d))

#define BVERROR_BSF_NOT_FOUND      FFERRTAG(0xF8,'B','S','F') ///< Bitstream filter not found
#define BVERROR_BUG                FFERRTAG( 'B','U','G','!') ///< Internal bug, also see BVERROR_BUG2
#define BVERROR_BUFFER_TOO_SMALL   FFERRTAG( 'B','U','F','S') ///< Buffer too small
#define BVERROR_DECODER_NOT_FOUND  FFERRTAG(0xF8,'D','E','C') ///< Decoder not found
#define BVERROR_DEMUXER_NOT_FOUND  FFERRTAG(0xF8,'D','E','M') ///< Demuxer not found
#define BVERROR_ENCODER_NOT_FOUND  FFERRTAG(0xF8,'E','N','C') ///< Encoder not found
#define BVERROR_EOF                FFERRTAG( 'E','O','F',' ') ///< End of file
#define BVERROR_EXIT               FFERRTAG( 'E','X','I','T') ///< Immediate exit was requested; the called function should not be restarted
#define BVERROR_EXTERNAL           FFERRTAG( 'E','X','T',' ') ///< Generic error in an external library
#define BVERROR_FILTER_NOT_FOUND   FFERRTAG(0xF8,'F','I','L') ///< Filter not found
#define BVERROR_INVALIDDATA        FFERRTAG( 'I','N','D','A') ///< Invalid data found when processing input
#define BVERROR_MUXER_NOT_FOUND    FFERRTAG(0xF8,'M','U','X') ///< Muxer not found
#define BVERROR_OPTION_NOT_FOUND   FFERRTAG(0xF8,'O','P','T') ///< Option not found
#define BVERROR_PATCHWELCOME       FFERRTAG( 'P','A','W','E') ///< Not yet implemented in BVbase, patches welcome
#define BVERROR_PROTOCOL_NOT_FOUND FFERRTAG(0xF8,'P','R','O') ///< Protocol not found

#define BVERROR_STREAM_NOT_FOUND   FFERRTAG(0xF8,'S','T','R') ///< Stream not found
/**
 * This is semantically identical to BVERROR_BUG
 * it has been introduced in Libav after our BVERROR_BUG and with a modified value.
 */
#define BVERROR_BUG2               FFERRTAG( 'B','U','G',' ')
#define BVERROR_UNKNOWN            FFERRTAG( 'U','N','K','N') ///< Unknown error, typically from an external library
#define BVERROR_EXPERIMENTAL       (-0x2bb2afa8) ///< Requested feature is flagged experimental. Set strict_std_compliance if you really want to use it.
#define BVERROR_INPUT_CHANGED      (-0x636e6701) ///< Input changed between calls. Reconfiguration is required. (can be OR-ed with BVERROR_OUTPUT_CHANGED)
#define BVERROR_OUTPUT_CHANGED     (-0x636e6702) ///< Output changed between calls. Reconfiguration is required. (can be OR-ed with BVERROR_INPUT_CHANGED)
/* HTTP & RTSP errors */
#define BVERROR_HTTP_BAD_REQUEST   FFERRTAG(0xF8,'4','0','0')
#define BVERROR_HTTP_UNAUTHORIZED  FFERRTAG(0xF8,'4','0','1')
#define BVERROR_HTTP_FORBIDDEN     FFERRTAG(0xF8,'4','0','3')
#define BVERROR_HTTP_NOT_FOUND     FFERRTAG(0xF8,'4','0','4')
#define BVERROR_HTTP_OTHER_4XX     FFERRTAG(0xF8,'4','X','X')
#define BVERROR_HTTP_SERVER_ERROR  FFERRTAG(0xF8,'5','X','X')

#define BV_ERROR_MAX_STRING_SIZE 64

/**
 * Put a description of the BVERROR code errnum in errbuf.
 * In case of failure the global variable errno is set to indicate the
 * error. Even in case of failure bv_strerror() will print a generic
 * error message indicating the errnum provided to errbuf.
 *
 * @param errnum      error code to describe
 * @param errbuf      buffer to which description is written
 * @param errbuf_size the size in bytes of errbuf
 * @return 0 on success, a negative value if a description for errnum
 * cannot be found
 */
int bv_strerror(int errnum, char *errbuf, size_t errbuf_size);

/**
 * Fill the provided buffer with a string containing an error string
 * corresponding to the BVERROR code errnum.
 *
 * @param errbuf         a buffer
 * @param errbuf_size    size in bytes of errbuf
 * @param errnum         error code to describe
 * @return the buffer in input, filled with the error description
 * @see bv_strerror()
 */
static inline char *bv_make_error_string(char *errbuf, size_t errbuf_size, int errnum)
{
    bv_strerror(errnum, errbuf, errbuf_size);
    return errbuf;
}

/**
 * Convenience macro, the return value should be used only directly in
 * function arguments but never stand-alone.
 */
#define bv_err2str(errnum) \
    bv_make_error_string((char[BV_ERROR_MAX_STRING_SIZE]){0}, BV_ERROR_MAX_STRING_SIZE, errnum)

/**
 * @}
 */

#endif /* BVUTIL_ERROR_H */
