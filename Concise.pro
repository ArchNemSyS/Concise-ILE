QT += core network sql
QT -= gui
DEFINES *= QT_USE_QSTRINGBUILDER

INCLUDEPATH += $$_PRO_FILE_PWD_
include(../QtWebApp/httpserver/httpserver.pri)
include(../QtWebApp/templateengine/templateengine.pri)
include(templates/preprocess.pri)


QMAKE_CXXFLAGS += -lsodium
QMAKE_LFLAGS += -lsodium

TARGET = Concise
CONFIG += console
CONFIG -= app_bundle

CONFIG(release, debug|release) {
    #release build
    DEFINES += QT_NO_DEBUG_OUTPUT
} else {
    #debug build
}

TEMPLATE = app

SOURCES += src/main.cpp \
    src/concisedb.cpp \
    src/requestmapper.cpp \
    src/logincontroller.cpp \
    src/studenttemplates.cpp \
    src/stafftemplates.cpp

OTHER_FILES += etc/Concise.ini \
            etc/init.sql \
    docroot/files/css/splash.css \
    docroot/files/css/stylesheet.css \
    docroot/files/index.html \


HEADERS += \
    src/qstdout.h \
    src/concisedb.h \
    src/logincontroller.h \
    src/studenttemplates.h \
    src/stafftemplates.h \
    src/requestmapper.h

#copydata.commands += $(MKDIR) $$top_builddir/docroot/staff && $(MKDIR) $$top_builddir/docroot/student
copydata.commands += $(COPY_DIR) $$top_srcdir/docroot $$top_builddir
#copydata.commands += && $(COPY_DIR) $$top_srcdir/docroot/staff $$top_builddir/docroot/staff
#copydata.commands += && $(COPY_DIR) $$top_srcdir/docroot/student $$top_builddir/docroot/student
first.depends = $(first) copydata
export(first.depends)
export(copydata.commands)

QMAKE_EXTRA_TARGETS += first copydata


DISTFILES += \
    etc/eff_short_wordlist_1.txt \
    docroot/files/js/passgen.js \
    docroot/files/js/seedrandom.min.js






