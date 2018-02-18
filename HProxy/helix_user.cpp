#include "helix_user.h"
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <evpp/buffer.h>
#include <evpp/tcp_conn.h>
#include <glog/logging.h>
#include <evpp/event_loop.h>
#include <evpp/event_loop_thread.h>

helix_user::helix_user(evpp::EventLoop *loop, const int port, const std::function<void(size_t length, std::shared_ptr<char> message)> &callback)
{
	callback_ = callback;
	loop_ = loop;
	const auto conn_string = "127.0.0.1:" + std::to_string(port);
	id_ = boost::uuids::uuid(boost::uuids::random_generator()());
	client_ = std::make_unique<evpp::TCPClient>(loop, conn_string, to_string(id_));
	
	client_->SetConnectionCallback([this](const evpp::TCPConnPtr &conn)
	{
		this->connect_handler(conn);
	});

	client_->SetMessageCallback([this](const evpp::TCPConnPtr &conn, evpp::Buffer *msg)
	{
		this->message_callback(conn, msg);
	});

	client_->set_connecting_timeout(evpp::Duration(5.0));
	client_->set_reconnect_interval(evpp::Duration(5.0));
	client_->Connect();
}

void helix_user::connect_handler(const evpp::TCPConnPtr &conn) const
{
	if (conn->IsConnected())
	{
		LOG(INFO) << "Connected to TCP Server. User: " << this->info();
	} else
	{
		LOG(ERROR) << "Failed to connect to TCP Server or disconnected.";
	}
}

std::string helix_user::info() const
{
	return to_string(id_);
}

void helix_user::send_async(const size_t length, char* message) const
{
	this->send_async(length, message, 0);
}

void helix_user::send_async(const size_t length, char* message, int attempt) const
{
	LOG(INFO) << "Sending message to TCP server with length " << length << " from user: " << this->info() << " . Attempt: " << attempt; 
	if (attempt > max_attempts_ || is_deleted_) return;
	if (client_->conn() == nullptr || !client_->conn()->IsConnected())
	{
		loop_->RunAfter(evpp::Duration(1.0 * (attempt + 1)), [this, length, message, attempt]()
		{
			this->send_async(length, message, attempt + 1);
		});
		return;
	};
	auto *full = new char[4 + length]; // Full packet

	memcpy(full, &length, 4);
	memcpy(full + 4, message, length);

	client_->conn()->Send(full, 4 + length);
	LOG(INFO) << "Sent message to TCP server with length " << length << " from user: " << this->info() << " . On attempt: " << attempt; 
	delete[] full;
	delete[] message;
}

void helix_user::message_callback(const evpp::TCPConnPtr& conn, evpp::Buffer *msg) const
{
	LOG(INFO) << "Received message from TCP server with length " << msg->length() << " for user: " << this->info(); 
	msg->Skip(4); // Skip 4 size bytes;
	auto message = std::make_shared<char>(msg->length());
	const auto len = msg->length();
	memcpy(message.get(), msg->data(), msg->length());
	msg->Skip(msg->length());
	callback_(len, message);
}

helix_user::~helix_user()
{
	is_deleted_ = true;
	if (client_->conn() != nullptr && (!client_->conn()->IsDisconnected())) // If connected
	{
		auto client = client_.get();
		client_.release();
		
		client->SetConnectionCallback([client](const evpp::TCPConnPtr &conn)
		{
			delete client;
		});

		client->Disconnect();
	} else // If not yet connected
	{
		auto client = client_.get();
		client_.release();
		
		client->Disconnect();
		loop_->RunAfter(evpp::Duration(10.0), [client]()
		{
			delete client;
		});
	}
	callback_ = nullptr;
}
