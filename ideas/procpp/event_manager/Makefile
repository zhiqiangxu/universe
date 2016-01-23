
all: test server  client socks socks_worker pool allocator

test: tests/test.cpp EventManager.cpp Server/Protocol/Global.cpp Protocol.cpp StateMachine/Bufferable.cpp Log/ErrorHandler.cpp Utils.cpp
	bash -lic 'rc $+ '

server: tests/server.cpp EventManager.cpp Server/Protocol/Global.cpp Protocol.cpp StateMachine/Bufferable.cpp Server.cpp \
		Protocol/Echo.cpp Log/ErrorHandler.cpp Utils.cpp
	bash -lic 'rc $+ '

client: tests/client.cpp EventManager.cpp Server/Protocol/Global.cpp Protocol.cpp StateMachine/Bufferable.cpp Client.cpp Log/ErrorHandler.cpp Utils.cpp
	bash -lic 'rc $+ '

socks: tests/socks.cpp EventManager.cpp Server/Protocol/Global.cpp Protocol.cpp StateMachine/Bufferable.cpp Server.cpp \
	   Client.cpp ClientServer.cpp Protocol/Socks.cpp Log/ErrorHandler.cpp Utils.cpp
	bash -lic 'rc $+ -valgrind'

echo_worker: tests/echo_worker.cpp EventManager.cpp Server/Protocol/Global.cpp Protocol.cpp StateMachine/Bufferable.cpp \
			 Server.cpp Client.cpp ClientServer.cpp Protocol/Echo.cpp Log/ErrorHandler.cpp Dispatch/ProcessWorker.cpp
	rm -f /tmp/react_handler.sock
	bash -lic 'rc $+ -valgrind'

socks_worker: tests/socks_worker.cpp EventManager.cpp Server/Protocol/Global.cpp Protocol.cpp StateMachine/Bufferable.cpp \
			  Server.cpp Client.cpp ClientServer.cpp Protocol/Socks.cpp Log/ErrorHandler.cpp Dispatch/ProcessWorker.cpp
	rm -f /tmp/react_handler.sock
	bash -lic 'rc $+ -valgrind'

pool: tests/pool.cpp ThreadPool.cpp
	bash -lic 'rc $+ '

allocator: tests/allocator.cpp Memory/SharedMemory.cpp Log/ErrorHandler.cpp
	bash -lic 'rc $+  -rflags rflags.php'

unix_server: tests/unix_server.cpp EventManager.cpp Server/Protocol/Global.cpp Protocol.cpp StateMachine/Bufferable.cpp Server.cpp \
			 Client.cpp ClientServer.cpp Protocol/Echo.cpp Log/ErrorHandler.cpp Utils.cpp
	rm -f /tmp/test.sock
	bash -lic 'rc $+ '

p2p: tests/easyp2p.cpp EventManager.cpp Server/Protocol/Global.cpp Protocol.cpp StateMachine/Bufferable.cpp \
	 Server.cpp Log/ErrorHandler.cpp Protocol/EasyP2P.cpp Utils.cpp
	bash -lic 'rc $+ -valgrind'


