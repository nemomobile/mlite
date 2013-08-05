include(testapplication.pri)

QT += dbus

check.commands = '\
    cd "$${OUT_PWD}" \
    && export LD_LIBRARY_PATH="$${OUT_PWD}/../src:\$\${LD_LIBRARY_PATH}" \
    && dbus-launch ./$${TARGET}'
