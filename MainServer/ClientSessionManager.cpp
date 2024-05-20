#include "pch.h"
#include "ClientSessionManager.h"
#include "ClientSession.h"

ClientSessionManager g_sessionManager;

void ClientSessionManager::Add(std::shared_ptr<ClientSession> session)
{
	std::lock_guard<std::mutex> lock(_mutex);
	_sessions.insert(session);
}

void ClientSessionManager::Remove(std::shared_ptr<ClientSession> session)
{
	std::lock_guard<std::mutex> lock(_mutex);
	_sessions.erase(session);
}

void ClientSessionManager::Broadcast(std::shared_ptr<SendBuffer> sendBuffer)
{
	std::lock_guard<std::mutex> lock(_mutex);
	for (auto session : _sessions)
	{
		session->Send(sendBuffer);
	}
}