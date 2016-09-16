#ifndef AMVK_DEVICE_QUEUE_INDICES
#define AMVK_DEVICE_QUEUE_INDICES

#include "macro.h"

class DeviceQueueIndicies {
public:
	DeviceQueueIndicies();
	void unset();
	bool allIndicesSet();
	bool graphicsIndexSet();
	bool supportedIndexSet();
	bool computeIndexSet();
	bool transferIndexSet();

	void setGraphicsIndex(int index);
	void setSupportedIndex(int index);
	void setComputeIndex(int index);
	void setTransferIndex(int index);

	static constexpr int 
		NUM_INDICES = 4, INDEX_UNSET = -1;

private:
	void setIndex(int& currIndex, int index);
	int mGraphicsIndex, mSupportedIndex, mComputeIndex, mTransferIndex;
	int mNumIndicesFound; 
};

#endif
