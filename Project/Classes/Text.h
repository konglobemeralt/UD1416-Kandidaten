#ifndef H_TEXT
#define H_TEXT

#include "base.h"

class Text : public Base {
public:
	Text();
	~Text();

	void init();
	void render();
};

#endif