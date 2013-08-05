isEmpty(COMMON_PRI_INCLUDED) {
COMMON_PRI_INCLUDED = 1

# Creates Makefile target to substitute all occurencies of @VARIABLE@ in the given file.
# To substitute a VARIABLE, add it to the CONFIG_SUBST list.
defineTest(configure) {
    in_file = $$1
    target_file = $$basename(in_file)
    target_file ~= s/\\.in$//
    target = configure_$${target_file}
    target ~= s/[^a-zA-Z0-9]/_/g

    eval($${target}.target = $${target_file})
    eval($${target}.depends = $${in_file} $(MAKEFILE_LIST))
    eval($${target}.commands = sed)
    for(v, CONFIG_SUBST) {
        eval($${target}.commands += '-e \\\"s!@$${v}@!\$${$${v}}!g\\\"')
    }
    eval($${target}.commands += '<$${in_file} >$${target_file} || { rm -f $${target_file}; exit 1; }')
    QMAKE_EXTRA_TARGETS += $${target}

    equals(TEMPLATE, subdirs) {
        equals(QT_MAJOR_VERSION, 4): first = make_default
        equals(QT_MAJOR_VERSION, 5): first = make_first
    } else {
        first = all
    }

    eval($${first}.depends += $${target})
    eval($${first}.CONFIG = phony)
    QMAKE_EXTRA_TARGETS += $${first}

    export($${target}.target)
    export($${target}.depends)
    export($${target}.commands)
    export($${first}.depends)
    export($${first}.CONFIG)
    export(QMAKE_EXTRA_TARGETS)
}

# Use the following variables to differentiate build against Qt 4.x vs Qt 5.x
# Example:
#       TARGET = foo$${DASH_QT_VERSION}
#       LIBS += -lbar$${DASH_QT_VERSION}
equals(QT_MAJOR_VERSION, 4): DASH_QT_VERSION = ""
equals(QT_MAJOR_VERSION, 5): DASH_QT_VERSION = "-qt5"
CONFIG_SUBST += DASH_QT_VERSION
equals(QT_MAJOR_VERSION, 4): NODASH_QT_VERSION = ""
equals(QT_MAJOR_VERSION, 5): NODASH_QT_VERSION = "5"
CONFIG_SUBST += NODASH_QT_VERSION

}
