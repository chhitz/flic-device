/*
 * connection.h
 *
 *  Created on: 20.02.2016
 *      Author: chhitz
 */

#ifndef CONNECTION_H_
#define CONNECTION_H_

#include <deque>

using asio::ip::tcp;

typedef std::deque<std::string> message_queue;

class Connection {
public:
	Connection(asio::io_service& io_service,
			const tcp::resolver::query& query);

	void write(const std::string& msg);

private:
	void do_connect(const tcp::resolver::query& query);
	void do_read();
	void do_write();

private:
	asio::io_service& io_service_;
	tcp::socket socket_;
	char read_msg_[128];
	message_queue write_msgs_;
};

#endif /* CONNECTION_H_ */
