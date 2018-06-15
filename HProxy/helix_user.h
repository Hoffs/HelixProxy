#pragma once
#include <evpp/tcp_client.h>

class helix_user
{
private:
	std::unique_ptr<evpp::TCPClient> client_;
	std::string client_ip_;
	std::function<void(size_t length, char *message)> callback_;
	evpp::EventLoop* loop_;
	const int max_attempts_ = 5;
	bool is_deleted_ = false;
	char *server_ip_;
public:
	helix_user(evpp::EventLoop *loop, std::string ip, char *server_ip, int server_port, const std::function<void(size_t length, char *message)> &callback);
	~helix_user();
	void connect_handler(const evpp::TCPConnPtr &conn) const;
	std::string info() const;
	void send_async(size_t length, char* message) const;
	void send_async(size_t length, char* message, int attempt) const;
	void message_callback(const evpp::TCPConnPtr &conn, evpp::Buffer *msg) const;
};