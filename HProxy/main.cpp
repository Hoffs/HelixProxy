#define _CRT_SECURE_NO_WARNINGS
#include "helix_proxy_server.h"
#include <evpp/logging.h>
#include <cstdlib>

int main(int argc, char* argv[])
{
	google::InitGoogleLogging(argv[0]);
    google::LogToStderr();
	// google::SetStderrLogging(google::NUM_SEVERITIES - 1);
	if (argc < 4) {
		LOG(FATAL) << "Not enough arguments passed";
		return 0;
	}

	const auto server = new helix_proxy_server(std::stoi(argv[1]), argv[2], std::stoi(argv[3]));
	delete server;
}
