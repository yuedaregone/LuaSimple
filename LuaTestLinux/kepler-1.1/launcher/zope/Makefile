# $Id: Makefile,v 1.1 2005/03/08 19:48:14 mascarenhas Exp $

T= zope

include $(CONFIG)

CT= t_$T.lua
CL= $T.lua

build: $CL

$(CL): $(CT)
	sed -e "s|\[\[LUA_PATH\]\]|\[\[$(LUA_PATH)\]\]|" -e "s|\[\[LUA_CPATH\]\]|\[\[$(LUA_CPATH)\]\]|" $(CT) > $(CL)

clean:
	rm -f $T $(CL) 
