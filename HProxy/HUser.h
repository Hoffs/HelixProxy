#pragma once
#include <boost/asio.hpp>
#include <boost/uuid/uuid.hpp>

using boost::asio::ip::tcp;

class huser
{
private:
	std::shared_ptr<tcp::socket> socket_;
	boost::uuids::uuid id_{};
	static int size_to_int(size_t u);
public:
	static void connect_handler(const boost::system::error_code& ec);
	std::string info() const;
	huser(boost::asio::io_service &io_service);
	void send_async(const size_t length, char* message) const;
	~huser();
};
