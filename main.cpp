#include <iostream>
#include <deque>
#include <array>

#include "asio.hpp"
#include "flic.hpp"
#include "format.h"

#include <dsuid.h>

using asio::ip::tcp;

typedef std::deque<std::string> message_queue;

class ButtonEventListener: public flic::client::button::ButtonEventListener {
public:
	ButtonEventListener(asio::io_service& io_service,
			const std::string& deviceId) :
			io_service_(io_service), socket_(io_service), id_(deviceId) {
		dsuid_generate_v3_from_namespace(DSUID_NS_IEEE_MAC, id_.c_str(),
				&dsuid_);

		write(fmt::format("{'message':'init',"
				" 'protocol':'json',"
				" 'tag':'{}',"
				" 'uniqueid':'{}',"
				" 'group': 8,"
				" 'hardwarename':'Flic button',"
				" 'name':'Flic',"
				" 'buttons':[{'id':0}],"
				" 'inputs': [{'inputtype':12},{'inputtype':1}]"
				"}", id_, strdsuid(&dsuid_)));
	}

	virtual std::string getHash() override {
		return "main";
	}

	virtual void onButtonUpOrDown(const std::string& deviceId,
			const bool& queued, const int& timeDiff, const bool& isUp,
			const bool& isDown) override {
		std::cout << deviceId << (isUp ? " up" : " down") << std::endl;
		write(fmt::format("{'message':'button',"
				" 'index':0,"
				" 'value':{},"
				" 'tag':'{}'"
				"}", (isUp ? 1 : 0), id_));
	}

	virtual void onReady(const std::string& deviceId) override {
		std::cout << deviceId << " ready" << std::endl;
		write(fmt::format("{'message':'input',"
				" 'index':1,"
				" 'value':1,"
				" 'tag':'{}'"
				"}", id_));
	}

	virtual void onDisconnect(const std::string& deviceId) override {
		std::cout << deviceId << " disconnected" << std::endl;
		write(fmt::format("{'message':'input',"
				" 'index':1,"
				" 'value':0,"
				" 'tag':'{}'"
				"}", id_));
	}

	virtual void onBatteryStatus(const std::string& deviceId,
			const int& battery) override {
		std::cout << deviceId << " battery is: " << battery << std::endl;
		write(fmt::format("{'message':'input',"
				" 'index':0,"
				" 'value':{},"
				" 'tag':'{}'"
				"}", (battery < 10 ? 1 : 0), id_));
	}

private:
	void write(const std::string& msg) {
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

	void do_write() {
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
						socket_.close();
					}
				});
	}

private:
	asio::io_service& io_service_;
	tcp::socket socket_;
	message_queue write_msgs_;
	std::string id_;
	dsuid_t dsuid_;
};

class ButtonListener: public flic::client::manager::ButtonListener {
private:
	std::shared_ptr<flic::client::manager::Manager> manager;
public:
	ButtonListener(std::shared_ptr<flic::client::manager::Manager> manager,
			asio::io_service& io_service,
			tcp::resolver::iterator endpoint_iterator) :
			manager(manager), io_service_(io_service), socket_(io_service) {

	}

	virtual std::string getHash() override {
		return "main";
	}

	virtual void onButtonDiscover(const std::string& deviceId) override {
		auto button = manager->getButton(deviceId);
		button->addButtonEventListener(
				std::shared_ptr<flic::client::button::ButtonEventListener>(
						new ButtonEventListener(io_service_, deviceId)));
	}

private:
	void do_connect(tcp::resolver::iterator endpoint_iterator) {
		asio::async_connect(socket_, endpoint_iterator,
				[this](std::error_code ec, tcp::resolver::iterator)
				{
					if (!ec)
					{
						do_read();
					}
				});
	}

	void do_read() {
		asio::async_read(socket_, asio::buffer(read_msg_),
				[this](std::error_code ec, std::size_t /*length*/)
				{
					if (!ec)
					{
						std::cout << read_msg_ << std::endl;
						do_read();
					}
					else
					{
						socket_.close();
					}
				});
	}

private:
	asio::io_service& io_service_;
	tcp::socket socket_;
	char read_msg_[128];
};

int main() {
	flic::client::Client client;

	try {
		asio::io_service io_service;

		tcp::resolver resolver(io_service);
		auto endpoint_iterator = resolver.resolve( { "localhost", "8999" });

		std::thread t([&io_service]() {io_service.run();});

		client.getManager()->addButtonListener(
				std::shared_ptr<flic::client::manager::ButtonListener>(
						new ButtonListener(client.getManager(), io_service,
								endpoint_iterator)));

		client.start(
				[&client] () {
					std::cout << "Initialized" << std::endl;
					auto manager = client.getManager();

					for (auto& button : manager->getButtons()) {
						button->addButtonEventListener(std::shared_ptr<flic::client::button::ButtonEventListener>(
								new ButtonEventListener(io_service, button->getDeviceId())));
					}
				}, [&client] (const bool& resumable) {
					std::cout << "Uninitialized" << std::endl;
				});
		client.run();
	} catch (flic::client::ClientNetworkException& e) {
		std::cerr << e.what() << std::endl;
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	} catch (...) {
		std::cerr << "Unknown exception" << std::endl;
	}
}