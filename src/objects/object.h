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
 *
 * @copydoc doc_object
 */

/**
 * @addtogroup objects_object "Class: object type"
 *
 * @{
 */

#ifndef __WS_OBJECTS_OBJECT_H__
#define __WS_OBJECTS_OBJECT_H__

#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>

#include "util/attributes.h"
#include "logger/module.h"
#include "values/value.h"
#include "values/value_type.h"
#include "command/command.h"

/*
 *
 * Convenience macros
 *
 */
#define getref(obj_) ((__typeof__(obj_)) \
                     ws_object_getref((struct ws_object*) (obj_)))

/*
 * Type names
 */
struct ws_object_type;
struct ws_object;

/**
 * object type identification
 *
 * References to a single ws_object_type. Used for indicating an object type by
 * a single pointer.
 */
typedef struct ws_object_type const ws_object_type_id;

/*
 *
 * Object callback function types
 *
 */

/**
 * Destructor callback
 */
typedef bool (*ws_object_deinit_callback)(struct ws_object* const);

/**
 * hash callback
 */
typedef size_t (*ws_object_hash_callback)(struct ws_object* const);

/**
 * Compare callback
 */
typedef int (*ws_object_cmp_callback)(struct ws_object const*,
                                      struct ws_object const*);

/**
 * UUID creation callback
 */
typedef uintmax_t (*ws_object_uuid_callback)(struct ws_object*);


/*
 *
 * Type implementation
 *
 */

/**
 * Attribute type identifier
 */
enum ws_object_attribute_type {
    WS_OBJ_ATTR_NO_TYPE = 0,

    WS_OBJ_ATTR_TYPE_BOOL,
    WS_OBJ_ATTR_TYPE_CHAR,
    WS_OBJ_ATTR_TYPE_INT32,
    WS_OBJ_ATTR_TYPE_INT64,
    WS_OBJ_ATTR_TYPE_UINT32,
    WS_OBJ_ATTR_TYPE_UINT64,
    WS_OBJ_ATTR_TYPE_STRING,
    WS_OBJ_ATTR_TYPE_OBJ,
};

/**
 * Attribute type
 *
 * For storing information about an attribute of an object
 *
 * @note When specifying the table for a type, this _should_ be NULL termiated
 */
struct ws_object_attribute {
    char const* const   name; //!< Name of the attribute
    size_t              offset_in_struct; //!< Offset in the struct
    enum ws_object_attribute_type type; //!< Attribute type
    enum ws_value_type vtype; //!< Attribute type in ws_value_type shape
};

/**
 * Function type
 *
 * For Storing information about an function callback of an object
 *
 * @note When specifying the table for a type, this _MUST_ be NULL terminated
 * by setting the `name` member and the `func` to NULL.
 *
 * @note The first two parameters of the command function to be called are the
 * object itself, referenced by a ws_value_obj_id and the string which
 * identifies the function itself. When implementing a object callback command,
 * this should be considered and possibly these values on the stack should be
 * skipped.
 */
struct ws_object_function {
    char const* const       name; //!< Name of the function
    ws_regular_command_func func; //!< Function pointer
};

/**
 * Object type identifier for identifiying an object type
 */
struct ws_object_type {
    ws_object_type_id* supertype; //!< supertype. Ref to itself if there is none
    const char* const typestr; //!< string which represents the type

    ws_object_deinit_callback deinit_callback; //!< Free callback for the type
    ws_object_hash_callback hash_callback; //!< Hash callback for the type
    ws_object_cmp_callback cmp_callback; //!< Compare callback for the type
    ws_object_uuid_callback uuid_callback; //!< @protected UUID callback

    struct ws_object_attribute const* attribute_table; //!< Attr table
    struct ws_object_function const* function_table; //!< Func table
};

/**
 * Object settings type
 *
 * For identifying an object "setting"
 */
enum ws_object_settings {
    WS_OBJ_NO_SETTINGS = 0,
    WS_OBJ_CONST            = 1 << 0,
    WS_OBJ_SELF_DESTROYING  = 1 << 1,
    WS_OBJECT_HEAPALLOCED   = 1 << 2,
    WS_OBJECT_LOCKABLE      = 1 << 3,
};

/**
 * Object type
 *
 * The object type is the root class for all classes
 */
struct ws_object {
    ws_object_type_id* id;        //!< Object id, identifies the actual type

    struct {
        pthread_mutex_t lock;
        size_t refcnt;
    } ref_counting; //!< @private Ref counting

    enum ws_object_settings settings; //!< @private Object settings
    pthread_rwlock_t rw_lock; //!< @private Read/Write lock

    uintmax_t uuid; // @protected Unique ID for the object
};

/**
 * Variable which holds type information about the ws_object type
 */
extern ws_object_type_id WS_OBJECT_TYPE_ID_OBJECT;

/**
 * Allocate an object with a given size
 *
 * @memberof ws_object
 *
 * @note One ref on the object is used
 *
 * @return A new ws_object object or NULL on failure
 */
struct ws_object*
ws_object_new(
    size_t
);

/**
 * Allocate a new plain ws_object object.
 *
 * @memberof ws_object
 *
 * @note One ref on the object is used
 *
 * "plain" means that the size of the object is `sizeof(struct ws_object)`
 *
 * @return A new ws_object object or NULL on failure
 */
struct ws_object*
ws_object_new_raw(void);

/**
 * Get the ID of the object
 *
 * @memberof ws_object
 *
 * @return The id of the object or NULL on failure
 */
ws_object_type_id*
ws_object_get_type_id(
    struct ws_object* const self //!< The object
);

/**
 * Get the settings of an object
 *
 * @memberof ws_object
 *
 * @return The settings
 */
enum ws_object_settings
ws_object_get_settings(
    struct ws_object* const self //!< The object
);

/**
 * Set the settings of an object
 *
 * @memberof ws_object
 *
 * @note This should only be done _once_.
 */
void
ws_object_set_settings(
    struct ws_object* self, //!< The object
    enum ws_object_settings //!< The settings to set
);

/**
 * Initialize an object
 *
 * @memberof ws_object
 *
 * @note One ref of the object is used
 *
 * @return true if the object was initialized successfully, else false
 */
bool
ws_object_init(
    struct ws_object* self //!< The object
);

/**
 * Get ref on the object
 *
 * @memberof ws_object
 *
 * @return The object itself or NULL on failure
 */
struct ws_object*
ws_object_getref(
    struct ws_object* self //!< The object
);

/**
 * Unreference an object
 *
 * @memberof ws_object
 *
 * @warning This function must not be called for stack-allocated ws_objects,
 * as freeing an object with zero references will possibly lead to a
 * segmentation fault.
 *
 * @warning It is not save to use the object after this operation _in any kind_.
 * The object might be unavailable after this operation, as it was freed from
 * the heap.
 */
void
ws_object_unref(
    struct ws_object* self //!< The object
);

/**
 * Run the object
 *
 * @memberof ws_object
 *
 * @note returns false also if there is no run callback available
 *
 * @return true if the object is run, else false
 */
bool
ws_object_run(
    struct ws_object* self //!< The object
);

/**
 * Hash the object
 *
 * @memberof ws_object
 *
 * @return the object hash as a `size_t` or negative errno.h number
 */
ssize_t
ws_object_hash(
    struct ws_object* self //!< The object
);

/**
 * Read-lock the object
 *
 * @memberof ws_object
 *
 * @return true if the lock was aquired, false on error
 */
bool
ws_object_lock_read(
    struct ws_object* self //!< The object
);

/**
 * Write-lock the object
 *
 * @memberof ws_object
 *
 * @return true if the lock was aquired, false on error
 */
bool
ws_object_lock_write(
    struct ws_object* self //!< The object
);

/**
 * Try to read-lock the object
 *
 * @memberof ws_object
 *
 * @return zero on success, else a negative errno.h number (from pthread call)
 */
int
ws_object_lock_try_read(
    struct ws_object* self //!< The object to lock
);

/**
 * Try to write-lock the object
 *
 * @memberof ws_object
 *
 * @return zero on success, else a negative errno.h number (from pthread call)
 */
int
ws_object_lock_try_write(
    struct ws_object* self //!< The object to lock
);

/**
 * Unlock the object
 *
 * @memberof ws_object
 *
 * @return true if the lock was unlocked, else false
 */
bool
ws_object_unlock(
    struct ws_object* self //!< The object
);

/**
 * Uninitialize a ws_object
 *
 * @warning It is not save to use the object after this method was called on it.
 *
 * @warning Should only be called for objects which are allocated on the stack.
 */
void
ws_object_deinit(
    struct ws_object* self //!< The object
)
__ws_nonnull__(1)
;

/**
 * Check whether an attribute is available
 *
 * @memberof ws_object
 *
 * @return true when there is such an attribute within the object, else false
 */
bool
ws_object_has_attr(
    struct ws_object* self, //!< The object
    char const* ident //!< The identifier for the attribute
);

/**
 * Get an attribute of an object
 *
 * @memberof ws_object
 *
 * How this works:
 *
 * Each object type has to embedd a attribute table into its type information
 * table (@see ws_object_attribute, @see ws_object_type). Each entry in this
 * table has three members:
 *
 * - The name of the attribute, so how it can be addressed/referenced. This can
 *   be a name of a member, but also something else, so forward-compatibility
 *   is granted.
 * - The offset in the struct definition. This one is compiletime generated and
 *   simply stores the offset of the member to reference, E.G:
 *      @code{.c}
 *      struct a {
 *         int b;
 *         int c;
 *      };
 *      @endcode
 *   So, `c` has the offset `sizeof(b) == sizeof(int)`. This is neccessary to
 *   get a pointer on the actual member and retreiving the data from it.
 * - The third member is the actual type information about the attribute to
 *   read. According to this, and an internal table, the type of the destination
 *   `ws_value` subtype is decided.
 *
 * The overall approach of the method is simple: Find the entry in the table
 * which matches the specified identifier. If the type of the member can be
 * parsed into the type of the passed ws_value type, get the data from the
 * object and put it into the `dest` ws_value type.
 *
 * @note Does string-match the attribute name with the `ident` parameter in O(n)
 *
 * @return zero on success, else negative error code from errno.h
 *      -EINVAL - if the passed `dest` type does not match the attribute type
 *      -EINVAL - if there is no attribute table
 *      -ECANCELED - if the requested member is not available
 *      -EFAULT - If the member has no type (unlikely)
 *      -ENOMEM - failed to allocate resources
 */
int
ws_object_attr_read(
    struct ws_object* self, //!< The object
    char const* ident, //!< The identifier for the attribute
    struct ws_value* dest //!< Destination of the data
)
__ws_nonnull__(1, 2, 3)
;

/**
 * Write an attribute of an object
 *
 * @memberof wes_object
 *
 * How it works: \see ws_object_attr_read
 *
 * @return zero on success, else negative errno.h number
 */
int
ws_object_attr_write(
    struct ws_object* self, //!< The object
    char const* ident, //!< The identifier for the attribute
    struct ws_value* src //!< Source of the data
)
__ws_nonnull__(1, 2, 3)
;

/**
 * Get the type of an attribute identified by its name
 *
 * @memberof ws_object
 *
 * @return The attribute type of the attribute identified by name or
 * WS_OBJ_ATTR_NO_TYPE
 */
enum ws_object_attribute_type
ws_object_attr_type(
    struct ws_object* self, //!< The object
    char* ident //!< The identifier for the attribute
)
__ws_nonnull__(1, 2)
;


/**
 * Get the value-type of an attribute identified by its name
 *
 * @memberof ws_object
 *
 * @return The value-attribute type of the attribute identified by name or
 * WS_VALUE_TYPE_NONE
 */
enum ws_value_type
ws_object_attr_value_type(
    struct ws_object* self, //!< The object
    char* ident //!< The identifier for the attribute
)
__ws_nonnull__(1, 2)
;


/**
 * Compare two ws_object instances
 *
 * @memberof ws_object
 *
 * @note If either of the arguments is `NULL`, the other is defined to be the
 * greater one.
 *
 * @warning If the types of the arguments `o1` and `o2` differ, the return value
 * is undefined.
 *
 * @warning If the type has no compare callback, the return value is undefined.
 *
 * @note Aquires read-locks on both objects.
 *
 * @return -1 if `o1` is bigger, zero if they are equal, 1 if `o2` is bigger
 */
int
ws_object_cmp(
    struct ws_object const* o1, //!< The first operant of the comparison
    struct ws_object const* o2 //!< The second operant of the comparison
);

/**
 * Get an UUID for an object
 *
 * This method creates the UUID for the object if there is no uuid available for
 * it.
 *
 * @memberof ws_object
 *
 * @return The UUID of the object
 */
uintmax_t
ws_object_uuid(
    struct ws_object const* self //!< The object
);

/**
 * Check whether an object if of a specific type
 *
 * @note an object is also an instance of a type if it's really an instance
 *       of a _subtype_.
 *
 * @memberof ws_object
 *
 * @return true if the object is of the type supplied, false if it is not
 */
bool
ws_object_is_instance_of(
    struct ws_object const* self, //!< The object
    ws_object_type_id* type //!< The type to check against
)
__ws_nonnull__(1)
;

/**
 * Check whether an object is a type by comparing type names
 *
 * @memberof ws_object
 *
 * @return true if object is of type, else false
 */
bool
ws_object_has_typename(
    struct ws_object* self, //!< The object
    const char* type_name //!< The typename
)
__ws_nonnull__(1, 2)
;

/*
 *
 * Forward declaration
 *
 */
union ws_value_union;

/**
 * Call a command on the object
 *
 * @memberof ws_object
 *
 * @return return value of command function or negative errno.h number:
 *  -ENOENT if no command was found
 *  -EINVAL if arguments where invalid
 */
int
ws_object_call_cmd(
    struct ws_object* self, //!< The object
    char const* ident, //!< Name of the command to call for the object
    union ws_value_union* stack //!< Stack to pass to the command function
);

/**
 * Check whether the object has a command callback with a certain name
 *
 * @memberof ws_object
 *
 * @return true if the command is available, else false.
 */
bool
ws_object_has_cmd(
    struct ws_object* self, //!< The object
    char const* ident //!< Name of command to check the object for
);

/**
 * Get the typename of the object
 *
 * @memberof ws_object
 *
 * @return Typename of passed object or NULL on failure
 */
const char*
ws_object_typename(
    struct ws_object const* self //!< The object
);

#endif // __WS_OBJECTS_OBJECT_H__

/**
 * @}
 */

/**
 * @}
 */
