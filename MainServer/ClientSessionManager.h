#pragma once

class ClientSessionManager
{
public:
	void Add(std::shared_ptr<class ClientSession> session);
	void Remove(std::shared_ptr<class ClientSession> session);
	void Broadcast(std::shared_ptr<SendBuffer> sendBuffer);

private:
	std::mutex _mutex;
	std::set<std::shared_ptr<class ClientSession>> _sessions;
};

extern ClientSessionManager g_sessionManager;

