#define _CRT_SECURE_NO_WARNINGS
#include <uWS/uWS.h>
#include <iostream>
#include "HProxyServer.h"
#include <evpp/logging.h>

int main(int argc, char* argv[])
{
	// google::InitGoogleLogging(argv[0]);
    // google::SetStderrLogging(google::NUM_SEVERITIES - 1);
	auto server = new HProxyServer(3000);
}
