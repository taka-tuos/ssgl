#include <SDL.h>

Uint32 keysym;

int poll_event(SDL_Event *sdl_event)
{
	Uint8 *key_state = SDL_GetKeyState(NULL);

	if(SDL_PollEvent(sdl_event)) {
		switch (sdl_event->type) {
		case SDL_KEYDOWN:
			keysym = sdl_event->key.keysym.sym;
			break;
		case SDL_KEYUP:
			keysym = 0;
			break;
		case SDL_QUIT:
			return 1;
		}
	}

	return 0;
}