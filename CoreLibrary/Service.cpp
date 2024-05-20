#include "pch.h"
#include "Service.h"
#include "Session.h"
#include "Listener.h"

Service::Service(ServiceType type, SocketAddress address, std::shared_ptr<IOCPBase> base, FSessionMaker f_maker, int32_t maxSessionCount)
	: _type(type), _socketAddress(address), _iocpBase(base), _f_sessionMaker(f_maker), _maxSessionCount(maxSessionCount)
{

}

Service::~Service()
{
}

void Service::CloseService()
{

}


std::shared_ptr<Session> Service::CreateSession()
{
	std::shared_ptr<Session> session = _f_sessionMaker();
	session->SetService(shared_from_this());

	if (_iocpBase->Register(session) == false)
		return nullptr;

	return session;
}

void Service::AddSession(std::shared_ptr<Session> session)
{
	std::lock_guard<std::mutex> lock(_mutex);
	_sessionCount++;
	_sessions.insert(session);
}

void Service::ReleaseSession(std::shared_ptr<Session> session)
{
	std::lock_guard<std::mutex> lock(_mutex);
	SJ_ASSERT(_sessions.erase(session) != 0);
	_sessionCount--;
}

ClientService::ClientService(SocketAddress targetAddress, std::shared_ptr<IOCPBase> base, FSessionMaker f_maker, int32_t maxSessionCount)
	: Service(ServiceType::Client, targetAddress, base, f_maker, maxSessionCount)
{
}

bool ClientService::Start()
{
	if (CanStart() == false)
		return false;

	const int32_t sessionCount = GetMaxSessionCount();
	for (int32_t i = 0; i < sessionCount; i++)
	{
		std::shared_ptr<Session> session = CreateSession();
		if (session->Connect() == false)
			return false;
	}

	return true;
}

ServerService::ServerService(SocketAddress serverAddress, std::shared_ptr<IOCPBase> base, FSessionMaker f_maker, __int32 maxSessionCount)
	: Service(ServiceType::Server, serverAddress, base, f_maker, maxSessionCount)
{
}

bool ServerService::Start()
{
	if (CanStart() == false)
		return false;

	_listener = std::make_shared<Listener>();
	if (_listener == nullptr)
		return false;

	std::shared_ptr<ServerService> service = std::static_pointer_cast<ServerService>(shared_from_this());
	if (_listener->StartAccept(service) == false)
		return false;

	return true;
}

void ServerService::CloseService()
{
	Service::CloseService();
}
