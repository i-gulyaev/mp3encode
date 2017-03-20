#include <iostream>
#include <algorithm>
#include <string>
#include <thread>

#include <dirent.h>

#include "encoding_job.h"
#include "thread_safe_queue.h"

std::string getMp3FileName(const std::string& name)
{
    auto pos = name.find_last_of('.');
    auto extension = name.substr(pos+1);
    std::transform(extension.begin(), extension.end(), extension.begin(), tolower);
    std::string mp3FileName;
    if (extension == "wav") {
        mp3FileName = name.substr(0,pos) + ".mp3";
    }
    return mp3FileName;
}

int main(int argc, char *argv[])
{
    DIR *dir;
    struct dirent *entry;

    if (argc < 2) {
        std::cerr << "Usage: mp3encode <PATH>\n";
        return 1;
    }
    
    if ((dir = opendir(argv[1])) == nullptr) {
        std::cerr << "Failed to scan directory: " << argv[1] << '\n';
        return 1;
    }

    using FilesPair = std::pair<std::string, std::string>;
    using TSQueue = ThreadSafeQueue<FilesPair>;
    TSQueue queue;
    
    while((entry = readdir(dir)) != nullptr) {
        if (entry->d_type != DT_REG) continue;

        auto mp3FileName = getMp3FileName(entry->d_name);
        if (mp3FileName.empty()) continue;

        std::cout << entry->d_name << '\n';
        auto pcmFile = std::string(argv[1]) + "/" + entry->d_name;
        auto mp3File = std::string(argv[1]) + "/" + mp3FileName;
        queue.push(std::make_pair(pcmFile, mp3File));
    }

    auto numThreads = (std::min)(std::thread::hardware_concurrency(), queue.size());
    
    std::vector<std::thread> workers;
    
    for (auto i = 0u; i < numThreads; ++i) {
        std::cout << "Spawn encoding thread #" << i << '\n';
        workers.push_back(
            std::thread([&queue](){
                    while(!queue.empty()) {
                        FilesPair f;
                        queue.pop(f);
                        EncodingJob::encode(f.first, f.second);
                    }
                }));
    }

    for (auto& worker : workers) {
        worker.join();
    }
    
    return 0;
}
