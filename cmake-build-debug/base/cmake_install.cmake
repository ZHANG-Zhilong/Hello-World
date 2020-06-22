# Install script for directory: /tmp/tmp.5rawK3RpZ8/base

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/tmp/tmp.5rawK3RpZ8/cmake-build-debug/base/libserver_base.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/muduo/base" TYPE FILE FILES
    "/tmp/tmp.5rawK3RpZ8/base/Atomic.h"
    "/tmp/tmp.5rawK3RpZ8/base/Condition.h"
    "/tmp/tmp.5rawK3RpZ8/base/CountDownLatch.h"
    "/tmp/tmp.5rawK3RpZ8/base/CurrentThread.h"
    "/tmp/tmp.5rawK3RpZ8/base/Date.h"
    "/tmp/tmp.5rawK3RpZ8/base/Exception.h"
    "/tmp/tmp.5rawK3RpZ8/base/LogStream.h"
    "/tmp/tmp.5rawK3RpZ8/base/Logging.h"
    "/tmp/tmp.5rawK3RpZ8/base/Mutex.h"
    "/tmp/tmp.5rawK3RpZ8/base/StringPiece.h"
    "/tmp/tmp.5rawK3RpZ8/base/Thread.h"
    "/tmp/tmp.5rawK3RpZ8/base/ThreadPool.h"
    "/tmp/tmp.5rawK3RpZ8/base/TimeZone.h"
    "/tmp/tmp.5rawK3RpZ8/base/Timestamp.h"
    "/tmp/tmp.5rawK3RpZ8/base/Types.h"
    "/tmp/tmp.5rawK3RpZ8/base/WeakCallback.h"
    "/tmp/tmp.5rawK3RpZ8/base/copyable.h"
    "/tmp/tmp.5rawK3RpZ8/base/noncopyable.h"
    )
endif()

