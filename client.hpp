#ifndef DEFINE_FLIC_CLIENT_HPP
#define DEFINE_FLIC_CLIENT_HPP

#include <functional>
#include <map>
#include <memory>
#include <netdb.h>
#include <vector>

#include "button.hpp"
#include "manager.hpp"

namespace flic {
    namespace protocol {
	class Message;
    }

    namespace client {

	class Client {
	    friend class flic::client::button::Button;
	    friend class flic::client::manager::Manager;
	private:
	    int sockfd;
	    std::string hostname;
	    short port;
	private:
	    bool started;
	    bool initialized;
	    std::shared_ptr<flic::client::manager::Manager> manager;
	    std::function<void()> onInitialized;
	    std::function<void(const bool&)> onUninitialized;
	public:
	    Client();
	    ~Client();
	    Client(const std::string& hostname, short port);
	    std::shared_ptr<flic::client::manager::Manager> getManager();
	public:
	    void run();
	    void start(const std::function<void()>& onInitialized,
		       const std::function<void(const bool&)>& onUninitialized);
	private:
	    void handleMessage(const std::shared_ptr<flic::protocol::Message>& message);
	};

	class ClientException : public std::exception {
	public:
	    explicit ClientException(const std::string& message):
		msg(message) {
	    }

	    virtual ~ClientException() throw () {

	    }

	    virtual const char* what() const throw () {
		return msg.c_str();
	    }

	protected:
	    std::string msg;
	};

	class ClientNetworkException : public ClientException {
	public:
	    explicit ClientNetworkException(const std::string& message) :
		ClientException(message) {
	    }

	    virtual ~ClientNetworkException() throw () {

	    }
	};
    }
}

#endif
