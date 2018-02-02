#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include <uWS/Hub.h>
#include <boost/asio/io_service.hpp>
#include <evpp/event_loop.h>

class HProxyServer
{
private:
	uWS::Hub h_;
	evpp::EventLoop loop_;
	std::thread *evpp_thread;

	void on_connection_handler(uWS::WebSocket<uWS::SERVER> *ws, uWS::HttpRequest req);
	void on_message_handler(uWS::WebSocket<uWS::SERVER> *ws, char *message, size_t length, uWS::OpCode op_code);
	void onDisconnection(uWS::WebSocket<uWS::SERVER> *ws, int code, char *message, size_t length);
public:
	explicit HProxyServer(int port);
	void on_disconnection_handler(uWS::WebSocket<true>* ws, int code, char* message, size_t length);
};
