include(tests_common.pri)

pro_file_basename = $$basename(_PRO_FILE_)
pro_file_basename ~= s/\\.pro$//

TEMPLATE = app
TARGET = $${pro_file_basename}.bin

QT += testlib

SOURCES = $${pro_file_basename}.cpp

INCLUDEPATH += ../src
LIBS += -L../src -lmlite$${NODASH_QT_VERSION}

target.path = $${INSTALL_TESTDIR}
INSTALLS += target

wrapper.files = $${pro_file_basename}
wrapper.path = $${INSTALL_TESTDIR}
INSTALLS += wrapper

check.depends = all
check.commands = '\
    cd "$${OUT_PWD}" \
    && export LD_LIBRARY_PATH="$${OUT_PWD}/../src:\$\${LD_LIBRARY_PATH}" \
    && ./$${TARGET}'
check.CONFIG = phony
QMAKE_EXTRA_TARGETS += check
