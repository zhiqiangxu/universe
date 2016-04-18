cmake_minimum_required(VERSION 3.4)
project( ReactHandler )

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

set(SRC EventManager.cpp
		Scheduler.cpp
		Server.cpp
		Client.cpp
		ClientServer.cpp
		Codec/Utf8.cpp
		DataStructure/Trie.cpp
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
		Protocol/Client/Redis.cpp
		Dispatch/IBaseWorker.cpp
		Dispatch/RemoteWorker.cpp
		Dispatch/ProxyWorker.cpp
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

if (${LAN} STREQUAL "native")
	set( TEST
			dispatcher
			watch_stdin
			schedule
			client
			socks
			redis
			http
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
	set(index 0)
	while (index LESS count)

		list(GET TEST ${index} target)
		math(EXPR index "${index}+1")

    	add_executable( ${target} ${SRC} "tests/${target}.cpp" )
		target_link_libraries( ${target} ${OPENSSL_LIBRARIES} pthread )
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

	target_link_libraries( _${PROJECT_NAME} ${OPENSSL_LIBRARIES} pthread )
	set_target_properties( _${PROJECT_NAME} PROPERTIES PREFIX "" )

endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -g -O0")
