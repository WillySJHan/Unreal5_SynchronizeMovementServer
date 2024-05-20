#pragma once

class Player;
class BaseObject;

class Room : public TaskQueue
{
public:
	Room();
	virtual ~Room();

public:
	bool Enter(std::shared_ptr<BaseObject> object, bool randPos = true);
	bool Leave(std::shared_ptr<BaseObject> object);

	bool ManageEnterPlayer(std::shared_ptr<Player> player);
	bool ManageLeavePlayer(std::shared_ptr<Player> player);

	void ManageMove(Protocol::UC_MOVE proto);

public:
	void UpdateTick();

private:
	bool EnterObject(std::shared_ptr<BaseObject> object);
	bool LeaveObject(uint64_t objectId);


private:
	void Broadcast(std::shared_ptr<SendBuffer> sendBuffer, uint64_t exceptId = 0);


private:
	std::unordered_map<uint64_t, std::shared_ptr<BaseObject>> _objects;
};

extern std::shared_ptr<Room> g_room;
