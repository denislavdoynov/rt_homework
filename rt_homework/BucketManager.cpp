#include "BucketManager.h"
#include "Scene.h"
#include <cassert>

void BucketManager::updateSceneSettings(const Settings& settings) 
{ 
    _bucketSize = settings.BucketSize * settings.BucketSize;
    _totalPixels = settings.ImageWidth * settings.ImageHeight; 
    _nextBucket = 0; 
}

bool BucketManager::generateBucket(Bucket& bucket) 
{   
    assert(_bucketSize != 0);
    if(_nextBucket >= _totalPixels || _bucketSize == 0) {
        return false;
    }

    bucket.StartingPixel = _nextBucket;
    bucket.EndingPixel = bucket.StartingPixel + _bucketSize;
    
    if(bucket.EndingPixel > _totalPixels)
        bucket.EndingPixel = _totalPixels;

    _nextBucket = bucket.EndingPixel;

    return true;
}
