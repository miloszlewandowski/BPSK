#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <bitset>
#include <fstream>
#include <math.h>
#include <vector>
#include <array>


#define BUFFER_SIZE 128

int readBytesFromFile(std::ifstream *file,
                      char (*buffer)[BUFFER_SIZE],
                      int atOffset) {
    file->seekg(atOffset);
    file->read(reinterpret_cast<char *>(buffer), BUFFER_SIZE);
    //TODO try to implement this without calling file->gcount()
    int readBytesCount = file->gcount();
    return readBytesCount;
}

void writeBytesToFile(std::ofstream *file,
                      std::array<std::array<float, 8>, BUFFER_SIZE> (*buffer),
                      int atOffset,
                      int writeByteCount) {
    file->seekp(atOffset);
    file->write(reinterpret_cast<char *>(buffer), writeByteCount);
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
        int inputByte = (int)(*input)[i];
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


int main() {

    // computational complexity = THETA(n)
    // memory complexity = THETA(1)

    std::ifstream inputFile1 ("../input1.txt");
    std::ofstream outputFile1 ("../output1.txt");
    std::ifstream inputFile2 ("../input2.txt");
    std::ofstream outputFile2 ("../output2.txt");
    std::ifstream inputFile3 ("../input3.txt");
    std::ofstream outputFile3 ("../output3.txt");

    //This buffer is declared on the stack
    char inputBuffer[BUFFER_SIZE] = {0};
    std::array<std::array<float, 8>, BUFFER_SIZE> outputBuffer;

    int offset = 0;
    int bytesRead = BUFFER_SIZE;

    while (bytesRead == BUFFER_SIZE) {
        bytesRead = readBytesFromFile(&inputFile1, &inputBuffer, offset);
        if (bytesRead == 0) break;
        encode(&inputBuffer, &outputBuffer, bytesRead);
        writeBytesToFile(&outputFile1, &outputBuffer, offset, bytesRead);
        offset += bytesRead;
    }
}
