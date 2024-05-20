#pragma once

class Room;

class BaseObject : public std::enable_shared_from_this<BaseObject>
{
public:
	BaseObject();
	virtual ~BaseObject();

	bool IsPlayer() { return _isPlayer; }

public:
	Protocol::ObjectInfo* _objectInfo;
	Protocol::PosInfo* _posInfo;

public:
	std::atomic<std::weak_ptr<Room>> _room;

protected:
	bool _isPlayer = false;
};