#include "pch.h"
#include "BaseObject.h"

BaseObject::BaseObject()
{
	_objectInfo = new Protocol::ObjectInfo();
	_posInfo = new Protocol::PosInfo();
	_objectInfo->set_allocated_pos_info(_posInfo);
}

BaseObject::~BaseObject()
{
	delete _objectInfo;
}