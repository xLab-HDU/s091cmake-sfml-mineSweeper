#include "Game.h"

int main()
{
	Game SweeperMine;
	while (SweeperMine.window.isOpen())
	{
		SweeperMine.Run();
	}
}