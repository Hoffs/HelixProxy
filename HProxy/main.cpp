#define _CRT_SECURE_NO_WARNINGS
#include <uWS/uWS.h>
#include <iostream>
#include "HProxyServer.h"

int main(int argc, char* argv[])
{
    uWS::Hub h;

	h.onConnection([](uWS::WebSocket<uWS::SERVER> *ws, uWS::HttpRequest req)
	{
		std::cout << "Connection..." << std::endl;
	});

    h.onMessage([](uWS::WebSocket<uWS::SERVER> *ws, char *message, size_t length, uWS::OpCode opCode) {
        std::cout << "Message received..." << std::endl;
    	ws->send(message, length, opCode);
    });

    h.onHttpRequest([](uWS::HttpResponse *res, uWS::HttpRequest req, char *data, size_t length, size_t remainingBytes) {
		const char* message = nullptr;
		size_t len = 0;
    	res->end(message, len);
        // res->end(const char *, size_t);
    });

	auto server = new HProxyServer(3000);
}
