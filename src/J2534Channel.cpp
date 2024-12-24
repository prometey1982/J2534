#include "j2534/J2534Channel.hpp"

#include <stdexcept>

namespace j2534 {

	namespace {

		std::vector<PASSTHRU_MSG> toPassThruMsgs(const std::vector<uint8_t>& data, unsigned long protocolId, unsigned long txFlags) {
			PASSTHRU_MSG msg;
			memset(&msg, 0, sizeof(msg));
			msg.ProtocolID = protocolId;
			msg.TxFlags = txFlags;
			msg.DataSize = data.size();
			for (size_t i = 0; i < data.size(); ++i) {
				msg.Data[i] = data[i];
			}
			return { msg };
		}

	}

	J2534Channel::J2534Channel(J2534& j2534, unsigned long ProtocolID,
		unsigned long Flags, unsigned long Baudrate,
		unsigned long TxFlags)
		: _j2534{ j2534 }, _protocolId(ProtocolID), _txFlags(TxFlags) {
		const auto result =
			_j2534.PassThruConnect(ProtocolID, Flags, Baudrate, _channelID);
		if (result != STATUS_NOERROR) {
			std::string err;
			_j2534.PassThruGetLastError(err);
			throw std::runtime_error("Can't open channel: " + err);
		}
	}

	J2534Channel::~J2534Channel() { _j2534.PassThruDisconnect(_channelID); }

	J2534_ERROR_CODE J2534Channel::readMsgs(std::vector<PASSTHRU_MSG>& msgs,
		unsigned long Timeout) const {
		return _j2534.PassThruReadMsgs(_channelID, msgs, Timeout);
	}

	J2534_ERROR_CODE J2534Channel::writeMsgs(const std::vector<PASSTHRU_MSG>& msgs,
		unsigned long& numMsgs,
		unsigned long Timeout) const {
		return _j2534.PassThruWriteMsgs(_channelID, msgs, numMsgs, Timeout);
	}

	J2534_ERROR_CODE J2534Channel::writeMsgs(const std::vector<BaseMessage*>& msgs,
		unsigned long& numMsgs,
		unsigned long Timeout) const {
		std::vector<PASSTHRU_MSG> messages;
		for (const auto& msg : msgs) {
			for (auto& passMsg : msg->toPassThruMsgs(_protocolId, _txFlags))
				messages.emplace_back(std::move(passMsg));
		}
		return _j2534.PassThruWriteMsgs(_channelID, messages, numMsgs, Timeout);
	}

	J2534_ERROR_CODE J2534Channel::writeMsgs(const BaseMessage& msg,
		unsigned long& numMsgs,
		unsigned long Timeout) const {
		std::vector<PASSTHRU_MSG> messages;
		for (auto& passMsg : msg.toPassThruMsgs(_protocolId, _txFlags))
			messages.emplace_back(std::move(passMsg));
		return _j2534.PassThruWriteMsgs(_channelID, messages, numMsgs, Timeout);
	}

	J2534_ERROR_CODE J2534Channel::writeMsg(const std::vector<uint8_t>& data,
		unsigned long Timeout) const {
		unsigned long numMsgs = 1;
		return _j2534.PassThruWriteMsgs(_channelID, toPassThruMsgs(data, _protocolId, _txFlags), numMsgs, Timeout);
	}

	J2534_ERROR_CODE
		J2534Channel::startPeriodicMsg(const PASSTHRU_MSG& msg, unsigned long& msgID,
			unsigned long TimeInterval) const {
		return _j2534.PassThruStartPeriodicMsg(_channelID, msg, msgID, TimeInterval);
	}

	std::vector<unsigned long> J2534Channel::startPeriodicMsgs(const BaseMessage& msg,
		unsigned long TimeInterval) const {
		std::vector<PASSTHRU_MSG> messages;
		for (auto& passMsg : msg.toPassThruMsgs(_protocolId, _txFlags))
			messages.emplace_back(std::move(passMsg));
		std::vector<unsigned long> result;
		for (const auto& message : messages) {
			unsigned long msgID;
			if (_j2534.PassThruStartPeriodicMsg(_channelID, message, msgID, TimeInterval)
				== STATUS_NOERROR) {
				result.push_back(msgID);
			}
		}
		return result;
	}

	J2534_ERROR_CODE J2534Channel::stopPeriodicMsg(unsigned long MsgID) const {
		return _j2534.PassThruStopPeriodicMsg(_channelID, MsgID);
	}

	void J2534Channel::stopPeriodicMsg(const std::vector<unsigned long>& ids) const {
		for (const auto& messageId : ids) {
			_j2534.PassThruStopPeriodicMsg(_channelID, messageId);
		}
	}

	J2534_ERROR_CODE J2534Channel::startMsgFilter(unsigned long FilterType,
		PASSTHRU_MSG* maskMsg,
		PASSTHRU_MSG* patternMsg,
		PASSTHRU_MSG* flowControlMsg,
		unsigned long& msgID) const {
		return _j2534.PassThruStartMsgFilter(_channelID, FilterType, maskMsg,
			patternMsg, flowControlMsg, msgID);
	}

	J2534_ERROR_CODE J2534Channel::stopMsgFilter(unsigned long MsgID) const {
		return _j2534.PassThruStopMsgFilter(_channelID, MsgID);
	}

	J2534_ERROR_CODE J2534Channel::passThruIoctl(unsigned long IoctlID,
		const void* input,
		void* output) const {
		return _j2534.PassThruIoctl(_channelID, IoctlID, input, output);
	}

	J2534_ERROR_CODE J2534Channel::clearRx() const {
		return passThruIoctl(CLEAR_RX_BUFFER, nullptr, nullptr);
	}

	J2534_ERROR_CODE J2534Channel::clearTx() const {
		return passThruIoctl(CLEAR_TX_BUFFER, nullptr, nullptr);
	}

	J2534_ERROR_CODE
		J2534Channel::setConfig(const std::vector<SCONFIG>& config) const {
		SCONFIG_LIST configList;
		configList.NumOfParams = config.size();
		configList.ConfigPtr = const_cast<SCONFIG*>(config.data());
		return passThruIoctl(SET_CONFIG, &configList);
	}

	unsigned long J2534Channel::getProtocolId() const {
		return _protocolId;
	}

	unsigned long J2534Channel::getTxFlags() const {
		return _txFlags;
	}

} // namespace j2534
