#include "ThreadManager.h"
#include "BucketManager.h"
#include "FrameBuffer.h"
#include "Renderer.h"
#include <thread> 
#include <vector>

ThreadManager::ThreadManager()
{
    //may return 0 when not able to detect
    _maxThreadCount = std::thread::hardware_concurrency();
    if(_maxThreadCount == 0) {
        _maxThreadCount = DEFAULT_THREAD_COUNT;
    } else {
        _maxThreadCount -= 1;
    }
}

void ThreadManager::startRender(Renderer& renderer, FrameBuffer& framebuffer, BucketManager& bucketManager) const
{
    std::vector<std::thread> renderThreads;
    renderThreads.reserve(_maxThreadCount);
    for (int threadId = 0; threadId < _maxThreadCount; ++threadId) {
        renderThreads.emplace_back([this, &renderer, &bucketManager, &framebuffer]() {            
            Bucket bucket;
            // Generate buckets on the fly until no pixels left for rendering
            while(!_abort && bucketManager.generateBucket(bucket)) {
                for (uint32_t i = bucket.StartingPixel; i < bucket.EndingPixel; ++i) {
                    framebuffer[i] = renderer.castCameraRay(i);
                }
            }
        });
    }

    // Wait for all threads to finish
    for (auto& th : renderThreads) {
        th.join();
    }
}