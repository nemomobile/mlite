include(../common.pri)

isEmpty(VERSION) {
    GIT_TAG = $$system(git describe --tags --abbrev=0)
    GIT_VERSION = $$find(GIT_TAG, ^\\d+(\\.\\d+)?(\\.\\d+)?$)
    isEmpty(GIT_VERSION) {
        # if you're trying to build this from a tarball, I'm sorry. but being
        # able to specify the version in just one place (git tags) is a lot less
        # error-prone and easy.
        warning("Can't find a valid git tag version, got: $$GIT_TAG")
        GIT_VERSION = 0.0.0
    }
    !isEmpty(GIT_VERSION): VERSION = $$GIT_VERSION
}
CONFIG_SUBST += VERSION

QT     = core dbus
TARGET = $$qtLibraryTarget(mlite$${NODASH_QT_VERSION})
TEMPLATE = lib

CONFIG += link_pkgconfig
packagesExist(gconf-2.0) {
    PKGCONFIG += gconf-2.0
    DEFINES += HAVE_GCONF
    HEADERS += mgconfitem.h \
               MGConfItem
    SOURCES += mgconfitem.cpp

    INSTALL_HEADERS += mgconfitem.h \
                       MGConfItem

} else {
    warning("gconf-2.0 not found; MGConfItem will not be built")
}

equals(QT_MAJOR_VERSION, 4) {
    include(../json/json.pri)
}

system(qdbusxml2cpp notificationmanager.xml -p mnotificationmanagerproxy -c MNotificationManagerProxy -i metatypedeclarations.h)

SOURCES += mnotificationmanagerproxy.cpp \
           mnotification.cpp \
           mnotificationgroup.cpp \
           mremoteaction.cpp \
           mdesktopentry.cpp \
           mfiledatastore.cpp

HEADERS += mnotificationmanagerproxy.h \
           mnotification.h \
           mnotification_p.h \
           mnotificationgroup.h \
           mnotificationgroup_p.h \
           MNotification \
           MNotificationGroup \
           mremoteaction.h \
           mdesktopentry_p.h \
           mlite-global.h \
           mfiledatastore_p.h \
           mdataaccess.h \
           mdatastore.h

INSTALL_HEADERS += mnotification.h \
                   mnotificationgroup.h \
                   mremoteaction.h \
                   MNotification \
                   MNotificationGroup \
                   MRemoteAction \
                   mdesktopentry.h \
                   mlite-global.h \
                   mfiledatastore.h \
                   MDesktopEntry

HEADERS *= $$INSTALL_HEADERS

DEFINES += MLITE_LIBRARY

PCFILE=mlite$${NODASH_QT_VERSION}.pc
configure($${PWD}/$${PCFILE}.in)
pcfiles.files = $$PCFILE
pcfiles.CONFIG = no_check_exist
pcfiles.path += $$INSTALL_ROOT/$$[QT_INSTALL_LIBS]/pkgconfig

headers.files += $$INSTALL_HEADERS
headers.path += $$INSTALL_ROOT/usr/include/mlite$${NODASH_QT_VERSION}

target.path += $$[QT_INSTALL_LIBS]

INSTALLS += target headers pcfiles
