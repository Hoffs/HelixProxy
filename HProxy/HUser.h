#pragma once
#include <boost/asio.hpp>
#include <boost/uuid/uuid.hpp>
#include <evpp/tcp_client.h>

using boost::asio::ip::tcp;

class huser
{
private:
	evpp::TCPClient *client_;
	boost::uuids::uuid id_{};
	static int size_to_int(size_t u);
	std::function<void(size_t length, const char *message)> callback_;
public:
	huser(evpp::EventLoop *loop, int port, const std::function<void(size_t length, const char *message)> &callback);
	~huser();
	void connect_handler(const evpp::TCPConnPtr &conn);
	std::string info() const;
	void send_async(const size_t length, char* message) const;
	void message_callback(const evpp::TCPConnPtr &conn, evpp::Buffer *msg);
};
