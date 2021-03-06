#include"stdafx.h"
#include"Game.h"

int main(int argc, char** argv)
{
	Unbound::Game::init();
	Unbound::Game::running = true;

	while (Unbound::Game::running)
	{
		Unbound::Game::update();

		if (glfwWindowShouldClose(Unbound::Graphics::Renderer::get()->getTargetWindow().window))
		{
			Unbound::Game::running = false;
		}
	}

	return Unbound::Game::close();
}