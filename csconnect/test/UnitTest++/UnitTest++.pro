TARGET = UnitTest++
LANGUAGE = C++
TEMPLATE = lib
DESTDIR = ..
CONFIG = staticlib


SOURCES = Test.cpp \
          Checks.cpp \
          TestRunner.cpp \
          TestResults.cpp \
          TestReporter.cpp \
          TestReporterStdout.cpp \
          ReportAssert.cpp \
          TestList.cpp \
          TimeConstraint.cpp \
          TestDetails.cpp \
          MemoryOutStream.cpp \
          DeferredTestReporter.cpp \
          DeferredTestResult.cpp \
          XmlTestReporter.cpp \
          CurrentTest.cpp \
          AssertException.cpp

win32:SOURCES += Win32/TimeHelpers.cpp
macx|unix:SOURCES += Posix/TimeHelpers.cpp Posix/SignalTranslator.cpp


