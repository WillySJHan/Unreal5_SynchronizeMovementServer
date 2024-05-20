#pragma once
#include "Creature.h"

class ClientSession;

class Player : public Creature
{
public:
	Player();
	virtual ~Player();


public:
	std::weak_ptr<ClientSession> _session;
};
