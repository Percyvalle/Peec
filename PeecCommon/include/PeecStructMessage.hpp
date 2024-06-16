#pragma once

#include <boost/format.hpp>
#include <boost/date_time.hpp>

#include "PeecMessageStatus.hpp"
#include "PeecCommonHeaders.hpp"

namespace Net {

	template<typename TypeMsg>
	struct HeaderMessage {
		TypeMsg type;
		MessageStatus status;
		uint32_t sizeData = 0;
	};

	struct BodyMessage {
		std::vector<uint8_t> data;
	};

	template<typename T>
	struct Message {
		HeaderMessage<T> header;
		BodyMessage body;

		T GetType() { return header.type; }
		void SetType(T _type) { header.type = _type; }

		MessageStatus GetStatus() { return header.status; }
		void SetStatus(MessageStatus _status) { header.status = _status; }

		void Clear()
		{
			body.data.clear();
			header.sizeData = body.data.size();
		}

		bool IsValidate() const
		{
			return JSON::accept(GetStrData());
		}

		std::string GetStrData() const 
		{
			if (!body.data.empty()) {
				std::string ret;
				ret.resize(body.data.size());
				memcpy(ret.data(), body.data.data(), body.data.size());

				return ret;
			}

			return "";
		}

		JSON GetJSONData() const
		{
			return IsValidate() ? JSON::parse(GetStrData()) : JSON();
		}

		template<typename TypeMsg>
		Message& operator<<(const TypeMsg& _msg)
		{
			static_assert(std::is_standard_layout<TypeMsg>::value, "Data is too complex to be pushed into vector");

			size_t startPtr = body.data.size();

			body.data.resize(startPtr + sizeof(TypeMsg));

			std:memcpy(body.data.data() + startPtr, &_msg, sizeof(TypeMsg));

			header.sizeData = body.data.size();

			return *this;
		}

		template<>
		Message& operator<<(const std::vector<std::uint8_t>& _message)
		{
			size_t startPtr = body.data.size();

			body.data.resize(startPtr + _message.size());

			std:memcpy(body.data.data() + startPtr, _message.data(), _message.size());

			header.sizeData = body.data.size();

			return *this;
		}

		template<>
		Message& operator<<(const std::string& _message) 
		{
			size_t startPtr = body.data.size();

			body.data.resize(startPtr + _message.size());

			std:memcpy(body.data.data() + startPtr, _message.data(), _message.size());

			header.sizeData = body.data.size();

			return *this;
		}
	};

	template<typename T>
	class Connection;

	template<typename T>
	struct 	OwnerMessage {
		std::shared_ptr<Connection<T>> remoteConnection = nullptr;
		Message<T> remoteMsg;

		OwnerMessage(std::shared_ptr<Connection<T>> _con, Message<T> _msg) : remoteConnection{ _con }, remoteMsg{ _msg } {}
	};

	template<typename T>
	using OWN_MSG_PTR = std::shared_ptr<OwnerMessage<T>>;

	
	template<typename MessageType>
	struct MessageFactory
	{
		template<typename Data>
		static Message<MessageType> CreateMessage(MessageType _type, MessageStatus _status, Data _msg)
		{
			Net::Message<MessageType> msg;
			msg.SetType(_type);
			msg.SetStatus(_status);
			msg << _msg;

			return msg;
		}

		static Message<MessageType> CreateMessage(MessageType _type, MessageStatus _status)
		{
			Net::Message<MessageType> msg;
			msg.SetType(_type);
			msg.SetStatus(_status);

			return msg;
		}
	};

}