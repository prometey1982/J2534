#pragma once

#include "J2534.hpp"

#include "BaseMessage.hpp"

#include <functional>

namespace j2534 {
	class J2534Channel final {
	public:
		explicit J2534Channel(J2534& j2534, unsigned long ProtocolID,
			unsigned long Flags, unsigned long Baudrate,
			unsigned long TxFlags);
        J2534Channel(const J2534Channel&) = delete;
        J2534Channel(J2534Channel&&) = delete;
		~J2534Channel();

        J2534Channel& operator=(const J2534Channel&) = delete;

		J2534_ERROR_CODE readMsgs(std::vector<PASSTHRU_MSG>& msgs,
			unsigned long Timeout = 1000) const;
		J2534_ERROR_CODE writeMsgs(const std::vector<PASSTHRU_MSG>& msgs,
			unsigned long& numMsgs,
			unsigned long Timeout = 1000) const;
		J2534_ERROR_CODE writeMsgs(const std::vector<BaseMessage*>& msgs,
			unsigned long& numMsgs,
			unsigned long Timeout = 1000) const;
		J2534_ERROR_CODE writeMsgs(const BaseMessage& msgs, unsigned long& numMsgs,
			unsigned long Timeout = 1000) const;
		J2534_ERROR_CODE writeMsg(const std::vector<uint8_t>& data, unsigned long Timeout = 1000) const;
		J2534_ERROR_CODE startPeriodicMsg(const PASSTHRU_MSG& msg,
			unsigned long& msgID,
			unsigned long TimeInterval) const;
		std::vector<unsigned long> startPeriodicMsgs(const BaseMessage& msg, unsigned long TimeInterval) const;
		J2534_ERROR_CODE stopPeriodicMsg(unsigned long MsgID) const;
		void stopPeriodicMsg(const std::vector<unsigned long>& ids) const;
		J2534_ERROR_CODE startMsgFilter(unsigned long FilterType,
			PASSTHRU_MSG* maskMsg,
			PASSTHRU_MSG* patternMsg,
			PASSTHRU_MSG* flowControlMsg,
			unsigned long& msgID) const;
		J2534_ERROR_CODE stopMsgFilter(unsigned long MsgID) const;
		J2534_ERROR_CODE passThruIoctl(unsigned long IoctlID, const void* input,
			void* output = nullptr) const;
		J2534_ERROR_CODE clearRx() const;
		J2534_ERROR_CODE clearTx() const;

		J2534_ERROR_CODE setConfig(const std::vector<SCONFIG>& config) const;

		unsigned long getProtocolId() const;
		unsigned long getTxFlags() const;
		unsigned long getBaudrate() const;

        /**
         * @brief Read messages from the channel while func returns true.
         * @param func - Function which called on message receiving.
         * @param Timeout - Read message timeout.
         */
        void readMsgs(const std::function<bool(const uint8_t* data, size_t length)> &func, unsigned long timeout = 1000) const;

	private:
		J2534& _j2534;
		unsigned long _protocolId;
		unsigned long _txFlags;
		unsigned long _baudrate;

		unsigned long _channelID;
	};
} // namespace j2534
