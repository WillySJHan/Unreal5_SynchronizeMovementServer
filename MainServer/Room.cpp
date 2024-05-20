#include "pch.h"
#include "Room.h"
#include "Player.h"
#include "ClientSession.h"
#include "Utility.h"

std::shared_ptr<Room> g_room = std::make_shared<Room>();

Room::Room()
{
}

Room::~Room()
{
}

bool Room::Enter(std::shared_ptr<BaseObject> object, bool randPos)
{
	bool success = EnterObject(object);

	// ���� ��ġ
	if (randPos)
	{
		object->_posInfo->set_x(Utility::GetRandom(0.f, 500.f));
		object->_posInfo->set_y(Utility::GetRandom(0.f, 500.f));
		object->_posInfo->set_z(100.f);
		object->_posInfo->set_yaw(Utility::GetRandom(0.f, 100.f));
	}

	// ���� ����� ���� �÷��̾�� �˸���
	if (auto player = dynamic_pointer_cast<Player>(object))
	{
		Protocol::MS_ENTER_GAME enterGamePkt;
		enterGamePkt.set_success(success);

		Protocol::ObjectInfo* playerInfo = new Protocol::ObjectInfo();
		playerInfo->CopyFrom(*object->_objectInfo);
		enterGamePkt.set_allocated_player(playerInfo);

		std::shared_ptr<SendBuffer> sendBuffer = ClientProtocolManager::MakeSendBuffer(enterGamePkt);
		if (auto session = player->_session.lock())
			session->Send(sendBuffer);
	}

	// ���� ����� �ٸ� �÷��̾�� �˸���
	{
		Protocol::MS_SPAWN spawnProto;

		Protocol::ObjectInfo* objectInfo = spawnProto.add_players();
		objectInfo->CopyFrom(*object->_objectInfo);

		std::shared_ptr<SendBuffer> sendBuffer = ClientProtocolManager::MakeSendBuffer(spawnProto);
		Broadcast(sendBuffer, object->_objectInfo->object_id());
	}

	// ���� ������ �÷��̾� ����� ���� �÷��̾����� �������ش�
	if (auto player = dynamic_pointer_cast<Player>(object))
	{
		Protocol::MS_SPAWN spawnProto;

		for (auto& item : _objects)
		{
			if (item.second->IsPlayer() == false)
				continue;

			Protocol::ObjectInfo* playerInfo = spawnProto.add_players();
			playerInfo->CopyFrom(*item.second->_objectInfo);
		}

		std::shared_ptr<SendBuffer> sendBuffer = ClientProtocolManager::MakeSendBuffer(spawnProto);
		if (auto session = player->_session.lock())
			session->Send(sendBuffer);
	}

	return success;
}

bool Room::Leave(std::shared_ptr<BaseObject> object)
{
	if (object == nullptr)
		return false;

	const uint64_t objectId = object->_objectInfo->object_id();
	bool success = LeaveObject(objectId);

	// ���� ����� �����ϴ� �÷��̾�� �˸���
	if (auto player = dynamic_pointer_cast<Player>(object))
	{
		Protocol::MS_LEAVE_GAME leaveGamePkt;

		std::shared_ptr<SendBuffer> sendBuffer = ClientProtocolManager::MakeSendBuffer(leaveGamePkt);
		if (auto session = player->_session.lock())
			session->Send(sendBuffer);
	}

	// ���� ����� �˸���
	{
		Protocol::MS_DESPAWN despawnPkt;
		despawnPkt.add_object_ids(objectId);

		std::shared_ptr<SendBuffer> sendBuffer = ClientProtocolManager::MakeSendBuffer(despawnPkt);
		Broadcast(sendBuffer, objectId);

		if (auto player = dynamic_pointer_cast<Player>(object))
			if (auto session = player->_session.lock())
				session->Send(sendBuffer);
	}

	return success;
}

bool Room::ManageEnterPlayer(std::shared_ptr<Player> player)
{
	return Enter(player, true);
}

bool Room::ManageLeavePlayer(std::shared_ptr<Player> player)
{
	return Leave(player);
}

void Room::ManageMove(Protocol::UC_MOVE proto)
{
	const uint64_t objectId = proto.info().object_id();
	if (_objects.find(objectId) == _objects.end())
		return;

	// ����
	// validation
	std::shared_ptr<Player> player = dynamic_pointer_cast<Player>(_objects[objectId]);
	player->_posInfo->CopyFrom(proto.info());

	// �̵� 
	{
		Protocol::MS_MOVE moveProto;
		{
			Protocol::PosInfo* info = moveProto.mutable_info();
			info->CopyFrom(proto.info());
		}

		std::shared_ptr<SendBuffer> sendBuffer = ClientProtocolManager::MakeSendBuffer(moveProto);
		Broadcast(sendBuffer);
	}
}

void Room::UpdateTick()
{
	std::cout << "Update Room" << std::endl;

	ReservePush(100, &Room::UpdateTick);
}

bool Room::EnterObject(std::shared_ptr<BaseObject> object)
{
	// �ִٸ� ������ �ִ�
	if (_objects.find(object->_objectInfo->object_id()) != _objects.end())
		return false;

	_objects.insert(make_pair(object->_objectInfo->object_id(), object));

	object->_room.store(static_pointer_cast<Room>(shared_from_this()));

	return true;
}

bool Room::LeaveObject(uint64_t objectId)
{
	// ���ٸ� ������ �ִ�.
	if (_objects.find(objectId) == _objects.end())
		return false;

	std::shared_ptr<BaseObject> object = _objects[objectId];
	std::shared_ptr<Player> player = dynamic_pointer_cast<Player>(object);
	if (player)
		player->_room.store(std::weak_ptr<Room>());

	_objects.erase(objectId);

	return true;
}

void Room::Broadcast(std::shared_ptr<SendBuffer> sendBuffer, uint64_t exceptId)
{
	// ���� ���Ϳ� �̾Ƽ�
	for (auto& item : _objects)
	{
		std::shared_ptr<Player> player = dynamic_pointer_cast<Player>(item.second);
		if (player == nullptr)
			continue;
		if (player->_objectInfo->object_id() == exceptId)
			continue;

		if (std::shared_ptr<ClientSession> session = player->_session.lock())
			session->Send(sendBuffer);
	}
}
