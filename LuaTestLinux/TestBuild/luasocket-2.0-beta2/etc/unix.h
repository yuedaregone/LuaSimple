#ifndef UNIX_H
#define UNIX_H
/*=========================================================================*\
* Unix domain object
* LuaSocket toolkit
*
* This module is just an example of how to extend LuaSocket with a new 
* domain.
*
* RCS ID: $Id: unix.h,v 1.6 2004/06/22 16:12:53 diego Exp $
\*=========================================================================*/
#include <lua.h>

#include "buffer.h"
#include "timeout.h"
#include "socket.h"

typedef struct t_unix_ {
    t_sock sock;
    t_io io;
    t_buf buf;
    t_tm tm;
} t_unix;
typedef t_unix *p_unix;

int unix_open(lua_State *L);

#endif /* UNIX_H */
