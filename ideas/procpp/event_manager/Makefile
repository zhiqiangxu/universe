
test: tests/test.cpp EventManager.cpp Protocol.cpp
	bash -lic 'rc $+ '

server: tests/server.cpp EventManager.cpp Protocol.cpp Server.cpp Protocol/Echo.cpp
	bash -lic 'rc $+ '

client: tests/client.cpp EventManager.cpp Protocol.cpp Client.cpp
	bash -lic 'rc $+ '

socks: tests/socks.cpp EventManager.cpp Protocol.cpp Server.cpp Client.cpp ClientServer.cpp Protocol/Socks.cpp
	bash -lic 'rc $+ -valgrind'

pool: tests/pool.cpp ThreadPool.cpp
	bash -lic 'rc $+ '

allocator: tests/allocator.cpp Memory/SharedMemory.cpp
	bash -lic 'rc $+  -rflags rflags.php'
