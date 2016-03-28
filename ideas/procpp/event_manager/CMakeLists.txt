project( ReactHandler )
set(SRC EventManager.cpp
		Scheduler.cpp
		Server.cpp
		Client.cpp
		ClientServer.cpp
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
add_library( _${PROJECT_NAME} SHARED ${SRC} )
set_target_properties( _${PROJECT_NAME} PROPERTIES PREFIX "" )
include_directories(.)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")
