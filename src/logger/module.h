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
 * @addtogroup logger "Logging module"
 *
 * @{
 *
 * @copydoc doc_logger
 */

#ifndef __WS_LOGGER_MODULE_H__
#define __WS_LOGGER_MODULE_H__

#include <stdarg.h>
#include <pthread.h>
#include <syslog.h>

/**
 * Logging context
 */
struct ws_logger_context {
    char const* prefix;
};

/**
 * Initialize the logger module
 *
 * @return zero on success, else error code from errno.h
 */
int
ws_logger_init(void);

/**
 * Log with a logger
 *
 * @note `ctx` can be NULL
 */
void
ws_log(
    struct ws_logger_context* const ctx, //!< The logging context
    int lvl, //!< Logging level
    char* fmt,  //!< Format string
    ...         //!< Additional parameters
);

/**
 * Log an null-terminated array of strings with a logger
 *
 * @note `ctx` can be NULL
 */
void
ws_log_ary(
    struct ws_logger_context* const ctx, //!< The logging context
    int lvl, //!< Logging level
    char** ary //!< ary to log
);

/*
 * void
 * ws_log_str(
 *     struct ws_string* const str
 * );
 *
 */

#endif // __WS_LOGGER_MODULE_H__

/**
 * @}
 */
