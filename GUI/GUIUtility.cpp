

#include "GUIUtility.h"
#include "GUIAudio.h"


void initSDL(Uint32 flags){
    if ( SDL_Init(flags) < 0 ) {
        throw Error("Unable to init SDL: \n" + std::string(SDL_GetError()));
    }

	if(flags & SDL_INIT_AUDIO){
		initAudio(flags);
	}

	TTF_Init();
	SDL_EnableUNICODE( SDL_ENABLE );

    atexit(SDL_Quit);
}
