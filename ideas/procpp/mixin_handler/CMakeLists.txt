cmake_minimum_required(VERSION 3.4)
project( ReactHandler )

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

set(SRC EventManager.cpp
        Scheduler.cpp
        Server.cpp
        Client.cpp
        Client/Base.cpp
        Client/Redis.cpp
        Client/Soa.cpp
        ClientServer.cpp
        Codec/Utf8.cpp
        Codec/Utf16.cpp
        Codec/Json.cpp
        Codec/Base64.cpp
        DataStructure/Trie.cpp
        HttpClientServer.cpp
        WebSocketClientServer.cpp
        SocksClientServer.cpp
        SoaClientServer.cpp
        Server/Protocol/Global.cpp
        Protocol.cpp
        StateMachine/Bufferable.cpp
        Protocol/Echo.cpp
        Protocol/Socks.cpp
        Protocol/EasyP2P.cpp
        Protocol/Http.cpp
        Protocol/HttpGateway.cpp
        Protocol/WebSocket.cpp
        Protocol/Soa.cpp
        Protocol/Client/Redis.cpp
        Protocol/Client/Soa.cpp
        Dispatch/IBaseWorker.cpp
        Dispatch/RemoteWorker.cpp
        Dispatch/ProxyWorker.cpp
        Dispatch/ProcessWorker.cpp
        Utils.cpp Log/ErrorHandler.cpp
        UProtocol/Echo.cpp
        StreamReader.cpp
        ThreadPool.cpp
        Memory/SharedMemory.cpp
        )
set(LAN "php" CACHE STRING "target language")


set(PYTHON_SRC ${SRC} build/python/ReactHandler_wrap.cxx)
set(PHP_SRC ${SRC} build/php/ReactHandler_wrap.cpp)



include_directories(.)
find_package(OpenSSL REQUIRED)

# don't build into ext for the time being
if (${LAN} STREQUAL "native")
    find_package(OpenCV REQUIRED)
endif()

find_package(ZLIB REQUIRED)
set( DEPENDED_LIBS
        ${OPENSSL_LIBRARIES}
        ${OpenCV_LIBS}
        ${ZLIB_LIBRARIES}
        pthread
        uuid
     )

if (${LAN} STREQUAL "native")
    set( TEST
            httpgateway
            util
            opencv
            map
            http
            http_process_worker
            soa_server_dispatcher
            soa_client
            redis
            soa_server
            client
            dispatcher
            watch_stdin
            schedule
            socks
            websocket
            trie
            echo_worker
            socks_worker
            proxy_worker
            remote_worker
            worker
            unix_server
            udp_server
            easyp2p
            pool
            allocator
            event_hook
        )

    list(LENGTH TEST count)
    set(count 1)
    set(index 0)
    while (index LESS count)

        list(GET TEST ${index} target)
        math(EXPR index "${index}+1")

        add_executable( ${target} ${SRC} "tests/${target}.cpp" )
        target_link_libraries( ${target} ${DEPENDED_LIBS} )
    endwhile()

else()
    if (${LAN} STREQUAL "php")
        find_package(PHP5 REQUIRED)
        include_directories(${PHP5_INCLUDE_PATH})

        add_library( _${PROJECT_NAME} SHARED ${PHP_SRC} )
    elseif (${LAN} STREQUAL "python")
        find_package(PythonLibs REQUIRED)
        include_directories(${PYTHON_INCLUDE_DIRS})

        add_library( _${PROJECT_NAME} SHARED ${PYTHON_SRC} )
    endif()

    target_link_libraries( _${PROJECT_NAME} ${DEPENDED_LIBS} )
    set_target_properties( _${PROJECT_NAME} PROPERTIES PREFIX "" )

endif()

# release
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -g -O2 -Wall")

# debug
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -g -O0 -Wall")
