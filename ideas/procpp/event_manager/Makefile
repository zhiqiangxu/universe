SOURCES = EventManager.cpp Scheduler.cpp Server.cpp Client.cpp ClientServer.cpp Server/Protocol/Global.cpp \
		  Protocol.cpp StateMachine/Bufferable.cpp Protocol/Echo.cpp Protocol/Socks.cpp Protocol/EasyP2P.cpp\
		  Dispatch/IBaseWorker.cpp Dispatch/ProxyWorker.cpp Dispatch/ProxyWorker.cpp Utils.cpp Log/ErrorHandler.cpp \
		  UProtocol/Echo.cpp

test: tests/test.cpp $(SOURCES)
	bash -lic 'rc $+ '

server: tests/server.cpp $(SOURCES)
	bash -lic 'rc $+ '

client: tests/client.cpp $(SOURCES)
	bash -lic 'rc $+ '

socks: tests/socks.cpp $(SOURCES)
	bash -lic 'rc $+ -valgrind'

echo_worker: tests/echo_worker.cpp $(SOURCES)
	rm -f /tmp/react_handler.sock
	bash -lic 'rc $+ -valgrind'

socks_worker: tests/socks_worker.cpp $(SOURCES)
	rm -f /tmp/react_handler.sock
	bash -lic 'rc $+ -valgrind'

proxy_worker: tests/proxy_worker.cpp $(SOURCES)
	bash -lic 'rc $+ -valgrind'

unix_server: tests/unix_server.cpp $(SOURCES)
	rm -f /tmp/test.sock
	bash -lic 'rc $+ '

udp_server: tests/udp_server.cpp $(SOURCES)
	bash -lic 'rc $+ '

p2p: tests/easyp2p.cpp $(SOURCES)
	bash -lic 'rc $+ -valgrind'

pool: tests/pool.cpp ThreadPool.cpp
	bash -lic 'rc $+ '

allocator: tests/allocator.cpp Memory/SharedMemory.cpp Log/ErrorHandler.cpp
	bash -lic 'rc $+  -rflags rflags.php'

event_hook: tests/event_hook.cpp
	bash -lic 'rc $+  -rflags rflags.php'

