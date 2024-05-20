#pragma once
#include "Session.h"

class Player;

class ClientSession : public ProtocolSession
{
public:
	~ClientSession()
	{
		std::cout << "~ClientSession" << std::endl;
	}

	virtual void OnConnected() override;
	virtual void OnDisconnected() override;
	virtual void OnRecvProtocol(BYTE* buffer, int32_t len) override;
	virtual void OnSend(int32_t len) override;

public:
	std::atomic<std::shared_ptr<Player>> _player;
};
