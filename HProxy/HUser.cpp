#include "HUser.h"
#include <iostream>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <evpp/buffer.h>
#include <evpp/tcp_conn.h>

void huser::connect_handler(const evpp::TCPConnPtr &conn)
{
	if (conn->IsConnected())
	{
		std::cout << "Connected to TCP Server. User: " << this->info() << std::endl;
	} else
	{
		std::cout << "Failed to connect to TCP Server." << " Status: " << conn->status() << ". User: " << this->info() << std::endl;
	}
}

std::string huser::info() const
{
	return to_string(id_);
}

huser::huser(evpp::EventLoop *loop, const int port, const std::function<void(const size_t length, const char *message)> &callback)
{
	callback_ = callback;
	id_ = boost::uuids::uuid(boost::uuids::random_generator()());
	const auto conn_string = "127.0.0.1:" + std::to_string(port);
	client_ = new evpp::TCPClient(loop, conn_string, to_string(id_));
	
	client_->SetConnectionCallback([this](const evpp::TCPConnPtr &conn)
	{
		this->connect_handler(conn);
	});

	client_->SetMessageCallback([this](const evpp::TCPConnPtr &conn, evpp::Buffer *msg)
	{
		this->message_callback(conn, msg);
	});

	client_->set_connecting_timeout(evpp::Duration(10.0));
	std::cout << "Connecting to TCP User: " << this->info() << std::endl;
	client_->Connect();
}
// std::function<void(const size_t response_length, char *response_message)> handler
void huser::send_async(const size_t length, char* message) const
{
	std::cout << "Sending to TCP User: " << this->info() << std::endl;
	if (client_->conn() == nullptr || client_->conn()->IsDisconnected()) return;
	auto *len = new unsigned char[4]; // Length part of the packet
	auto *full = new unsigned char [4 + length]; // Full packet
	auto length_int = size_to_int(length);

	memcpy(len, &length_int, sizeof(length_int));
	memcpy(full, len, 4);
	memcpy(full + 4, message, length);

	client_->conn()->Send(full, 4 + length);
	std::cout << "Finished sending to TCP User: " << this->info() << std::endl;
}

huser::~huser()
{
	std::cout << "Destroying HUser: " << this->info() << std::endl;
	if (client_->conn() != nullptr && client_->conn()->IsConnected())
	{
		client_->Disconnect();
		client_ = nullptr;
	}
}

void huser::message_callback(const evpp::TCPConnPtr& conn, evpp::Buffer *msg)
{
	std::cout << "Received message: " << msg->ToString() << ". User: " << this->info() << std::endl;
	msg->Skip(4); // Skip 4 size bytes;
	callback_(msg->length(), msg->data());
}

int huser::size_to_int(const size_t u)
{
	if (u > std::numeric_limits<int>::max())
	{
		throw std::overflow_error("size_t cannot be stored in a variable of type int.");
	}

	return static_cast<int>(u);
}
