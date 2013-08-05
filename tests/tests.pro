include(tests_common.pri)

TEMPLATE = subdirs
SUBDIRS = \
        ut_mdesktopentry.pro \
        ut_mfiledatastore.pro \
        ut_mgconfitem.pro \
        ut_mnotification.pro \
        ut_mremoteaction.pro \

configure($${PWD}/tests.xml.in)
tests_xml.path = $${INSTALL_TESTDIR}
tests_xml.files = tests.xml
tests_xml.CONFIG = no_check_exist
INSTALLS += tests_xml
