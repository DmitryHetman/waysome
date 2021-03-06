/*
 * waysome - wayland based window manager
 *
 * Copyright in alphabetical order:
 *
 * Copyright (C) 2014-2015 Julian Ganz
 * Copyright (C) 2014-2015 Manuel Messner
 * Copyright (C) 2014-2015 Marcel Müller
 * Copyright (C) 2014-2015 Matthias Beyer
 * Copyright (C) 2014-2015 Nadja Sommerfeld
 *
 * This file is part of waysome.
 *
 * waysome is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation, either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * waysome is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with waysome. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @addtogroup objects "Classes"
 *
 * @{
 */

/**
 * @addtogroup objects_messages "Message classes"
 *
 * @{
 */

#ifndef __WS_OBJECTS_ERROR_REPLY_H__
#define __WS_OBJECTS_ERROR_REPLY_H__

#include "objects/message/reply.h"


// forward declarations
struct ws_transaction;


/**
 * Error reply message type
 *
 * This reply message type is intended for communication of errors back to the
 * issuer of a transaction.
 *
 * @extends ws_reply
 */
struct ws_error_reply {
    struct ws_reply reply; //!< @protected base class
    unsigned int code; //!< @protected error code (e.g. an errno.h value)
    char const* description; //!< @protected error description
    char const* cause; //!< @protected cause of the error
};

/**
 * Variable which holds the type information about the ws_error_reply type
 */
extern ws_object_type_id WS_OBJECT_TYPE_ID_ERROR_REPLY;

/**
 * Create a new error reply message
 *
 * This function creates a new error reply message.
 *
 * @return a newly created error reply message or `NULL`, if an error occurred
 */
struct ws_error_reply*
ws_error_reply_new(
    struct ws_transaction* src, //!< transaction for which this is the reply
    unsigned int code, //!< error code
    char const* description, //!< description of the error (will be copied)
    char const* cause //!< cause of the error (will be copied)
)
__ws_nonnull__(1)
;

/**
 * Get the error code embedded in an error reply
 *
 * @return the error code
 */
unsigned int
ws_error_reply_get_code(
    struct ws_error_reply* self //!< error reply from which to extract the code
)
__ws_nonnull__(1)
;

/**
 * Get the error description embedded in an error reply
 *
 * @note always returns a valid string, "" if it doesn't contain one
 *
 * @warning you must not `free()` any string returned by this function
 *
 * @return the error description
 */
char const*
ws_error_reply_get_description(
    struct ws_error_reply* self //!< error reply from which to extract the code
)
__ws_nonnull__(1)
;

/**
 * Get the cause of the error embedded in an error reply
 *
 * @note always returns a valid string, "" if it doesn't contain one
 *
 * @warning you must not `free()` any string returned by this function
 *
 * @return cause of the error
 */
char const*
ws_error_reply_get_cause(
    struct ws_error_reply* self //!< error reply from which to extract the code
)
__ws_nonnull__(1)
;

#endif // __WS_OBJECTS_ERROR_REPLY_H__

/**
 * @}
 */

/**
 * @}
 */

