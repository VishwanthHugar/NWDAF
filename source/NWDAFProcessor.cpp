#include "NWDAFProcessor.h"
#include "Logger.h"


NWDAFProcessor::NWDAFProcessor(SafeQueue<std::shared_ptr<AnalyticsEvent>>& queue,
    std::shared_ptr<IEventRepository> repo,
    size_t batch_size,
    std::chrono::milliseconds flush_ms)
    : mQueue(queue), 
    repository(std::move(repo)),
    batchSize(batch_size),
    flushInterval(flush_ms), 
    mRunning(false) {
}

NWDAFProcessor::~NWDAFProcessor() {
    stop();
}

void NWDAFProcessor::start() {
    if (mRunning) 
        return;
    mRunning = true;
    workerThread = std::thread(&NWDAFProcessor::processLoop, this);
}

void NWDAFProcessor::stop() {
    std::cerr << "[NWDAFProcessor] Stopping NWDAFProcessor::stop() \n";
    mRunning = false;
    mQueue.close(); // signal queue is closed
    if (workerThread.joinable())
        workerThread.join();
}

void NWDAFProcessor::processLoop() {
    std::vector<std::shared_ptr<AnalyticsEvent>> batch;
    auto lastFlush = std::chrono::steady_clock::now();

    while (mRunning || !mQueue.empty()) {
        std::cout << "[NWDAFProcessor] Before Pop mQueue length: " << mQueue.size() << std::endl;

        auto event = mQueue.pop(); // blocks until event available or queue closed
        if (!event.has_value() || !event.value()) 
            continue;      // skip if queue closed

        std::cout << "[NWDAFProcessor] Afer Pop mQueue length: " << mQueue.size() <<std::endl;
        batch.push_back(event.value());

        auto now = std::chrono::steady_clock::now();
        bool flushByTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastFlush) >= flushInterval;
        bool flushBySize = batch.size() >= batchSize;
        std::cout << "[NWDAFProcessor] ---------->batch.size: " << batch.size() << std::endl;

        if (flushBySize || flushByTime) {
            std::cout << "[NWDAFProcessor] batch.size: "<< batch.size() <<std::endl;
            if (!batch.empty()) {
                if (!repository) {
                    // log and skip this batch
                    std::cerr << "repository is null or corrupted, skipping batch\n";
                }
                else {
                    try {
                        repository->insertBatch(batch);
                    }
                    catch (const std::exception& e) {
                        std::cerr << "[NWDAFProcessor] insertBatch threw: " << e.what() << '\n';
                    }
                    catch (...) {
                        std::cerr << "[NWDAFProcessor] insertBatch threw unknown exception\n";
                    }
                }
                std::cout << "[NWDAFProcessor] Clearing batch\n";
                batch.clear();
                lastFlush = now;
            }
        }
    }

    // Flush remaining events before exit
    if (!batch.empty()) {
        if (repository) {
            try {
                repository->insertBatch(batch);
            }
            catch (const std::exception& e) {
                std::cerr << "[NWDAFProcessor] final insertBatch threw: " << e.what() << '\n';
            }
            catch (...) {
                std::cerr << "[NWDAFProcessor] final insertBatch threw unknown exception\n";
            }
        }
        else {
            std::cerr << "[NWDAFProcessor] repository null on final flush, dropping batch\n";
        }
        std::cout << "[NWDAFProcessor] final flush, clear batch\n";
        batch.clear();
    }
}
