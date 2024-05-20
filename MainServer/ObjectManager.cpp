#include "pch.h"
#include "ObjectManager.h"
#include "Player.h"
#include "ClientSession.h"

std::atomic<int64_t> ObjectManager::_s_idGenerator = 1;

std::shared_ptr<Player> ObjectManager::CreatePlayer(std::shared_ptr<ClientSession> session)
{
	// ID »ý¼º±â
	//const int64_t newId = _s_idGenerator.fetch_add(1);
	const int64_t newId = GenerateId(Protocol::ObjectType::PLAYER);

	std::shared_ptr<Player> player = std::make_shared<Player>();
	player->_objectInfo->set_object_id(newId);
	player->_posInfo->set_object_id(newId);

	player->_session = session;
	session->_player.store(player);

	return player;

}

Protocol::ObjectType ObjectManager::GetObjectTypeById(int64_t objectId)
{
	int64_t type = (objectId >> 56) & 0x7F;
	return static_cast<Protocol::ObjectType>(type);
	
}

int64_t ObjectManager::GenerateId(Protocol::ObjectType type)
{
	const int64_t newId = _s_idGenerator.fetch_add(1);
	return ((static_cast<int64_t>(type) << 56) | (newId));
}
