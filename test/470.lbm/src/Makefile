../lbm: lbm.c main.c
	clang main.c lbm.c -lm -o ../lbm

../lbm-dswp: main-dswp.cpp main.h lbm-dswp.cpp lbm.h lbm_1d_array.h config.h
	clang++ -std=c++11 main-dswp.cpp lbm-dswp.cpp -lm -lpthread -o ../lbm-dswp

../lbm-prof: main-prof.c main.h lbm-prof.c lbm.h lbm_1d_array.h config.h
	clang main-prof.c lbm-prof.c -lm -o ../lbm-prof
