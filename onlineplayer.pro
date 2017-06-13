TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
INCLUDEPATH += .include
LIBS   += -lpthread -lasound -lcurl -lz -L$$PWD/libspc/ -lmad -lsqlite3


SOURCES += main.cpp \
    coremain.cpp \
    globaflags.cpp \
    mythread.cpp \
    thread.cpp \
    decode/audiooss.cpp \
    decode/buffercache/cachebuf.cpp \
    network/netclient.cpp \
    cache/docache.cpp \
    network/httpclient.cpp \
    tools/json/json_reader.cpp \
    tools/json/json_value.cpp \
    tools/json/json_writer.cpp \
    network/checknetwork.cpp \
    playmp3list.cpp \
    decode/mp3/mp3decode.cpp \
    decode/thransferstation.cpp \
    mp3player.cpp \
    decode/audiothreadcheck.cpp \
    decode/soundalsa.cpp \
    tools/checkspace/spacecheck.cpp \
    tools/times/gettime.cpp \
    network/udpclient.cpp \
    network/heartbeat.cpp \
    tools/debug/debug.cpp \
    tools/cpumem/cpuandmem.cpp \
    network/proccessdata.cpp \
    tools/tools.cpp \
    tools/handlesqlite.cpp \
    playlistcontrol.cpp \
    tools/md5sum.cpp \
    network/netlink.cpp \
    update/update.cpp \
    rommanage/managerom.cpp \
    decode/mp3/alsaapi.cpp

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    coremain.h \
    globaflags.h \
    mythread.h \
    thread.h \
    decode/audiooss.h \
    decode/buffercache/cachebuf.h \
    network/netclient.h \
    network/protocal.h \
    cache/docache.h \
    network/httpclient.h \
    include/json/autolink.h \
    include/json/config.h \
    include/json/features.h \
    include/json/forwards.h \
    include/json/json.h \
    include/json/reader.h \
    include/json/value.h \
    include/json/writer.h \
    tools/json/json_batchallocator.h \
    include/json/json_batchallocator.h \
    network/checknetwork.h \
    include/mad/mad.h \
    playmp3list.h \
    decode/mp3/mp3decode.h \
    decode/thransferstation.h \
    mp3player.h \
    decode/audiothreadcheck.h \
    decode/soundalsa.h \
    tools/checkspace/spacecheck.h \
    include/mad/mad.h \
    tools/times/gettime.h \
    network/udpclient.h \
    network/heartbeat.h \
    tools/debug/debug.h \
    tools/cpumem/cpuandmem.h \
    network/proccessdata.h \
    tools/tools.h \
    tools/handlesqlite.h \
    playlistcontrol.h \
    tools/md5sum.h \
    network/netlink.h \
    update/update.h \
    rommanage/managerom.h \
    decode/mp3/alsaapi.h

