#include "helix_proxy_server.h"
#include "helix_user.h"
#include <glog/logging.h>

helix_proxy_server::helix_proxy_server(const int in_port, const int out_port)
{
	this->out_port_ = out_port;
	loop_ = std::make_unique<evpp::EventLoop>();
	pool_ = std::make_unique<evpp::EventLoopThreadPool>(loop_.get(), 4);

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


	if (h_.listen(in_port)) {
		LOG(INFO) << "Starting evpp event loop...";
		pool_->Start(false);
		LOG(INFO) << "Listening on port " << in_port << "...";
		h_.run();
	}
}

void helix_proxy_server::on_connection_handler(uWS::WebSocket<uWS::SERVER>* ws, uWS::HttpRequest req) const
{
<<<<<<< HEAD
	auto *user = new helix_user(pool_->GetNextLoop(), this->out_port_, [ws](const size_t length, char* message)
	{
		if (ws == nullptr) return;
		ws->send(message, length, uWS::OpCode::BINARY);
		delete message;
=======
	auto *user = new helix_user(pool_->GetNextLoop(), 4000, [ws](const size_t length, char *message)
	{
		if (ws == nullptr) return;
		ws->send(message, length, uWS::OpCode::BINARY);
		delete[] message;
		LOG(INFO) << "Sent message to WS Proxy client with length: " << length;
>>>>>>> cb1fa42b9e4034cdbd9b829fc21eac5c49e87d0b
	});
	ws->setUserData(user);
	LOG(INFO) << "Client connected to WS proxy: " << user->info();
}

void helix_proxy_server::on_message_handler(uWS::WebSocket<uWS::SERVER>* ws, char *message, const size_t length, uWS::OpCode op_code)
{
	const auto msg = new char[length];
	memcpy(msg, message, length);
	auto *user = static_cast<helix_user*>(ws->getUserData());
	LOG(INFO) << "Message received from WS proxy client: " << user->info();
	user->send_async(length, msg);
}

void helix_proxy_server::on_disconnection_handler(uWS::WebSocket<true>* ws, int code, char *message, size_t length)
{
	auto *user = static_cast<helix_user*>(ws->getUserData());
	LOG(INFO) << "Client disconnected from WS proxy: " << user->info();
	delete user;
}
