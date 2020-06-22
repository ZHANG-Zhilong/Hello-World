include_directories(${PROJECT_SOURCE_DIR})

set(base_SRCS noncopyable.h copyable.h CurrentThread.h Types.h CountDownLatch.cpp CountDownLatch.h Mutex.h Condition.cpp Condition.h CurrentThread.cpp Thread.cpp Thread.h Atomic.h Exception.cpp Exception.h Logging.cpp Logging.h LogStream.cpp LogStream.h Timestamp.cpp Timestamp.h TimeZone.cpp TimeZone.h StringPiece.h Date.cpp Date.h ThreadPool.cpp ThreadPool.h)

add_library(server_base ${base_SRCS})
target_link_libraries(server_base pthread rt)

install(TARGETS server_base DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/muduo/base)
