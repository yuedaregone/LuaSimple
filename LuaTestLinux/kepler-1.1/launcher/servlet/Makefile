# $Id: Makefile,v 1.1 2005/03/03 14:42:22 tomas Exp $

T = servlet

include $(CONFIG)

MT= t_$T.lua
ML= $T.lua
T_WEB_XML= WEB-INF/t_web.xml
WEB_XML= WEB-INF/web.xml

SERVLET_SRC= WEB-INF/src
SERVLET_LIB= WEB-INF/lib
SERVLET_BUILD= WEB-INF/classes

JAVAC= $(JAVA_HOME)/bin/javac
JAR= $(JAVA_HOME)/bin/jar

LUAJAVA_JAR= $(SERVLET_LIB)/luajava-1.0b4.jar
SERVLET_JAR= $(CATALINA_HOME)/common/lib/servlet-api.jar
LOG4J_JAR= $(SERVLET_LIB)/log4j-1.2.9.jar
COMPILEOPT= -d $(SERVLET_BUILD) -classpath $(LUAJAVA_JAR):$(SERVLET_JAR):$(LOG4J_JAR)

ALLPACKAGES= org/keplerproject/cgilua/servlet
SUBDIRS= addprefix $(SERVLET_SRC), $(ALLPACKAGES)
SOURCEDIRS= $(SERVLET_SRC)/$(ALLPACKAGES)
SOURCEFILES = $(addsuffix /*.java, $(SOURCEDIRS))
JARSDIR= .
JARNAME = cgilua-$(VERSION).jar

$T: jar $(WEB_XML) $(ML)

build: $T

$(ML): $(MT)
	sed -e "s|\[\[LUA_PATH\]\]|\[\[$(LUA_PATH)\]\]|" -e "s|\[\[LUA_CPATH\]\]|\[\[$(LUA_CPATH)\]\]|" < $(MT) > $(ML)

$(WEB_XML): $(T_WEB_XML)
	sed -e "s|CGILUA_DIR|$(CGILUA_DIR)|" < $(T_WEB_XML) > $@

jar:
	rm -rf $(SERVLET_BUILD)
	mkdir $(SERVLET_BUILD)
	for i in $(SUBDIRS);  \
	do ( $(JAVAC) $(COMPILEOPT) $(SOURCEFILES) ) done
	$(JAR) -cvf $(JARSDIR)/$(JARNAME) -C $(SERVLET_BUILD) .

clean:
	rm -rf $(SERVLET_BUILD)
	rm -f $(JARSDIR)/$(JARNAME)
	rm -f $(WEB_XML)
	rm -f $(ML)
