#pragma once

#include <WinSock2.h>
#include <stdio.h>

namespace my {
	const bool createwsa(WSADATA& wsa);

	bool createsock(SOCKET& sock);
}