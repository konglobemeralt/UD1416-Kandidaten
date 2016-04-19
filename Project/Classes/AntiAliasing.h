#ifndef H_ANTIALIASING
#define H_ANTIALIASING

#include "base.h"


class AntiAliasing : public Base{
public:
	AntiAliasing();
	~AntiAliasing();

	void init();
	void render();
} antiAliasing;

#endif