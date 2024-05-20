#pragma once

class Player;
class ClientSession;

class ObjectManager
{
public:
	static std::shared_ptr<Player> CreatePlayer(std::shared_ptr<ClientSession> session);
	static Protocol::ObjectType GetObjectTypeById(int64_t objectId);
private:
	static int64_t GenerateId(Protocol::ObjectType type);

private:
	static std::atomic<int64_t> _s_idGenerator;
};

