project( ReactHandler )
set(SRC EventManager.cpp
		Scheduler.cpp
		Server.cpp
		Client.cpp
		ClientServer.cpp
		HttpClientServer.cpp
		Server/Protocol/Global.cpp
		Protocol.cpp
		StateMachine/Bufferable.cpp
		Protocol/Echo.cpp
		Protocol/Socks.cpp
		Protocol/EasyP2P.cpp
		Protocol/Http.cpp
		Dispatch/IBaseWorker.cpp
		Dispatch/RemoteWorker.cpp
		Dispatch/ProxyWorker.cpp
		Utils.cpp Log/ErrorHandler.cpp
		UProtocol/Echo.cpp
		StreamReader.cpp
		)
set(PYTHON_SRC ${SRC} build/ReactHandler_python_wrap.cxx)
set(PHP_SRC ${SRC} build/ReactHandler_php_wrap.cxx)

add_library( _${PROJECT_NAME} SHARED ${PHP_SRC} )
set_target_properties( _${PROJECT_NAME} PROPERTIES PREFIX "" )
include_directories(. /usr/include/python2.7 /usr/include/php5 /usr/include/php5/main /usr/include/php5/TSRM /usr/include/php5/Zend /usr/include/php5/ext /usr/include/php5/ext/date/lib)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -g")
