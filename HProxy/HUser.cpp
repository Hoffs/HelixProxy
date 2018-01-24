#include "HUser.h"
#include <iostream>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

void huser::connect_handler(const boost::system::error_code &ec)
{
	if (!ec)
	{
		std::cout << "Connected to TCPServer" << std::endl;
		socket_->async_receive()
	}
}

std::string huser::info() const
{
	return to_string(id_);
}

huser::huser(boost::asio::io_service &io_service)
{
	id_ = boost::uuids::uuid(boost::uuids::random_generator()());
	socket_ = std::make_shared<tcp::socket>(io_service);
	const tcp::endpoint endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 4000);
	socket_->async_connect(endpoint, [this](const boost::system::error_code &ec)
	{
		std::cout << "Trying to connect to TCPServer" << std::endl;
		this->connect_handler(ec);
	});
}
// std::function<void(const size_t response_length, char *response_message)> handler
void huser::send_async(const size_t length, char* message) const
{
	if (!socket_->is_open()) return;
	auto *len = new unsigned char[4]; // Length part of the packet
	auto *full = new unsigned char [4 + length]; // Full packet
	auto length_int = size_to_int(length);

	memcpy(len, &length_int, sizeof(length_int));
	memcpy(full, len, 4);
	memcpy(full + 4, message, length);

	socket_->async_send(boost::asio::buffer(full, 4 + length), [](const boost::system::error_code& ec, std::size_t bytes_transferred)
	{
		std::cout << "Sent message" << std::endl;
	});
}

huser::~huser()
{
	if (socket_->is_open())
	{
		socket_->close();
		socket_ = nullptr;
	}
}

int huser::size_to_int(const size_t u)
{
	if (u > std::numeric_limits<int>::max())
	{
		throw std::overflow_error("size_t cannot be stored in a variable of type int.");
	}

	return static_cast<int>(u);
}
