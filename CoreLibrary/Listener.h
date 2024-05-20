#pragma once
#include "IOCPBase.h"
#include "SocketAddress.h"

class Listener : public IOCPObject
{
public:
	Listener() = default;
	~Listener();

public:
	bool StartAccept(std::shared_ptr<class ServerService> service);
	void CloseSocket();

public:
	virtual HANDLE GetHandle() override;
	virtual void Dispatch(class IOCPEvent* iocpEvent, int32_t Bytes = 0) override;

private:
	void RegisterAccept(class AcceptEvent* acceptEvent);
	void ProcessAccept(class AcceptEvent* acceptEvent);

protected:
	SOCKET _socket = INVALID_SOCKET;
	std::vector<class AcceptEvent*> _acceptEvents;
	std::shared_ptr<class ServerService> _service;
};