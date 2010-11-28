
set(SOURCES
    AssertException.cpp
    Test.cpp
    Checks.cpp
    TestRunner.cpp
    TestResults.cpp
    TestReporter.cpp
    TestReporterStdout.cpp
    ReportAssert.cpp
    TestList.cpp
    TimeConstraint.cpp
    TestDetails.cpp
    MemoryOutStream.cpp
    DeferredTestReporter.cpp
    DeferredTestResult.cpp
    XmlTestReporter.cpp
    CurrentTest.cpp
)

if (WIN32)
    set (SOURCES ${SOURCES} Win32/TimeHelpers.cpp)
else (WIN32)
    set (SOURCES ${SOURCES} Posix/TimeHelpers.cpp Posix/SignalTranslator.cpp)
endif (WIN32)

add_library(UnitTest++ STATIC ${SOURCES})

SET_TARGET_PROPERTIES(UnitTest++ PROPERTIES COMPILE_FLAGS "-g -Wall -W -Winline -ansi") 

