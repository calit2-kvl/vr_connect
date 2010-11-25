TARGET = common
LANGUAGE = C
TEMPLATE = lib
CONFIG = staticlib

SOURCES = cJSON.c
HEADERS = cJSON.h
macx:CONFIG += x86_64
macx:CONFIG -= x86
