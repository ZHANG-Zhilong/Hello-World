# Install script for directory: /tmp/tmp.5rawK3RpZ8/net

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/tmp/tmp.5rawK3RpZ8/cmake-build-debug/net/libserver_net.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/muduo/base" TYPE FILE FILES
    "/tmp/tmp.5rawK3RpZ8/net/Acceptor.h"
    "/tmp/tmp.5rawK3RpZ8/net/Buffer.h"
    "/tmp/tmp.5rawK3RpZ8/net/Callbacks.h"
    "/tmp/tmp.5rawK3RpZ8/net/Channel.h"
    "/tmp/tmp.5rawK3RpZ8/net/Connector.h"
    "/tmp/tmp.5rawK3RpZ8/net/EPoller.h"
    "/tmp/tmp.5rawK3RpZ8/net/Endian.h"
    "/tmp/tmp.5rawK3RpZ8/net/EventLoop.h"
    "/tmp/tmp.5rawK3RpZ8/net/EventLoopThread.h"
    "/tmp/tmp.5rawK3RpZ8/net/EventLoopThreadPool.h"
    "/tmp/tmp.5rawK3RpZ8/net/InetAddress.h"
    "/tmp/tmp.5rawK3RpZ8/net/Poller.h"
    "/tmp/tmp.5rawK3RpZ8/net/Socket.h"
    "/tmp/tmp.5rawK3RpZ8/net/SocketsOps.h"
    "/tmp/tmp.5rawK3RpZ8/net/TcpConnection.h"
    "/tmp/tmp.5rawK3RpZ8/net/TcpServer.h"
    "/tmp/tmp.5rawK3RpZ8/net/Timer.h"
    "/tmp/tmp.5rawK3RpZ8/net/TimerId.h"
    "/tmp/tmp.5rawK3RpZ8/net/TimerQueue.h"
    )
endif()

