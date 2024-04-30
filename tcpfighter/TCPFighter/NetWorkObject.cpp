#include "stdafx.h"
#include "NetWorkObject.h"

Session::Session()
{
	sendFlag = false;
	connected = false;
	headerSaveFlag = false;
	totalRecv = 0;
	totalSend = 0;
	resvStream = new MinLib::StreamQueue(2000);
	sendStream = new MinLib::StreamQueue(2000);
}
