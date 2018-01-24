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
	std::cout << "Client connected to WS proxy" << std::endl;
	auto *user = new huser(io_service_);
	ws->setUserData(user);
	std::cout << "UserInfo: " << user->info() << std::endl;
}

void HProxyServer::on_message_handler(uWS::WebSocket<uWS::SERVER>* ws, char * message, size_t length, uWS::OpCode op_code)
{
	auto *user = static_cast<huser*>(ws->getUserData());
	std::cout << "Message from User: " << user->info() << std::endl;
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

	if (h_.listen(port)) {
		std::cout << "Listening on port " << port << "..." << std::endl;
		h_.run();
	}
}
