#ifndef H_TONEMAPPING
#define H_TONEMAPPING

#include "base.h"
class ToneMapping : public Base {
public:
	ToneMapping();
	~ToneMapping();
	
	void init();
	void render();
private:
	
};

#endif