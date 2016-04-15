SOURCES = EventManager.cpp Scheduler.cpp Server.cpp Client.cpp ClientServer.cpp Codec/Base64.cpp SocksClientServer.cpp WebSocketClientServer.cpp \
		  HttpClientServer.cpp Server/Protocol/Global.cpp Protocol/Client/Redis.cpp \
		  Codec/Utf8.cpp DataStructure/Trie.cpp \
		  Protocol.cpp StateMachine/Bufferable.cpp Protocol/Echo.cpp Protocol/Socks.cpp Protocol/EasyP2P.cpp Protocol/Http.cpp Protocol/WebSocket.cpp \
		  Dispatch/IBaseWorker.cpp Dispatch/RemoteWorker.cpp Dispatch/ProxyWorker.cpp Utils.cpp Log/ErrorHandler.cpp \
		  UProtocol/Echo.cpp StreamReader.cpp

watch_stdin: tests/watch_stdin.cpp $(SOURCES)
	bash -lic 'rc $+ '

schedule: tests/schedule.cpp $(SOURCES)
	bash -lic 'rc $+ '

client: tests/client.cpp $(SOURCES)
	bash -lic 'rc $+ '

socks: tests/socks.cpp $(SOURCES)
	bash -lic 'rc $+ -valgrind'

redis: tests/redis.cpp $(SOURCES)
	bash -lic 'rc $+ -valgrind'

http: tests/http.cpp $(SOURCES)
	bash -lic 'rc $+ -valgrind'

websocket: tests/websocket.cpp $(SOURCES)
	bash -lic 'rc $+ -valgrind'

trie: tests/trie.cpp $(SOURCES)
	bash -lic 'rc $+'

echo_worker: tests/echo_worker.cpp $(SOURCES)
	rm -f /tmp/react_handler.sock
	bash -lic 'rc $+ -keep'

socks_worker: tests/socks_worker.cpp $(SOURCES)
	rm -f /tmp/react_handler.sock
	bash -lic 'rc $+ -valgrind'

proxy_worker: tests/proxy_worker.cpp $(SOURCES)
	bash -lic 'rc $+ -valgrind'

remote_worker: tests/remote_worker.cpp $(SOURCES)
	bash -lic 'rc $+ -valgrind'

worker: tests/worker.cpp $(SOURCES)
	bash -lic 'rc $+ -valgrind'

unix_server: tests/unix_server.cpp $(SOURCES)
	rm -f /tmp/test.sock
	bash -lic 'rc $+ '

udp_server: tests/udp_server.cpp $(SOURCES)
	bash -lic 'rc $+ '

easyp2p: tests/easyp2p.cpp $(SOURCES)
	bash -lic 'rc $+ -valgrind'

pool: tests/pool.cpp ThreadPool.cpp
	bash -lic 'rc $+ '

allocator: tests/allocator.cpp Memory/SharedMemory.cpp Log/ErrorHandler.cpp
	bash -lic 'rc $+  -rflags rflags.php'

event_hook: tests/event_hook.cpp
	bash -lic 'rc $+  -rflags rflags.php'

