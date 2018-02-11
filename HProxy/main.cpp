#define _CRT_SECURE_NO_WARNINGS
#include "HProxyServer.h"
#include <evpp/logging.h>

int main(int argc, char* argv[])
{
	google::InitGoogleLogging(argv[0]);
    google::LogToStderr();
	// google::SetStderrLogging(google::NUM_SEVERITIES - 1);
	auto server = new helix_proxy_server(3000);
}
