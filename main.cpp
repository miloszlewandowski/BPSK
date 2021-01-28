#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <bitset>
#include <fstream>


#define BUFFER_SIZE 128


//std::bitset<8> convertToBinary(char word[], std::bitset<8> *&bit_result);


//int main() {
//
//    char *word = "ziobroziobro";
//    char z = '/';
//    cout << std::bitset<8>(z);
//    cout << endl;
//    std::bitset<8> *bit_result = new std::bitset<8>[sizeof(word)];
//
//    convertToBinary(word, bit_result);
//    cout << bit_result;
//    return 0;
//}

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
                      char (*buffer)[BUFFER_SIZE],
                      int atOffset,
                      int writeByteCount) {
    file->seekp(atOffset);
    file->write(reinterpret_cast<char *>(buffer), writeByteCount);
}

void encode(char (*input)[BUFFER_SIZE],
            char (*output)[BUFFER_SIZE],
            int byteCount) {
    for (int i = 0; i < byteCount; i++) {
        //how to encode a single byte
        char inputByte = (*input)[i];
        //TODO encode the byte
        char encodedInputByte = inputByte;
        (*output)[i] = encodedInputByte;
    }
}


int main() {

    // computational complexity = THETA(n)
    // memory complexity = THETA(1)

    std::ifstream inputFile ("../input.txt");
    std::ofstream outputFile ("../output.txt");

    //This buffer is declared on the stack
    char inputBuffer[BUFFER_SIZE] = {0};
    char outputBuffer[BUFFER_SIZE] = {0};

    int offset = 0;
    int bytesRead = BUFFER_SIZE;

    while (bytesRead == BUFFER_SIZE) {
        bytesRead = readBytesFromFile(&inputFile, &inputBuffer, offset);
        if (bytesRead == 0) break;
        encode(&inputBuffer, &outputBuffer, bytesRead);
        writeBytesToFile(&outputFile, &outputBuffer, offset, bytesRead);
        offset += bytesRead;
    }
}
