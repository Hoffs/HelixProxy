#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <uWS/Hub.h>
#include <evpp/event_loop.h>
#include <evpp/event_loop_thread.h>
#include <evpp/event_loop_thread_pool.h>

class helix_proxy_server
{
	uWS::Hub h_;
	std::thread *evpp_thread_;
	// evpp::EventLoop loop_;
	std::unique_ptr<evpp::EventLoop> loop_;
	std::unique_ptr<evpp::EventLoopThreadPool> pool_;
	// std::unique_ptr<evpp::EventLoopThread> loop_;
	void on_connection_handler(uWS::WebSocket<uWS::SERVER> *ws, uWS::HttpRequest req);
	static void on_message_handler(uWS::WebSocket<uWS::SERVER> *ws, char *message, size_t length, uWS::OpCode op_code);
	static void on_disconnection_handler(uWS::WebSocket<true>* ws, int code, char* message, size_t length);
public:
	explicit helix_proxy_server(const int port);
};
