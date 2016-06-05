#include "ReactHandler.h"
#include <unistd.h>//fork


ProcessWorker::ProcessWorker(ClientServer& server, int n)
{
	if (n == ProcessWorker::NUMBER_CORES) n = Utils::get_cpu_cores();

	for (int i = 0; i < n; i++) {
		auto pid = fork();
		if (pid == -1) L.error_exit("fork");

		if (pid) {
		} else {

			server.fire<EventManager::ON_FORK>();

			server.start();

			// child should never return
			exit(0);
		}
	}


	//restart child
	int status;
	pid_t child;
	while ((child = waitpid(-1, &status, 0)) > 0)
	{
		auto pid = fork();
		if (pid == -1) L.error_exit("fork");

		if (pid) {
		} else {
			server.fire<EventManager::ON_FORK>();

			server.start();

			// child should never return
			exit(0);
		}
	}

}
