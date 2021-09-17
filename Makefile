compile:
	g++-9 -Wall -g3 -fpermissive -fsanitize=address main.c -o giffer  `pkg-config --cflags --libs opencv4` `sdl2-config --cflags --libs` -lSDL2_mixer
	g++-9 -Wall -g3 -fpermissive -fsanitize=address init.c -o init  `pkg-config --cflags --libs opencv4` `sdl2-config --cflags --libs` -lSDL2_mixer
