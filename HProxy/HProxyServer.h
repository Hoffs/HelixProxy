#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include <uWS/Hub.h>
#include <boost/asio/io_service.hpp>

class HProxyServer
{
private:
	uWS::Hub h_;
	boost::asio::io_service io_service_;

	void on_connection_handler(uWS::WebSocket<uWS::SERVER> *ws, uWS::HttpRequest req);
	void on_message_handler(uWS::WebSocket<uWS::SERVER> *ws, char *message, size_t length, uWS::OpCode op_code);
	void onDisconnection(uWS::WebSocket<uWS::SERVER> *ws, int code, char *message, size_t length);
public:
	explicit HProxyServer(int port);
};
