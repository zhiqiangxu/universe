
test: test.cpp EventManager.cpp Protocol.cpp
	bash -lic 'rc $+ '

server: server.cpp EventManager.cpp Protocol.cpp Server.cpp Protocol/Echo.cpp
	bash -lic 'rc $+ '

client: client.cpp EventManager.cpp Protocol.cpp Client.cpp
	bash -lic 'rc $+ '

socks: socks.cpp EventManager.cpp Protocol.cpp Server.cpp Client.cpp ClientServer.cpp
	bash -lic 'rc $+ '
