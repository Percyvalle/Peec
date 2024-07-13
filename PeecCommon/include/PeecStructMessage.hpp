#pragma once

#include <boost/format.hpp>
#include <boost/date_time.hpp>

#include "xProject_pch.hpp"

#include <network/MessageInterface.hpp>

#include "PeecMessageTypes.hpp"
#include "PeecMessageStatus.hpp"

namespace Net {

	namespace Detail {
		template<typename T, typename = void>
		struct has_data_method : std::false_type {};

		template<typename T>
		struct has_data_method<T, std::void_t<decltype(std::declval<T>().data())>> : std::true_type {};

		template<typename T, typename = void>
		struct has_size_method : std::false_type {};

		template<typename T>
		struct has_size_method<T, std::void_t<decltype(std::declval<T>().size())>> : std::true_type {};

		template<typename T>
		constexpr bool has_data_and_size_v = has_data_method<T>::value && has_size_method<T>::value;
	}

	struct PeecHeaderMessage : public Net::IHeader<MessageTypes, MessageStatus> {
	public:

	};

	struct PeecBodyMessage : public Net::IBody {};

	struct PeecMessage : public Net::IMessage<PeecHeaderMessage, PeecBodyMessage> {

		MessageTypes GetType() const { return header.Type(); }
		MessageStatus GetStatus() const { return header.Status(); }

		void SetType(MessageTypes _type) { header.SetType(_type); }
		void SetStatus(MessageStatus _status) { header.SetStatus(_status); }

		void Clear()
		{
			body.Clear();;
			header.SetSize(NULL);
		}

		bool IsJsonValidate()
		{
			return JSON::accept(ConvertToString());
		}															

		std::string ConvertToString() 
		{
			if (!body.Empty()) {
				std::string ret;
				ret.resize(body.Size());
				memcpy(ret.data(), body.Data().data(), body.Size());

				return ret;
			}

			return "";
		}

		JSON ConvertToJson()
		{
			return IsJsonValidate() ? JSON::parse(ConvertToString()) : JSON();
		}

		template<typename T, std::enable_if_t<Detail::has_data_and_size_v<T>>>
		PeecMessage& operator<<(const T& _message)
		{
			static_assert(std::is_standard_layout<T>::value, "Data is too complex to be pushed into vector");

			size_t startPtr = body.Size();

			body.Data().resize(startPtr + sizeof(T));

			std:memcpy(body.Data().data() + startPtr, &_message, sizeof(T));

			header.SetSize(body.Size());

			return *this;
		}

		template<typename T>
		PeecMessage& operator<<(const T& _message)
		{
			size_t startPtr = body.Size();

			body.Data().resize(startPtr + _message.size());

			std:memcpy(body.Data().data() + startPtr, _message.data(), _message.size());

			header.SetSize(body.Size());

			return *this;
		}
	};
	
	struct MessageFactory
	{
		template<typename Data>
		static Net::PeecMessage CreateMessage(MessageTypes _type, MessageStatus _status, Data _msg)
		{
			Net::PeecMessage msg;
			msg.SetType(_type);
			msg.SetStatus(_status);
			msg << _msg;

			return msg;
		}

		static Net::PeecMessage CreateMessage(MessageTypes _type, MessageStatus _status)
		{
			Net::PeecMessage msg;
			msg.SetType(_type);
			msg.SetStatus(_status);

			return msg;
		}
	};

}