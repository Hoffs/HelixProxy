#include "HProxyServer.h"
#include <iostream>
#include "HUser.h"

/**
 * \brief Handler once websocket connection is established.
 * \param ws WebSocket server
 * \param req HttpRequest
 */
void HProxyServer::on_connection_handler(uWS::WebSocket<uWS::SERVER>* ws, uWS::HttpRequest req)
{
	std::cout << "Client connected to WS proxy." << std::endl;
	auto *user = new huser(&loop_, 4000, [ws](const size_t length, const char *message)
	{
		ws->send(message, length, uWS::OpCode::BINARY, [](uWS::WebSocket<uWS::SERVER> *ws, void *data, bool cancelled, void *reserver)
		{
			std::cout << "Sent to WS user" << std::endl;
		});
	});
	std::cout << "WS proxy UserInfo: " << user->info() << std::endl;
	ws->setUserData(user);
}

void HProxyServer::on_message_handler(uWS::WebSocket<uWS::SERVER>* ws, char * message, size_t length, uWS::OpCode op_code)
{
	auto *user = static_cast<huser*>(ws->getUserData());
	std::cout << "WS Proxy received message from User: " << user->info() << std::endl;
	user->send_async(length, message);
}

HProxyServer::HProxyServer(int port)
{
	h_.onMessage([this](uWS::WebSocket<uWS::SERVER>* ws, char * message, size_t length, uWS::OpCode op_code)
	{
		this->on_message_handler(ws, message, length, op_code);
	});

	h_.onConnection([this](uWS::WebSocket<uWS::SERVER>* ws, uWS::HttpRequest req)
	{
		this->on_connection_handler(ws, req);
	});

	h_.onDisconnection([this](uWS::WebSocket<uWS::SERVER> *ws, int code, char *message, size_t length)
	{
		this->on_disconnection_handler(ws, code, message, length);
	});


	if (h_.listen(port)) {
		evpp_thread = new std::thread([this]() 
		{
			std::cout << "Starting evpp Event loop..." << std::endl;
			this->loop_.Run();
		});
		std::cout << "Listening on port " << port << "..." << std::endl;
		h_.run();
	}
}

void HProxyServer::on_disconnection_handler(uWS::WebSocket<true>* ws, int code, char* message, size_t length)
{
	auto *user = static_cast<huser*>(ws->getUserData());
	std::cout << "WS Proxy user disconnecting: " << user->info() << std::endl;
	delete user;
}
