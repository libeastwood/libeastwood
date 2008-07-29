#include <iostream>
#include <string.h>
#include <SDL.h>
#include "Pakfile.h"
#include "Shpfile.h"
#include "Cpsfile.h"
#include "Wsafile.h"
#include "Icnfile.h"

SDL_Surface* screen;
SDL_Palette* palette;
bool quiting = false;
int ShpIndex = 0;
int WsaIndex = 0;
int IcnIndex = 0;
int PakIndex = 0;

SDL_Surface* picture = NULL;

Pakfile* myPakfile = NULL;
Shpfile* myShpfile = NULL;
Cpsfile* myCpsfile = NULL;
Wsafile* myWsafile = NULL;
Icnfile* myIcnfile = NULL;
unsigned char * filedata = NULL;

void showShp(int index)
{
	if(myShpfile == NULL) {
		return;
	}

	if(picture != NULL) {
		SDL_FreeSurface(picture);
		picture = NULL;
	}
	

	
	picture = myShpfile->getPicture(index);
	
	if(picture == NULL) {
		fprintf(stderr,"Cannot load picture %d\n",index);
	}
}

void showWsa(int index)
{
	if(myWsafile == NULL) {
		return;
	}

	if(picture != NULL) {
		SDL_FreeSurface(picture);
		picture = NULL;
	}
	

	
	picture = myWsafile->getPicture(index);
	
	if(picture == NULL) {
		fprintf(stderr,"Cannot load picture %d\n",index);
	}
}

void showIcn(int index)
{
	if(myIcnfile == NULL) {
		return;
	}

	if(picture != NULL) {
		SDL_FreeSurface(picture);
		picture = NULL;
	}
	
	picture = myIcnfile->getPicture(index);
	
	if(picture == NULL) {
		fprintf(stderr,"Cannot load picture %d\n",index);
	}
}

void showOtherFile(char *filename) {
	if(myShpfile != NULL) {
		delete myShpfile;
	}
	
	if(myCpsfile != NULL) {
		delete myCpsfile;
	}
	
	if(myWsafile != NULL) {
		delete myWsafile;
	}
	
	if(myIcnfile != NULL) {
		delete myIcnfile;
	}
	
	if(filedata != NULL) {
		free(filedata);
	}
	
	int bufsize;
	
	if((filedata = myPakfile->getFile(filename,&bufsize)) == NULL) {
		fprintf(stderr,"Error: Cannot open %s\n",filename);
		exit(EXIT_FAILURE);		
	}
	
	if(strstr(filename,".SHP") != 0) {
		myCpsfile = NULL;
		myWsafile = NULL;
		myIcnfile = NULL;
		if( (myShpfile = new Shpfile(filedata,bufsize)) == NULL) {
			fprintf(stderr,"Error: Cannot open Shp-File %s\n",filename);
			exit(EXIT_FAILURE);			
		}		
	} else if (strstr(filename,".CPS") != 0) {
		myShpfile = NULL;
		myWsafile = NULL;
		myIcnfile = NULL;
		if( (myCpsfile = new Cpsfile(filedata,bufsize)) == NULL) {
			fprintf(stderr,"Error: Cannot open Cps-File %s\n",filename);
			exit(EXIT_FAILURE);			
		}
	} else if (strstr(filename,".WSA") != 0) {
		myShpfile = NULL;
		myCpsfile = NULL;
		myIcnfile = NULL;
		if( (myWsafile = new Wsafile(filedata,bufsize)) == NULL) {
			fprintf(stderr,"Error: Cannot open Wsa-File %s\n",filename);
			exit(EXIT_FAILURE);			
		}
	} else if (strstr(filename,".ICN") != 0) {
		myShpfile = NULL;
		myCpsfile = NULL;
		myWsafile = NULL;
		if( (myIcnfile = new Icnfile(filedata,bufsize)) == NULL) {
			fprintf(stderr,"Error: Cannot open Icn-File %s\n",filename);
			exit(EXIT_FAILURE);			
		}
	} else {
		return;
	}
	

	
	printf("\nOpening File %s\n",filename);
	fflush(stdout);
	
	if(myShpfile != NULL) {
		ShpIndex = 0;
		showShp(ShpIndex);
	} else if (myCpsfile != NULL) {
		if(picture != NULL) {
			SDL_FreeSurface(picture);
			picture = NULL;
		}
		picture = myCpsfile->getPicture();
	} else if (myWsafile != NULL) {
		WsaIndex = 0;
		showWsa(WsaIndex);
	} else if (myIcnfile != NULL) {
		IcnIndex = 0;
		showIcn(IcnIndex);
	}
}

char * getNextFileInPak() {
	int oldPakIndex = PakIndex;
	char * Filename;
	
	PakIndex++;	
	if(PakIndex >= myPakfile->getNumFiles()) {
			PakIndex = 0;
	}	
	
	do {
		if( (Filename = myPakfile->getFilename(PakIndex)) == NULL) {
			fprintf(stderr,"ERROR: Pakfile::getFilename() returned NULL.\n");
			exit(EXIT_FAILURE);
		}
		
		if(strstr(Filename,".SHP") != 0) {
			return Filename;
		} else if (strstr(Filename,".CPS") != 0) {
			return Filename;
		} else if (strstr(Filename,".WSA") != 0) {
			return Filename;
		} else if (strstr(Filename,".ICN") != 0) {
			return Filename;
		}else {
			printf("Skipping %s\n",Filename);
		}
		
		
		PakIndex++;		
		if(PakIndex >= myPakfile->getNumFiles()) {
			PakIndex = 0;
		}
	} while(oldPakIndex != PakIndex);
	
	return NULL;
}

char * getPrevFileInPak() {
	int oldPakIndex = PakIndex;
	PakIndex--;
	char * Filename;
	
	if(PakIndex < 0) {
		PakIndex = myPakfile->getNumFiles() - 1;
	}
		
	do {

		if( (Filename = myPakfile->getFilename(PakIndex)) == NULL) {
			fprintf(stderr,"ERROR: Pakfile::getFilename() returned NULL.\n");
			exit(EXIT_FAILURE);
		}

		if(strstr(Filename,".SHP") != 0) {
			return Filename;
		} else if (strstr(Filename,".CPS") != 0) {
			return Filename;
		} else if (strstr(Filename,".WSA") != 0) {
			return Filename;
		} else if (strstr(Filename,".ICN") != 0) {
			return Filename;
		}else {
			printf("Skipping %s\n",Filename);
		}
		
		PakIndex--;		
		if(PakIndex < 0) {
			PakIndex = myPakfile->getNumFiles() - 1;
		}
	} while(oldPakIndex != PakIndex);
	
	return NULL;
}

int main(int argc, char ** argv)
{
	bool fullscreen = false;
	
	
	switch(argc)
	{
		case 2:
		{
		} break;
		
		case 3:
		{
			if ((argc == 3) & (strcmp(argv[1],"--fullscreen") == 0)) {
				fullscreen = true;
			} else {
				printf("Usage:\n  Pakfile [--fullscreen] filename.pak\n");
				exit(EXIT_SUCCESS);
			}					
		} break;
		
		default:
		{
			if((argc > 3) || (argc < 2)) {
				printf("Usage:\n  Pakfile [--fullscreen] filename.pak\n");
				exit(EXIT_SUCCESS);
			}			
		} break;
	}
	

	
	
	

	SDL_Event	event;
	
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE) < 0)
	{
		fprintf(stderr, "ERROR: Couldn't initialise SDL: %s\n", SDL_GetError());
		exit(1);
	}
	SDL_EnableUNICODE(1);

	SDL_WM_SetCaption("Pakfileviewer", "Pakfileviewer");
	
	
	// Load palette
	SDL_Surface* paletteSurf = SDL_LoadBMP("Palette.bmp");
	if(paletteSurf == NULL) {
		fprintf(stderr, "ERROR: Couldn't load Palette.bmp: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);		
	}
	
	if((palette = new SDL_Palette) == NULL) {
		fprintf(stderr, "ERROR: Couldn't allocate new SDL_Palette.\n");
		exit(EXIT_FAILURE);			
	}
	
	palette->ncolors = paletteSurf->format->palette->ncolors;
	if((palette->colors = new SDL_Color[palette->ncolors]) == NULL) {
		fprintf(stderr, "ERROR: Couldn't allocate new SDL_Color.\n");
		exit(EXIT_FAILURE);			
	}
	memcpy(palette->colors, paletteSurf->format->palette->colors, sizeof(SDL_Color) * palette->ncolors);
	SDL_FreeSurface(paletteSurf);

	// init Screen
	if( (screen = SDL_SetVideoMode(640, 480, 8, (fullscreen ? SDL_FULLSCREEN : 0) )) == NULL) {
		fprintf(stderr, "ERROR: Couldn't set video mode: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}
	SDL_SetColors(screen, palette->colors, 0, palette->ncolors);
	
	
	// open pakfile
	if( (myPakfile = new Pakfile( ((argc == 2) ? argv[1] : argv[2]))) == NULL) {

	}
	
	
	char *FName;
	
	if((FName = getNextFileInPak()) != NULL) {
		showOtherFile(FName);
	} else {
		fprintf(stderr,"Error: Pakfile contains no images!\n");
		exit(EXIT_FAILURE);		
	}
		
	
	// main Loop
	while(!quiting) {
		
		
		SDL_FillRect(screen,NULL,0);
		
		if(picture != NULL) {
			SDL_Rect dest;
			dest.x = 0;
			dest.y = 0;
			dest.w = picture->w;
			dest.h = picture->h;
			
			SDL_BlitSurface(picture,&dest,screen,&dest);	
		}
		
		SDL_Flip(screen);
		
		// do input
		while(SDL_PollEvent(&event)) {
			switch (event.type)
			{
				case (SDL_KEYUP):
				{
					switch(event.key.keysym.sym)
					{
						case SDLK_ESCAPE:
						{
							quiting = true;
						} break;
						
						case SDLK_DOWN:
						{
							if(myShpfile != NULL) {
								ShpIndex++;
								if(myShpfile) {
									if(ShpIndex == myShpfile->getNumFiles()) {
										ShpIndex = 0;
									}
								} else {
									ShpIndex = 0;
								}
	
								showShp(ShpIndex);
							} else if(myWsafile != NULL) {
								WsaIndex++;
								if(myWsafile) {
									if(WsaIndex == myWsafile->getNumFrames()) {
										WsaIndex = 0;
									}
								} else {
									WsaIndex = 0;
								}
								showWsa(WsaIndex);								
							} else if(myIcnfile != NULL) {
								IcnIndex++;
								if(myIcnfile) {
									if(IcnIndex == myIcnfile->getNumFiles()) {
										IcnIndex = 0;
									}
								} else {
									IcnIndex = 0;
								}
								showIcn(IcnIndex);								
							}
						} break;
						
						case SDLK_UP:
						{
							if(myShpfile != NULL) {
								ShpIndex--;
								if(ShpIndex < 0) {
									if(myShpfile) {
										ShpIndex = myShpfile->getNumFiles() - 1;
									} else {
										ShpIndex = 0;
									}
								}
								showShp(ShpIndex);
							} else if(myWsafile != NULL) {
								WsaIndex--;
								if(WsaIndex < 0) {
									if(myWsafile) {
										WsaIndex = myWsafile->getNumFrames() - 1;
									} else {
										WsaIndex = 0;
									}
								}
								showWsa(WsaIndex);
							}
							else if (myIcnfile != NULL) {
								IcnIndex--;
								if(IcnIndex < 0) {
									if(myIcnfile) {
										IcnIndex = myIcnfile->getNumFiles() - 1;
									} else {
										IcnIndex = 0;
									}
								}
								showIcn(IcnIndex);
							}
						} break;
						
						case SDLK_LEFT:
						{
							char * Filename = getPrevFileInPak();
							if(Filename != NULL) {
								showOtherFile(Filename);
							}
						} break;
						
						case SDLK_RIGHT:
						{
							char * Filename = getNextFileInPak();
							if(Filename != NULL) {
								showOtherFile(Filename);
							}
						} break;

						default:
						{
							;
						} break;
					}
				} break;
				
				default:
				{
					;
				} break;
			}
		}
	}
	
	delete myPakfile;
	delete myShpfile;
	free(filedata);
	
	SDL_Quit();
	
	return 0;
}
