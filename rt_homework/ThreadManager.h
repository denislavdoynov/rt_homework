#pragma once
#include <atomic>

constexpr int DEFAULT_THREAD_COUNT = 10;

class Renderer;
class FrameBuffer;
class BucketManager;

class ThreadManager
{
public:
    ThreadManager();

    void startRender(Renderer& renderer, FrameBuffer& framebuffer, BucketManager& bucketManager) const;
    void abort() { _abort = true; }

private:    
	std::atomic_bool _abort{ false };
    int _maxThreadCount = 0;
};

