#include "device_queue_indices.h"

DeviceQueueIndicies::DeviceQueueIndicies(): 
	mGraphicsIndex(INDEX_UNSET),
	mSupportedIndex(INDEX_UNSET),
	mComputeIndex(INDEX_UNSET),
	mTransferIndex(INDEX_UNSET),
	mNumIndicesFound(0)
{

}

void DeviceQueueIndicies::unset() 
{
	mGraphicsIndex = mSupportedIndex = mComputeIndex = mTransferIndex = INDEX_UNSET;
}

bool DeviceQueueIndicies::allIndicesSet() 
{
	LOG("ALL INDICES SET");
	return mNumIndicesFound == NUM_INDICES;
}

bool DeviceQueueIndicies::graphicsIndexSet() 
{
	return mGraphicsIndex != INDEX_UNSET;
}

bool DeviceQueueIndicies::supportedIndexSet() 
{
	return mSupportedIndex != INDEX_UNSET;
}

bool DeviceQueueIndicies::computeIndexSet() 
{
	return mComputeIndex != INDEX_UNSET;
}

bool DeviceQueueIndicies::transferIndexSet() 
{
	return mTransferIndex != INDEX_UNSET;
}

void DeviceQueueIndicies::setGraphicsIndex(int index) 
{
	setIndex(mGraphicsIndex, index);
}

void DeviceQueueIndicies::setSupportedIndex(int index)
{
	setIndex(mSupportedIndex, index);
}

void DeviceQueueIndicies::setComputeIndex(int index) 
{
	setIndex(mComputeIndex, index);
}

void DeviceQueueIndicies::setTransferIndex(int index) 
{
	setIndex(mTransferIndex, index);
}

void DeviceQueueIndicies::setIndex(int& currIndex, int index)
{
	LOG("INDEX " << index << " UPDATED");
	if (index >= INDEX_UNSET && currIndex != index) {
		if (currIndex == INDEX_UNSET) 
			++mNumIndicesFound;
		 else if (index == INDEX_UNSET)
			 --mNumIndicesFound;
		currIndex = index;
	}
}

