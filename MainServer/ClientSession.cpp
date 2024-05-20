#include "pch.h"
#include "ClientSession.h"
#include "ClientProtocolManager.h"
#include "ClientSessionManager.h"
#include "Room.h"

void ClientSession::OnConnected()
{
	g_sessionManager.Add(std::static_pointer_cast<ClientSession>(shared_from_this()));
}

void ClientSession::OnDisconnected()
{
	g_sessionManager.Remove(std::static_pointer_cast<ClientSession>(shared_from_this()));
}

void ClientSession::OnRecvProtocol(BYTE* buffer, int32_t len)
{
	std::shared_ptr<ProtocolSession> session = GetProtocolSessionSptr();
	ProtocolHeader* header = reinterpret_cast<ProtocolHeader*>(buffer);

	ClientProtocolManager::ManageProtocol(session, buffer, len);

}

void ClientSession::OnSend(int32_t len)
{

}
