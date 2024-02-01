TEMPLATE = subdirs
SUBDIRS = qtservice alkod arduino
CONFIG += ordered

arduino.commands = cd arduino && make -f Makefile
arduino_clean.commands = cd arduino && make -f Makefile clean
upload.commands = cd arduino && make -f Makefile upload

QMAKE_EXTRA_TARGETS += arduino upload
