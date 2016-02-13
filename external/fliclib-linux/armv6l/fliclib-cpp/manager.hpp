#ifndef DEFINE_FLIC_CLIENT_MANAGER_HPP
#define DEFINE_FLIC_CLIENT_MANAGER_HPP

#include <map>
#include <memory>
#include <vector>

#include "button.hpp"

namespace flic {
    namespace client {
	class Client;

	namespace manager {
	    class ButtonListener;
	    class ManagerListener;

	    class Manager :
		public std::enable_shared_from_this<Manager> {
		friend class flic::client::Client;
	    private:
		bool dangling;
		flic::client::Client& client;
		bool scanning;
		bool bluetoothState;
		std::map<std::string, std::shared_ptr<flic::client::button::Button>> buttons;
		std::map<std::string, std::shared_ptr<ButtonListener>> buttonListeners;
		std::map<std::string, std::shared_ptr<ManagerListener>> managerListeners;
	    public:
		~Manager();
	    protected:
		Manager(flic::client::Client& client,
			const bool& scanning,
			const bool& bluetoothState);
		void markAsDangling();
		void addButton(const std::shared_ptr<flic::client::button::Button>& button);
	    public:
		void startScan();
		void stopScan();
	    public:
		std::vector<std::shared_ptr<flic::client::button::Button>> getButtons();
		std::shared_ptr<flic::client::button::Button> getButton(const std::string& buttonId);
	    public:
		void addButtonListener(const std::shared_ptr<ButtonListener>& listener);
		void addManagerListener(const std::shared_ptr<ManagerListener>& listener);
		void removeButtonListener(const std::string& hash);
		void removeManagerListener(const std::string& hash);
	    };

	    class ButtonListener {
	    public:
		virtual ~ButtonListener();
		virtual std::string getHash();
		virtual void onButtonDiscover(const std::string& deviceId);
		virtual void onButtonForgotten(const std::string& deviceId);
	    };

	    class ManagerListener {
	    public:
		virtual ~ManagerListener();
		virtual std::string getHash();
		virtual void onBluetoothStateChange(const bool& state);
		virtual void onScanningStarted();
		virtual void onScanningStopped();
	    };
	}
    }
}

#endif
