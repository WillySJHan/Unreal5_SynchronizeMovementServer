#pragma once

#define USING_SHARED_PTR(name)	using name##SPtr = std::shared_ptr<class name>;

USING_SHARED_PTR(Session);
USING_SHARED_PTR(ProtocolSession);
USING_SHARED_PTR(SendBuffer);