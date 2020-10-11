# tdlib

contains(MEEGO_EDITION, harmattan) {
    isEmpty(TD_DIRECTORY) {
        TD_DIRECTORY = /opt/QtSDK/Madde/sysroots/harmattan_sysroot_10.2011.34-1_slim/usr/local
    }

    INCLUDEPATH += $$TD_DIRECTORY/include

    LIBS += $$TD_DIRECTORY/lib/libtdclient.a $$TD_DIRECTORY/lib/libtdcore.a $$TD_DIRECTORY/lib/libtdapi.a $$TD_DIRECTORY/lib/libtdnet.a \
            $$TD_DIRECTORY/lib/libtddb.a $$TD_DIRECTORY/lib/libtdactor.a $$TD_DIRECTORY/lib/libtdutils.a $$TD_DIRECTORY/lib/libtdsqlite.a \
            $$TD_DIRECTORY/lib/libssl.a $$TD_DIRECTORY/lib/libcrypto.a -ldl $$TD_DIRECTORY/lib/libz.a
} else {
    LIBS += -ltdclient -ltdcore -ltdapi -ltdnet -ltddb -ltdactor -ltdutils -ltdsqlite -lssl -lcrypto -ldl -lz
}
