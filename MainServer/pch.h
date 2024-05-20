#pragma once

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.

#ifdef _DEBUG
#pragma comment(lib, "CoreLibrary\\Debug\\CoreLibrary.lib")
#pragma comment(lib, "Protobuf\\Debug\\libprotobufd.lib")
#else
#pragma comment(lib, "CoreLibrary\\Release\\CoreLibrary.lib")
#pragma comment(lib, "Protobuf\\Release\\libprotobuf.lib")
#endif

#include "CoreLibraryPch.h"
#include "Enum.pb.h"
#include "Protocol.pb.h"
#include "Struct.pb.h"
#include "ClientProtocolManager.h"
#include "Utility.h"
#include "ClientSession.h"

#define SEND_PROTOCOL(proto)													\
	std::shared_ptr<SendBuffer> sendBuffer = ClientProtocolManager::MakeSendBuffer(proto);	\
	session->Send(sendBuffer);