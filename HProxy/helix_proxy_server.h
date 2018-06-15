#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <uWS/Hub.h>
#include <evpp/event_loop.h>
#include <evpp/event_loop_thread_pool.h>

class helix_proxy_server
{
	uWS::Hub h_;
	std::unique_ptr<evpp::EventLoop> loop_;
	std::unique_ptr<evpp::EventLoopThreadPool> pool_;
	int out_port_;
	char *out_ip_;
	void on_connection_handler(uWS::WebSocket<uWS::SERVER> *ws, uWS::HttpRequest req) const;
	static void on_message_handler(uWS::WebSocket<uWS::SERVER> *ws, char *message, size_t length, uWS::OpCode op_code);
	static void on_disconnection_handler(uWS::WebSocket<true>* ws, int code, char* message, size_t length);
public:
	helix_proxy_server(int in_port, char *out_ip, int out_port);
};
