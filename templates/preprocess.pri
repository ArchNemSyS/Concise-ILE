INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD



CONFIG(debug, debug|release) {
    #DEFINES += SUPERVERBOSE
}

#HEADERS += $$top_srcdir/docroot/include/*.htm
HEADERS += $$PWD/include/*.htm

# MINGW cpp may not work ?

SOURCES_HTM_STAFF += $$PWD/*.htm
htm_staff.name = htm
htm_staff.input = SOURCES_HTM_STAFF
htm_staff.dependency_type = TYPE_C
htm_staff.variable_out = JUNK
htm_staff.output = $$shadowed($$PWD)/${QMAKE_FILE_IN_BASE}.html
htm_staff.CONFIG = no_link target_predeps
htm_staff.commands = cpp -D__BUILD_DATE__='"$(shell date +'%Y')"'  -E -x c -P ${QMAKE_FILE_NAME} > ${QMAKE_FILE_OUT}
QMAKE_EXTRA_COMPILERS += htm_staff

DISTFILES += \
    $$PWD/include/head_login.htm \
    $$PWD/reset.htm






