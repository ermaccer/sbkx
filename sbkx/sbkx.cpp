// sbkx.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <memory>
#include <string>
#include <filesystem>

#include "wav.h"

#pragma pack(push, 1)
struct sbk_entry {
    unsigned short id;
    int flags;
    short field6;
    int frequency;
    int size;
};
#pragma (pop)


int main(int argc, char* argv[])
{
    if (argc < 1)
        return 0;

    std::ifstream pFile(argv[1], std::ifstream::binary);

    if (!pFile)
    {
        std::cout << "ERROR: Could not open " << argv[1] << "!" << std::endl;;
        return 1;
    }

    std::string input = argv[1];
    std::string folder = input.substr(0, input.find_last_of("."));
  
    std::filesystem::create_directory(folder);
    std::filesystem::current_path(folder);

    unsigned short numSounds = 0;
    pFile.read((char*)&numSounds, sizeof(short));

    std::cout << "INFO: Extracting " << numSounds << " sounds!" << std::endl;

    for (int i = 0; i < numSounds; i++)
    {
        sbk_entry sbk;
        pFile.read((char*)&sbk, sizeof(sbk_entry));

        std::string outName = folder;
        outName += "_";
        outName += std::to_string(i);
        outName += ".wav";

        std::ofstream oFile(outName, std::ofstream::binary);

        wav_header wav;
        wav.header = 'FFIR';
        wav.filesize = sbk.size + 36;
        wav.waveheader = 'EVAW';
        wav.format = ' tmf';
        wav.sectionsize = 16;
        wav.waveformat = 1;
        wav.channels = 1;
        wav.samplespersecond = sbk.frequency;
        wav.bytespersecond = sbk.frequency;
        wav.blockalign = 2;
        wav.bitspersample = 8;
        wav.dataheader = 'atad';
        wav.datasize = wav.filesize - sizeof(wav_header) - 8;
        // write wave header
        oFile.write((char*)&wav, sizeof(wav_header));

        int dataSize = sbk.size;
        std::unique_ptr<char[]> dataBuff = std::make_unique<char[]>(dataSize);
        pFile.read(dataBuff.get(), dataSize);
        oFile.write(dataBuff.get(), dataSize);
    }
    std::cout << "INFO: Finished!" << std::endl;
    return 0;
}
