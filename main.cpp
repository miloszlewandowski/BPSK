#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <bitset>
#include <fstream>
#include <math.h>
#include <vector>
#include <array>
#include <pthread.h>
#include <chrono>
#include <tuple>


#define BUFFER_SIZE 128

struct EncodeFileJob {
    std::string filePath;
    std::string outputPath;
    int totalTime;
};

int readBytesFromFile(std::ifstream *file,
                      char (*buffer)[BUFFER_SIZE],
                      int atOffset) {
    file->seekg(atOffset);
    file->read(reinterpret_cast<char *>(buffer), BUFFER_SIZE);
    //TODO try to implement this without calling file->gcount()
    int readBytesCount = file->gcount();
    return readBytesCount;
}

int writeBytesToFile(std::ofstream *file,
                      std::array<std::array<float, 8>, BUFFER_SIZE> (*buffer),
                      int atOffset,
                      int writeByteCount) {
    file->seekp(atOffset);

    int writtenSize = 0;
    for (int i = 0; i < writeByteCount; i++) {
        std::array<float, 8> e = (*buffer)[i];
        for (int j = 0; j < 8; j++) {
            short f = (short) round((e[j] * (2 << 13)));
            std::string s = std::to_string(f) + ",";
            file->write(s.c_str(), s.size());
            writtenSize += s.size();
        }
    }
    return writtenSize;
}



void encode(char (*input)[BUFFER_SIZE],
            std::array<std::array<float, 8>, BUFFER_SIZE> (*output),
            int byteCount) {

    long fc = 10000;
    long fs = 4*fc;
    int amplitude = 2;
    float timePerBit = 0.01;
    int samplesPerBit = round(timePerBit*fs);
    float basicCosineBody = 2*3.14*fc/fs;

    // signal patterns
    float sig1[samplesPerBit];
    memset( sig1, 0, samplesPerBit*sizeof(int));
    float sig0[samplesPerBit];
    memset( sig0, 0, samplesPerBit*sizeof(int));
    for (int i = 0; i < samplesPerBit-1; i++){
        sig1[i] = amplitude*cos(basicCosineBody*i);
        sig0[i] = -amplitude*cos(basicCosineBody*i);
    }
    for (int i = 0; i < byteCount; i++) {
        //how to encode a single byte
        int inputByte = (int) (*input)[i];
        std::array<float, 8> encodedInputByte;
        int j = 0;
        int bit = 0;
        while (inputByte != 0) {
            bit = inputByte%2 == 0 ? 0 : 1;
            encodedInputByte[j] = bit == 1 ? sig1[j] : sig0[j];
            inputByte /= 2;
            j++;
        }
        (*output)[i] = encodedInputByte;
    }
}

void * encodeFile(void * args) {
    EncodeFileJob *job = reinterpret_cast<EncodeFileJob *>(args);
    int totalTime = 0;
    int bytesRead1 = BUFFER_SIZE;
    std::ifstream inputFile1 (job->filePath);
    std::ofstream outputFile1 (job->outputPath);
    char inputBuffer1[BUFFER_SIZE] = {0};
    std::array<std::array<float, 8>, BUFFER_SIZE> outputBuffer;
    int offsetRead = 0;
    int offsetWrite = 0;
    while (bytesRead1 == BUFFER_SIZE) {
        bytesRead1 = readBytesFromFile(&inputFile1, &inputBuffer1, offsetRead);
        if (bytesRead1 == 0) break;
        auto start = std::chrono::high_resolution_clock::now();
        encode(&inputBuffer1, &outputBuffer, bytesRead1);
        auto finish = std::chrono::high_resolution_clock::now();
        offsetWrite += writeBytesToFile(&outputFile1, &outputBuffer, offsetWrite, bytesRead1);
        offsetRead += bytesRead1;
        totalTime += std::chrono::duration_cast<std::chrono::nanoseconds>(finish-start).count();
    }
    job->totalTime = totalTime;
}


int main() {

    std::vector<std::pair<std::string, std::string>> filePairs {
        std::make_pair("../input1.txt", "../output1.txt"),
        std::make_pair("../input2.txt", "../output2.txt"),
        std::make_pair("../input3.txt", "../output3.txt"),
    };

    pthread_t *threads = new pthread_t[filePairs.size()];
    EncodeFileJob **jobs = new EncodeFileJob*[filePairs.size()];

//    for (auto filePair : filePairs) {
    for (int i = 0; i < filePairs.size(); i++) {
        auto filePair = filePairs[i];
        EncodeFileJob *job = new EncodeFileJob();
        job->filePath = std::get<0>(filePair);
        job->outputPath = std::get<1>(filePair);
        jobs[i] = job;
        pthread_create(&threads[i], NULL, encodeFile, job);
    }

    int totalTime = 0;
    // synchronize to termination of all threads
    for (int i = 0; i < filePairs.size(); i++) {
        pthread_join(threads[i], NULL);
        EncodeFileJob *job = jobs[i];
        std::cout << "Time for file " << job->filePath << " : " << std::to_string(job->totalTime) << "ns" << std::endl;
        totalTime += job->totalTime;
    }

    std::cout << "Total time: " << std::to_string(totalTime) << "ns" << std::endl;

}
