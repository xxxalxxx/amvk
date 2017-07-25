#ifndef AMVK_TILED_RENDERER_H
#define AMVK_TILED_RENDERER_H

#include "macro.h"
#include "vulkan.h"
#include "state.h"

class TiledRenderer {
public:
	TiledRenderer(State& state);
	virtual ~TiledRenderer();
	void init();
private:
};

#endif
