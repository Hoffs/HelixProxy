#include "HUser.h"
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <evpp/buffer.h>
#include <evpp/tcp_conn.h>
#include <glog/logging.h>
#include <evpp/event_loop.h>
#include <evpp/event_loop_thread.h>

helix_user::helix_user(evpp::EventLoop *loop, const int port, const std::function<void(const size_t length, const char *message)> &callback)
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

	client_->set_connecting_timeout(evpp::Duration(10.0));
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
	LOG(INFO) << "Sending message to TCP server with length " << length << " from user: " << this->info(); 
	if (client_->conn() == nullptr || client_->conn()->IsDisconnected()) return;
	auto *len = new unsigned char[4]; // Length part of the packet
	auto *full = new unsigned char [4 + length]; // Full packet

	memcpy(len, &length, sizeof(length));
	memcpy(full, len, 4);
	memcpy(full + 4, message, length);

	client_->conn()->Send(full, 4 + length);
	LOG(INFO) << "Sent message to TCP server with length " << length << " from user: " << this->info(); 
}


void helix_user::message_callback(const evpp::TCPConnPtr& conn, evpp::Buffer *msg) const
{
	LOG(INFO) << "Received message from TCP server with length " << msg->length() << " for user: " << this->info(); 
	msg->Skip(4); // Skip 4 size bytes;
	const auto message = new char[msg->length()];
	const auto len = msg->length();
	memcpy(message, msg->data(), msg->length());
	msg->Skip(msg->length());
	callback_(len, message);
}

helix_user::~helix_user()
{
	client_->SetMessageCallback(nullptr);
	client_->SetConnectionCallback(nullptr);
	
	if (client_->conn() != nullptr && (!client_->conn()->IsDisconnected()))
	{
		auto client = client_.get();
		client_.release();
		
		client->SetConnectionCallback([client](const evpp::TCPConnPtr &conn)
		{
			delete client;
			//loop_->RunInLoop([client]()
			//{
				// this->client_ = nullptr;
			//});
		});

		// if (client_->conn() != nullptr && client_->conn()->IsConnected())
		// {
		// 	loop_->RunAfter(evpp::Duration(1.5), [this]() { client_->Disconnect();});
		// }
		loop_->RunInLoop([client]()
		{
			client->Disconnect();
		});
		// client_ = nullptr;
	}

	callback_ = nullptr;
}
