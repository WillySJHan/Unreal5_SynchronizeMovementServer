#include "pch.h"

#include "ThreadManager.h"
#include "SocketManager.h"
#include "Service.h"
#include "ClientSession.h"
#include "ClientProtocolManager.h"
#include "ClientSessionManager.h"
#include "Protocol.pb.h"
#include "Room.h"


constexpr uint64_t WORK_TIME = 256;

void RunWork(std::shared_ptr<ServerService>& service)
{
	while (true)
	{
		t_endTime = ::GetTickCount64() + WORK_TIME;
		service->GetIocpBase()->Dispatch(10);
		ThreadManager::PushReservedTask();
		ThreadManager::ExecuteTask();
	}
}

int main()
{
	ClientProtocolManager::Init();

	std::shared_ptr<ServerService> service = std::make_shared<ServerService>(
		SocketAddress(L"127.0.0.1", 9797),
		std::make_shared<IOCPBase>(),
		std::make_shared<ClientSession>, 
		100);

	SJ_ASSERT(service->Start());

	for (int32_t i = 0; i < 5; i++)
	{
		g_threadManager->Run([&service]()
			{
				while (true)
				{
					RunWork(service);
				}
			});
	}

	g_threadManager->Join();
}




