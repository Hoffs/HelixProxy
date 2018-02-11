﻿#include "HProxyServer.h"
#include <iostream>
#include "HUser.h"
#include <glog/logging.h>

helix_proxy_server::helix_proxy_server(const int port)
{
	loop_ = std::make_unique<evpp::EventLoop>();
	pool_ = std::make_unique<evpp::EventLoopThreadPool>(loop_.get(), 20);

	h_.onMessage([](uWS::WebSocket<uWS::SERVER>* ws, char * message, size_t length, uWS::OpCode op_code)
	{
		on_message_handler(ws, message, length, op_code);
	});

	h_.onConnection([this](uWS::WebSocket<uWS::SERVER>* ws, uWS::HttpRequest req)
	{
		this->on_connection_handler(ws, req);
	});

	h_.onDisconnection([](uWS::WebSocket<uWS::SERVER> *ws, int code, char *message, size_t length)
	{
		on_disconnection_handler(ws, code, message, length);
	});


	if (h_.listen(port)) {
		evpp_thread_ = new std::thread([this]() 
		{
			LOG(INFO) << "Starting evpp event loop...";
			pool_->Start(false);
			// this->loop_->Run();
		});
		LOG(INFO) << "Listening on port " << port << "...";
		h_.run();
	}
}

void helix_proxy_server::on_connection_handler(uWS::WebSocket<uWS::SERVER>* ws, uWS::HttpRequest req)
{
	auto *user = new helix_user(pool_->GetNextLoop(), 4000, [ws](const size_t length, const char *message)
	{
		ws->send(message, length, uWS::OpCode::BINARY, [](uWS::WebSocket<uWS::SERVER> *ws, void *data, bool cancelled, void *reserver)
		{
			LOG(INFO) << "Message sent back to WS Client successfully.";
		});
	});
	ws->setUserData(user);
	LOG(INFO) << "Client connected to WS proxy: " << user->info();
}

void helix_proxy_server::on_message_handler(uWS::WebSocket<uWS::SERVER>* ws, char * message, const size_t length, uWS::OpCode op_code)
{
	auto *user = static_cast<helix_user*>(ws->getUserData());
	LOG(INFO) << "Message received from WS Client:" << user->info();
	user->send_async(length, message);
}


void helix_proxy_server::on_disconnection_handler(uWS::WebSocket<true>* ws, int code, char* message, size_t length)
{
	auto *user = static_cast<helix_user*>(ws->getUserData());
	LOG(INFO) << "Client disconnected from proxy: " << user->info();
	delete user;
}
