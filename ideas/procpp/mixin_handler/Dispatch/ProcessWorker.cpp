#include "ReactHandler.h"
#include <unistd.h>//fork
#include <sys/prctl.h>//prctl


ProcessWorker::ProcessWorker(ClientServer& server, int n)
{
	if (n == ProcessWorker::NUMBER_CORES) n = Utils::get_cpu_cores();

    auto child_callback = [&server]() {

        prctl(PR_SET_PDEATHSIG, SIGHUP);
        server.fire<EventManager::ON_FORK>();

        server.start();

        // child should never return
        exit(0);

    };

	for (int i = 0; i < n; i++) {
		auto pid = fork();
		if (pid == -1) L.error_exit("fork");

		if (pid) {
		} else {

            child_callback();

		}
	}


    Utils::supervise_subprocess(child_callback);

}
