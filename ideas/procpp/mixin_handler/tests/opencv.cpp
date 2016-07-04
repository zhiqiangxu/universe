#include <opencv2/opencv.hpp>
#include "ReactHandler.h"


int main()
{
	HttpClientServer server;

    server.on<Http::ON_REQUEST>(Utils::to_function([&server](HttpRequest& req, HttpResponse& resp) {
        auto image = cv::imread( "/home/vagrant/opensource/ngx_openresty-1.7.10.2/test/ideas/opencv/data/0.png", CV_LOAD_IMAGE_COLOR );
        cv::vector<uchar> buf;
        cv::imencode(".png", image, buf, std::vector<int>());

        resp.body = "content from c++\r\n";
    }));

    server.listen(8082);
    server.start();


    return 0;
}
