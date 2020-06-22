set(net_SRCS Buffer.cc Buffer.h Channel.cpp Channel.h EventLoop.cpp EventLoop.h Poller.cpp Poller.h TimerQueue.cpp TimerQueue.h TimerId.h Timer.cpp Timer.h Callbacks.h SocketsOps.cpp SocketsOps.h EventLoopThread.cpp EventLoopThread.h Acceptor.cpp Acceptor.h Socket.cpp Socket.h InetAddress.cpp InetAddress.h Endian.h TcpServer.cpp TcpServer.h TcpConnection.cpp TcpConnection.h EventLoopThreadPool.cpp EventLoopThreadPool.h EPoller.cpp EPoller.h Connector.cpp Connector.h)

add_library(server_net ${net_SRCS})
target_link_libraries(server_net pthread)

install(TARGETS server_net DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/muduo/base)
