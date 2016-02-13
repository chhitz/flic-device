#ifndef DEFINE_FLIC_CLIENT_BUTTON_HPP
#define DEFINE_FLIC_CLIENT_BUTTON_HPP

#include <map>
#include <memory>
#include <string>

namespace flic {
    namespace client {
	class Client;

	namespace manager {
	    class Manager;
	}

	namespace button {
	    class ButtonEventListener;

	    enum ButtonState {
		ButtonStateConnecting = 0,
		ButtonStateConnected,
		ButtonStateReady,
		ButtonStateDisconnecting,
		ButtonStateDisconnected };

	    std::string stateToString(const flic::client::button::ButtonState& buttonState);

	    class Button :
		public std::enable_shared_from_this<Button> {
		friend class client::Client;
		friend class client::manager::Manager;
	    private:
		bool dangling;
		client::Client& client;
		const std::string deviceId;
		ButtonState buttonState;
		std::map<std::string, std::shared_ptr<ButtonEventListener>> buttonEventListeners;
	    protected:
		Button(client::Client& client,
		       const std::string& deviceId,
		       const ButtonState& buttonState);
		void setButtonState(const ButtonState& buttonState);
		void markAsDangling();
	    public:
		Button(Button const&)         = delete;
		void operator=(Button const&) = delete;
	    public:
		const std::string& getDeviceId();
		const ButtonState& getButtonState();
		void addButtonEventListener(const std::shared_ptr<ButtonEventListener>& listener);
		void removeButtonEventListener(const std::string& hash);
	    public:
		void connect();
		void disconnect();
	    };

	    class ButtonEventListener {
	    public:
		virtual ~ButtonEventListener();
		virtual std::string getHash();

		virtual void onConnecting(const std::string& deviceId);
		virtual void onConnect(const std::string& deviceId);
		virtual void onReady(const std::string& deviceId);
		virtual void onDisconnecting(const std::string& deviceId);
		virtual void onDisconnect(const std::string& deviceId);
		virtual void onConnectionFail(const std::string& deviceId);
		virtual void onButtonUpOrDown(const std::string& deviceId,
					      const bool& queued,
					      const int& timeDiff,
					      const bool& isUp,
					      const bool& isDown);
		virtual void onButtonClickOrHold(const std::string& deviceId,
						 const bool& queued,
						 const int& timeDiff,
						 const bool& isClick,
						 const bool& isHold);
		virtual void onButtonSingleOrDoubleClick(const std::string& deviceId,
							 const bool& queued,
							 const int& timeDiff,
							 const bool& isSingleClick,
							 const bool& isDoubleClick);
		virtual void onButtonSingleOrDoubleClickOrHold(const std::string& deviceId,
							       const bool& queued,
							       const int& timeDiff,
							       const bool& isSingleClick,
							       const bool& isDoubleClick,
							       const bool& isHold);
		virtual void onBatteryStatus(const std::string& deviceId,
					     const int& battery);
		virtual void onRssiValue(const std::string& deviceId,
					 const int& rssi);
	    };
	}
    }
}
#endif
