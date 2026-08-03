#include "included.h"
#include "Application.h"

// settings
const unsigned int SCR_WIDTH = 1980;
const unsigned int SCR_HEIGHT = 1080;

int main()
{
	Application PhysicSim(SCR_WIDTH, SCR_HEIGHT);
	PhysicSim.Init();
	PhysicSim.Run();
}

