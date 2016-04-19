#ifndef H_COMPOSITING
#define H_COMPOSITING

#include "base.h"


class Compositing : public Base {
public:
	Compositing();
	~Compositing();

	void init();
	void render();
};

#endif