#include "pch.h"
#include "ClientProtocolManager.h"
#include "Player.h"
#include "Room.h"
#include "ObjectManager.h"

FProtocolManager g_protocolManager[UINT16_MAX];

bool Manage_INVALID(std::shared_ptr<ProtocolSession>& session, BYTE* buffer, int32_t len)
{
	ProtocolHeader* header = reinterpret_cast<ProtocolHeader*>(buffer);
	// TODO : Log
	return false;
}

bool Manage_UC_LOGIN(std::shared_ptr<ProtocolSession>& session, Protocol::UC_LOGIN& proto)
{
	Protocol::MS_LOGIN loginProto;

	for (int32_t i = 0; i < 3; i++)
	{
		Protocol::ObjectInfo* objectInfo = loginProto.add_players();
		Protocol::PosInfo* posInfo = objectInfo->mutable_pos_info();
		posInfo->set_x(Utility::GetRandom(0.f, 100.f));
		posInfo->set_y(Utility::GetRandom(0.f, 100.f));
		posInfo->set_z(Utility::GetRandom(0.f, 100.f));
		posInfo->set_yaw(Utility::GetRandom(0.f, 45.f));
	}

	loginProto.set_success(true);
	SEND_PROTOCOL(loginProto);

	return true;
}

bool Manage_UC_ENTER_GAME(std::shared_ptr<ProtocolSession>& session, Protocol::UC_ENTER_GAME& proto)
{
	// 플레이어 생성
	std::shared_ptr<Player> player = ObjectManager::CreatePlayer(static_pointer_cast<ClientSession>(session));

	// 방에 입장
	g_room->PushTask(&Room::ManageEnterPlayer, player);

	//g_room->ManageEnterPlayer(player);
	

	return true;
}

bool Manage_UC_LEAVE_GAME(ProtocolSessionSPtr& session, Protocol::UC_LEAVE_GAME& proto)
{
	auto clientSession = static_pointer_cast<ClientSession>(session);

	std::shared_ptr<Player> player = clientSession->_player.load();
	if (player == nullptr)
		return false;

	std::shared_ptr<Room> room = player->_room.load().lock();
	if (room == nullptr)
		return false;

	g_room->PushTask(&Room::ManageLeavePlayer,player);

	return true;
}

bool Manage_UC_MOVE(ProtocolSessionSPtr& session, Protocol::UC_MOVE& proto)
{
	auto clientSession = static_pointer_cast<ClientSession>(session);

	std::shared_ptr<Player> player = clientSession->_player.load();
	if (player == nullptr)
		return false;

	std::shared_ptr<Room> room = player->_room.load().lock();
	if (room == nullptr)
		return false;


	g_room->PushTask(&Room::ManageMove,proto);

	return true;
}

bool Manage_UC_CHAT(std::shared_ptr<ProtocolSession>& session, Protocol::UC_CHAT& proto)
{


	return true;
}


