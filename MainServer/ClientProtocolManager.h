#pragma once
#include "Protocol.pb.h"
#include "P1.h"

using FProtocolManager = std::function<bool(ProtocolSessionSPtr&, BYTE*, int32_t)>;
extern FProtocolManager g_protocolManager[UINT16_MAX];

enum : uint16_t
{
	UC_LOGIN = 1000,
	MS_LOGIN = 1001,
	UC_ENTER_GAME = 1002,
	MS_ENTER_GAME = 1003,
	UC_LEAVE_GAME = 1004,
	MS_LEAVE_GAME = 1005,
	MS_SPAWN = 1006,
	MS_DESPAWN = 1007,
	UC_MOVE = 1008,
	MS_MOVE = 1009,
	UC_CHAT = 1010,
	MS_CHAT = 1011,
};

bool Manage_INVALID(ProtocolSessionSPtr& session, BYTE* buffer, int32_t len);
bool Manage_UC_LOGIN(ProtocolSessionSPtr& session, Protocol::UC_LOGIN& proto);
bool Manage_UC_ENTER_GAME(ProtocolSessionSPtr& session, Protocol::UC_ENTER_GAME& proto);
bool Manage_UC_LEAVE_GAME(ProtocolSessionSPtr& session, Protocol::UC_LEAVE_GAME& proto);
bool Manage_UC_MOVE(ProtocolSessionSPtr& session, Protocol::UC_MOVE& proto);
bool Manage_UC_CHAT(ProtocolSessionSPtr& session, Protocol::UC_CHAT& proto);

class ClientProtocolManager
{
public:
	static void Init()
	{
		for (int32_t i = 0; i < UINT16_MAX; i++)
			g_protocolManager[i] = Manage_INVALID;
		g_protocolManager[UC_LOGIN] = [](ProtocolSessionSPtr& session, BYTE* buffer, uint32_t len) { return ManageProtocol<Protocol::UC_LOGIN>(Manage_UC_LOGIN, session, buffer, len); };
		g_protocolManager[UC_ENTER_GAME] = [](ProtocolSessionSPtr& session, BYTE* buffer, uint32_t len) { return ManageProtocol<Protocol::UC_ENTER_GAME>(Manage_UC_ENTER_GAME, session, buffer, len); };
		g_protocolManager[UC_LEAVE_GAME] = [](ProtocolSessionSPtr& session, BYTE* buffer, uint32_t len) { return ManageProtocol<Protocol::UC_LEAVE_GAME>(Manage_UC_LEAVE_GAME, session, buffer, len); };
		g_protocolManager[UC_MOVE] = [](ProtocolSessionSPtr& session, BYTE* buffer, uint32_t len) { return ManageProtocol<Protocol::UC_MOVE>(Manage_UC_MOVE, session, buffer, len); };
		g_protocolManager[UC_CHAT] = [](ProtocolSessionSPtr& session, BYTE* buffer, uint32_t len) { return ManageProtocol<Protocol::UC_CHAT>(Manage_UC_CHAT, session, buffer, len); };
	}

	static bool ManageProtocol(ProtocolSessionSPtr& session, BYTE* buffer, int32_t len)
	{
		ProtocolHeader* header = reinterpret_cast<ProtocolHeader*>(buffer);
		return g_protocolManager[header->id](session, buffer, len);
	}
	static SendBufferSPtr MakeSendBuffer(Protocol::MS_LOGIN& proto) { return MakeSendBuffer(proto, MS_LOGIN); }
	static SendBufferSPtr MakeSendBuffer(Protocol::MS_ENTER_GAME& proto) { return MakeSendBuffer(proto, MS_ENTER_GAME); }
	static SendBufferSPtr MakeSendBuffer(Protocol::MS_LEAVE_GAME& proto) { return MakeSendBuffer(proto, MS_LEAVE_GAME); }
	static SendBufferSPtr MakeSendBuffer(Protocol::MS_SPAWN& proto) { return MakeSendBuffer(proto, MS_SPAWN); }
	static SendBufferSPtr MakeSendBuffer(Protocol::MS_DESPAWN& proto) { return MakeSendBuffer(proto, MS_DESPAWN); }
	static SendBufferSPtr MakeSendBuffer(Protocol::MS_MOVE& proto) { return MakeSendBuffer(proto, MS_MOVE); }
	static SendBufferSPtr MakeSendBuffer(Protocol::MS_CHAT& proto) { return MakeSendBuffer(proto, MS_CHAT); }

private:
	template<typename ProtocolType, typename FManage>
	static bool ManageProtocol(FManage f_manage, ProtocolSessionSPtr& session, BYTE* buffer, int32_t len)
	{
		ProtocolType proto;
		if (proto.ParseFromArray(buffer + sizeof(ProtocolHeader), len - sizeof(ProtocolHeader)) == false)
			return false;

		return f_manage(session, proto);
	}

	template<typename T>
	static SendBufferSPtr MakeSendBuffer(T& proto, uint16_t protoId)
	{
		const uint16_t dataSize = static_cast<uint16_t>(proto.ByteSizeLong());
		const uint16_t protoSize = dataSize + sizeof(ProtocolHeader);

#if UE_BUILD_DEBUG + UE_BUILD_DEVELOPMENT + UE_BUILD_TEST + UE_BUILD_SHIPPING >= 1
		SendBufferSPtr sendBuffer = MakeShared<SendBuffer>(protoSize);
#else
		std::shared_ptr<SendBuffer> sendBuffer = std::make_shared<SendBuffer>(protoSize);
#endif

		ProtocolHeader* header = reinterpret_cast<ProtocolHeader*>(sendBuffer->Buffer());
		header->size = protoSize;
		header->id = protoId;
		proto.SerializeToArray(&header[1], dataSize);
		sendBuffer->SetWriteSize(protoSize);

		return sendBuffer;
	}
};
