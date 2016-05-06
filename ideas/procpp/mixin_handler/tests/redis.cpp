#include "ReactHandler.h"
#include <iostream>
#include <vector>

using namespace P::Client;
using namespace std;

int main()
{

	// really old style
	ClientServer c;
	c.connect("localhost", 6379, [&c](int fd, ConnectResult r) {
		if (r == ConnectResult::OK) {
			auto p_proto = new Redis(c);
			auto& proto = *p_proto;
			c.watch(fd, c.to_callbacks(proto));

			GUID request_id;
			auto packet = Redis::request_packet<Redis, Redis::packet_type::GET>(request_id, string("key"));
			c.write(fd, packet.data(), packet.length());
			proto.add_callback(request_id, Utils::to_function([](RedisReply& r) {
				cout << "value = " << r.reply << (r.is_nil() ? " (nil)" : "") << endl;
			}));
		}
	}, true);

	c.start();

/*
	// old style
	C::Redis c("localhost", 6379);
	if (!c.connect()) L.error_log("connect failed");

	GUID request_id;
	c.cmd<Redis, Redis::packet_type::GET>(request_id, Utils::to_function([](RedisReply& r) {
		cout << "value = " << r.reply << (r.is_nil() ? " (nil)" : "") << endl;
	}), string("key"));

	c.start();
*/

/*
	// new style
	C::Redis c("localhost", 6379);


	GUID request_id;
	c.cmd<Redis, Redis::packet_type::GET>(request_id, Utils::to_function([](RedisReply& r) {
		cout << "value = " << r.reply << (r.is_nil() ? " (nil)" : "") << endl;
	}), string("key"));

	L.debug_log("before wait");
	c.wait(1500, vector<GUID>({request_id}));
	L.debug_log("after wait");
*/



	return 0;
}
