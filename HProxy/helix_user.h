#pragma once
#include <boost/uuid/uuid.hpp>
#include <evpp/tcp_client.h>

class helix_user
{
private:
	std::unique_ptr<evpp::TCPClient> client_;
	boost::uuids::uuid id_{};
	std::function<void(size_t length, char *message)> callback_;
	evpp::EventLoop* loop_;
	const int max_attempts_ = 5;
	bool is_deleted_ = false;
public:
	helix_user(evpp::EventLoop *loop, int port, const std::function<void(size_t length, char *message)> &callback);
	~helix_user();
	void connect_handler(const evpp::TCPConnPtr &conn) const;
	std::string info() const;
	void send_async(const size_t length, char* message) const;
	void send_async(const size_t length, char* message, int attempt) const;
	void message_callback(const evpp::TCPConnPtr &conn, evpp::Buffer *msg) const;
};