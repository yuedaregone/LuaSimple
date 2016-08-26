# $Id: Makefile,v 1.6 2005/07/22 19:27:18 tomas Exp $

CONFIG= ../config

include $(CONFIG)


build install:
	@echo $@ing $(CGILUA_LAUNCHER)
	cd $(CGILUA_LAUNCHER); export CONFIG="../$(CONFIG)" COMPAT_DIR="../$(COMPAT_DIR)"; make -e $@

clean:
	cd cgi; export CONFIG="../$(CONFIG)" COMPAT_DIR="../$(COMPAT_DIR)"; make -e $@
	cd fastcgi; export CONFIG="../$(CONFIG)" COMPAT_DIR="../$(COMPAT_DIR)"; make -e $@
	cd isapi; export CONFIG="../$(CONFIG)" COMPAT_DIR="../$(COMPAT_DIR)"; make -e $@
	cd mod2; export CONFIG="../$(CONFIG)" COMPAT_DIR="../$(COMPAT_DIR)"; make -e $@
	cd servlet; export CONFIG="../$(CONFIG)" COMPAT_DIR="../$(COMPAT_DIR)"; make -e $@
	cd xavante; export CONFIG="../$(CONFIG)" COMPAT_DIR="../$(COMPAT_DIR)"; make -e $@
