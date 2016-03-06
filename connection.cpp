/*
 * connection.cpp
 *
 *  Created on: 20.02.2016
 *      Author: chhitz
 */
#include <iostream>

#include "asio.hpp"
#include "connection.h"

Connection::Connection(asio::io_service& io_service,
		const tcp::resolver::query& query) :
		io_service_(io_service), socket_(io_service) {
	do_connect(query);
}

void Connection::write(const std::string& msg) {
	std::cout << "write: " << msg << std::endl;
	io_service_.post([this, msg]()
	{
		bool write_in_progress = !write_msgs_.empty();
		write_msgs_.push_back(msg);
		if (!write_in_progress)
		{
			do_write();
		}
	});
}

void Connection::do_connect(const tcp::resolver::query& query) {
	tcp::resolver resolver(io_service_);
	auto endpoint_iterator = resolver.resolve(query);
	auto endpt_iter = endpoint_iterator;

#ifndef NDEBUG
	while (endpt_iter != asio::ip::tcp::resolver::iterator()) {
		auto endpoint = *endpt_iter++;
		std::cout << endpoint.endpoint() << std::endl;
	}
#endif

	asio::async_connect(socket_, endpoint_iterator,
			[this](std::error_code ec, tcp::resolver::iterator)
			{
				if (!ec)
				{
					std::cout << "connected to vdcd" << std::endl;
					do_read();
				} else {
					std::cout << "connection to vdcd failed with: " << ec << std::endl;
				}
			});
}

void Connection::do_read() {
	asio::async_read(socket_, asio::buffer(read_msg_),
			[this](std::error_code ec, std::size_t /*length*/)
			{
				if (!ec)
				{
					std::cout << "read: " << read_msg_ << std::endl;
					do_read();
				}
				else
				{
					socket_.close();
				}
			});
}

void Connection::do_write() {
	asio::async_write(socket_,
			asio::buffer(write_msgs_.front().data(),
					write_msgs_.front().length()),
			[this](std::error_code ec, std::size_t /*length*/)
			{
				if (!ec)
				{
					write_msgs_.pop_front();
					if (!write_msgs_.empty())
					{
						do_write();
					}
				}
				else
				{
					std::cout << "write failed with: " << ec << std::endl;
					socket_.close();
				}
			});
}
