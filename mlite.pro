TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = \
        src \
        tests \

# Adds 'coverage' make target
include(coverage.pri)
