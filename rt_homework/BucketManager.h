#pragma once

#include <atomic>

struct Bucket {
	uint32_t StartingPixel = 0;
	uint32_t EndingPixel = 0;
};

struct Settings;
class BucketManager
{

public:
	bool generateBucket(Bucket& bucket);
	void updateSceneSettings(const Settings& settings);
	uint32_t totalPixels() const { return _totalPixels; }

private:    
	uint32_t _bucketSize = 0;
	uint32_t _totalPixels = 0;
	std::atomic_uint32_t _nextBucket{ 0 };
};

