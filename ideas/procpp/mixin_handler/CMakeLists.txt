project( ReactHandler )
set(SRC EventManager.cpp
		Scheduler.cpp
		Server.cpp
		Client.cpp
		ClientServer.cpp
		Codec/Base64.cpp
		HttpClientServer.cpp
		WebSocketClientServer.cpp
		SocksClientServer.cpp
		Server/Protocol/Global.cpp
		Protocol.cpp
		StateMachine/Bufferable.cpp
		Protocol/Echo.cpp
		Protocol/Socks.cpp
		Protocol/EasyP2P.cpp
		Protocol/Http.cpp
		Protocol/WebSocket.cpp
		Dispatch/IBaseWorker.cpp
		Dispatch/RemoteWorker.cpp
		Dispatch/ProxyWorker.cpp
		Utils.cpp Log/ErrorHandler.cpp
		UProtocol/Echo.cpp
		StreamReader.cpp
		)
set(LAN "php" CACHE STRING "target language")


set(PYTHON_SRC ${SRC} build/python/ReactHandler_wrap.cxx)
set(PHP_SRC ${SRC} build/php/ReactHandler_wrap.cpp)



if (${LAN} STREQUAL "php")
    include_directories(. /usr/include/php5 /usr/include/php5/main /usr/include/php5/TSRM /usr/include/php5/Zend /usr/include/php5/ext /usr/include/php5/ext/date/lib)
    add_library( _${PROJECT_NAME} SHARED ${PHP_SRC} )
elseif (${LAN} STREQUAL "python")
    include_directories(. /usr/include/python2.7)
    add_library( _${PROJECT_NAME} SHARED ${PYTHON_SRC} )
endif()

find_package(OpenSSL REQUIRED)
target_link_libraries( _${PROJECT_NAME} ${OpenSSL_LIBS} )


set_target_properties( _${PROJECT_NAME} PROPERTIES PREFIX "" )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -g")
