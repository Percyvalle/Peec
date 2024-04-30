#include "PeecTrackerServer.hpp"	

int main(int argv, char** argc) {

	TrackerServer a(20228, Utils::GetCountCPU());
	a.Start();

	while (1)
	{
		a.Update(false);
		a.CheckClientConnection();
	}

	return EXIT_FAILURE;
}