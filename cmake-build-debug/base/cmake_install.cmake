# Install script for directory: /tmp/tmp.5vGBuTlMNx/base

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/tmp/tmp.5vGBuTlMNx/cmake-build-debug/base/libserver_base.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/muduo/base" TYPE FILE FILES
    "/tmp/tmp.5vGBuTlMNx/base/Atomic.h"
    "/tmp/tmp.5vGBuTlMNx/base/Condition.h"
    "/tmp/tmp.5vGBuTlMNx/base/CountDownLatch.h"
    "/tmp/tmp.5vGBuTlMNx/base/CurrentThread.h"
    "/tmp/tmp.5vGBuTlMNx/base/Date.h"
    "/tmp/tmp.5vGBuTlMNx/base/Exception.h"
    "/tmp/tmp.5vGBuTlMNx/base/LogStream.h"
    "/tmp/tmp.5vGBuTlMNx/base/Logging.h"
    "/tmp/tmp.5vGBuTlMNx/base/Mutex.h"
    "/tmp/tmp.5vGBuTlMNx/base/StringPiece.h"
    "/tmp/tmp.5vGBuTlMNx/base/Thread.h"
    "/tmp/tmp.5vGBuTlMNx/base/ThreadPool.h"
    "/tmp/tmp.5vGBuTlMNx/base/TimeZone.h"
    "/tmp/tmp.5vGBuTlMNx/base/Timestamp.h"
    "/tmp/tmp.5vGBuTlMNx/base/Types.h"
    "/tmp/tmp.5vGBuTlMNx/base/WeakCallback.h"
    "/tmp/tmp.5vGBuTlMNx/base/copyable.h"
    "/tmp/tmp.5vGBuTlMNx/base/noncopyable.h"
    )
endif()

