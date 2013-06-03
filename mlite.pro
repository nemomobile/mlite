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

QT     = core dbus
equals(QT_MAJOR_VERSION, 4): TARGET = $$qtLibraryTarget(mlite)
equals(QT_MAJOR_VERSION, 5): TARGET = $$qtLibraryTarget(mlite5)
TEMPLATE = lib

CONFIG += link_pkgconfig
packagesExist(gconf-2.0) {
    PKGCONFIG += gconf-2.0
    DEFINES += HAVE_GCONF
    HEADERS += mgconfgroup.h \
               MGConfGroup \
               mgconfitem.h \
               mgconfitem_p.h \
               MGConfItem
    SOURCES += mgconfgroup.cpp \
               mgconfitem.cpp

    INSTALL_HEADERS += mgconfgroup.h \
                       MGConfGroup \
                       mgconfitem.h \
                       MGConfItem

} else {
    warning("gconf-2.0 not found; MGConfItem will not be built")
}

equals(QT_MAJOR_VERSION, 4) {
    include(json/json.pri)
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

HEADERS += $$INSTALL_HEADERS

DEFINES += MLITE_LIBRARY

OBJECTS_DIR = .obj
MOC_DIR = .moc

equals(QT_MAJOR_VERSION, 4): PCFILE=mlite.pc
equals(QT_MAJOR_VERSION, 5): PCFILE=mlite5.pc

# substitutions
system(cp $${PCFILE}.in $$PCFILE)
system(sed -i "s/\\@VERSION\\@/$$VERSION/g" $$PCFILE)

pcfiles.files = $$PCFILE
pcfiles.path += $$INSTALL_ROOT/$$[QT_INSTALL_LIBS]/pkgconfig

headers.files += $$INSTALL_HEADERS
equals(QT_MAJOR_VERSION, 4): headers.path += $$INSTALL_ROOT/usr/include/mlite
equals(QT_MAJOR_VERSION, 5): headers.path += $$INSTALL_ROOT/usr/include/mlite5

target.path += $$[QT_INSTALL_LIBS]

INSTALLS += target headers pcfiles
