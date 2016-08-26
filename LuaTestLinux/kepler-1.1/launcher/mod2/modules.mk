#
# this is used/needed by the APACHE2 build system
#

MOD_LUA = mod_lua apache2_lib compat-5.1

mod_lua.la: ${MOD_LUA:=.slo}
	$(SH_LINK) $(CL_LIBS) -rpath $(libexecdir) -module -avoid-version ${MOD_LUA:=.lo}

DISTCLEAN_TARGETS = modules.mk

shared =  mod_lua.la

