#include <iostream>
#include <thread>

#include "asio.hpp"
#include "flic.hpp"
#include "external/cppformat/cppformat/format.h"
#include "connection.h"

#include <digitalSTROM/dsuid.h>

class ButtonEventListener: public flic::client::button::ButtonEventListener {
public:
	ButtonEventListener(Connection &connection, const std::string& deviceId) :
			connection_(connection), id_(deviceId) {
		std::cout << deviceId << " collected" << std::endl;

		dsuid_generate_v3_from_namespace(DSUID_NS_IEEE_MAC, id_.c_str(),
				&dsuid_);

		connection_.write(
				fmt::format(
						"{{'message':'init',"
								" 'protocol':'json',"
								" 'tag':'{}',"
								" 'uniqueid':'{}',"
								" 'group': 8,"
								" 'hardwarename':'Flic button',"
								" 'modelname':'Flic button',"
								" 'name':'Flic',"
								" 'buttons':[{{'id':0,'buttontype':1,'group':1}}],"
								" 'inputs': [{{'inputtype':12,'usage':1}},{{'inputtype':1,'usage':1}}]"
								"}}\n", id_, strdsuid(&dsuid_)));
		onConnect(deviceId);
		onBatteryStatus(deviceId, 100);
	}

	virtual std::string getHash() override {
		return "main";
	}

	virtual void onButtonUpOrDown(const std::string& deviceId,
			const bool& queued, const int& timeDiff, const bool& isUp,
			const bool& isDown) override {
		//ignore any clicks that are older than 15 seconds
		if (queued && (timeDiff > 15)) {
			return;
		}
		std::cout << deviceId << (isUp ? " up" : " down") << std::endl;
		connection_.write(fmt::format("{{'message':'button',"
				" 'index':0,"
				" 'value':{},"
				" 'tag':'{}'"
				"}}\n", (isUp ? 0 : 1), id_));
	}

	virtual void onConnect(const std::string& deviceId) override {
		std::cout << deviceId << " ready" << std::endl;
		connection_.write(fmt::format("{{'message':'input',"
				" 'index':1,"
				" 'value':1,"
				" 'tag':'{}'"
				"}}\n", id_));
	}

	virtual void onDisconnect(const std::string& deviceId) override {
		std::cout << deviceId << " disconnected" << std::endl;
		connection_.write(fmt::format("{{'message':'input',"
				" 'index':1,"
				" 'value':0,"
				" 'tag':'{}'"
				"}}\n", id_));
	}

	virtual void onBatteryStatus(const std::string& deviceId,
			const int& battery) override {
		std::cout << deviceId << " battery is: " << battery << std::endl;
		connection_.write(fmt::format("{{'message':'input',"
				" 'index':0,"
				" 'value':{},"
				" 'tag':'{}'"
				"}}\n", (battery < 10 ? 1 : 0), id_));
	}

private:
	Connection& connection_;
	std::string id_;
	dsuid_t dsuid_;
};

class ButtonListener: public flic::client::manager::ButtonListener {
private:
	std::shared_ptr<flic::client::manager::Manager> manager;
public:
	ButtonListener(Connection &connection,
			std::shared_ptr<flic::client::manager::Manager> manager) :
			connection_(connection), manager(manager) {
	}

	virtual std::string getHash() override {
		return "main";
	}

	virtual void onButtonDiscover(const std::string& deviceId) override {
		auto button = manager->getButton(deviceId);
		button->addButtonEventListener(
				std::shared_ptr<flic::client::button::ButtonEventListener>(
						new ButtonEventListener(connection_, deviceId)));
	}

private:
	Connection& connection_;
};

int main() {
	flic::client::Client client;

	try {
		asio::io_service io_service;

		Connection connection(io_service, { "localhost", "8999" });

		std::thread t([&io_service]() {
			std::cout << "ASIO thread started" << std::endl;
			io_service.run();
			std::cout << "ASIO thread exited" << std::endl;
		});

		client.start(
				[&client, &connection] () {
					std::cout << "Initialized" << std::endl;
					auto manager = client.getManager();

					manager->addButtonListener(
							std::shared_ptr<flic::client::manager::ButtonListener>(
									new ButtonListener(connection, manager)));

					for (auto& button : manager->getButtons()) {
						button->addButtonEventListener(std::shared_ptr<flic::client::button::ButtonEventListener>(
										new ButtonEventListener(connection, button->getDeviceId())));
					}
				}, [&client] (const bool& resumable) {
					std::cout << "Uninitialized" << std::endl;
				});
		client.run();
		t.join();
	} catch (flic::client::ClientNetworkException& e) {
		std::cerr << e.what() << std::endl;
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	} catch (...) {
		std::cerr << "Unknown exception" << std::endl;
	}
}
