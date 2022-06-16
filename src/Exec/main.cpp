#pragma once

#include <iostream>
#include <iomanip>
#include <string>
#include <memory>
#include <vector>

#include "AudioFileIf.h"
#include "ErrorDef.h"
#include "Exception.h"
#include "ConvolverIf.h"

using std::string;
using std::vector;
using std::unique_ptr;
using std::cout;
using std::endl;

int main(int argc, char* argv[])
{
	static const int fileBlockSize = 1023;
	const int labelFormat = 30;

	CAudioFileIf* audioFileIn = nullptr;
	CAudioFileIf* audioFileOut = nullptr;
	CAudioFileIf::FileSpec_t fileSpec;

	float** inputAudioData = nullptr;
	float** outputAudioData = nullptr;
	std::vector<std::unique_ptr<ConvolverIf>> convolver;

	string inputFilePath{};
	string outputFilePath{};

	cout << "-- CombFilter Effect -- " << endl;

	try {
		// Open Input File
		cout << endl;
		cout << std::setw(labelFormat) << std::right << "Input WAV file: ";
		CAudioFileIf::create(audioFileIn);
		audioFileIn->openFile(inputFilePath, CAudioFileIf::FileIoType_t::kFileRead);
		if (!audioFileIn->isOpen()) {
			throw Exception("Couldn't open input file...");
		}
		audioFileIn->getFileSpec(fileSpec);

		// Open Output File
		cout << std::setw(labelFormat) << "Output WAV file: ";
		std::cin >> outputFilePath;
		CAudioFileIf::create(audioFileOut);
		audioFileOut->openFile(outputFilePath, CAudioFileIf::FileIoType_t::kFileWrite, &fileSpec);
		if (!audioFileOut->isOpen()) {
			throw Exception("Couldn't open output file...");
		}

		// Create Instances

		// Allocate memory
		inputAudioData = new float* [fileSpec.iNumChannels]{};
		outputAudioData = new float* [fileSpec.iNumChannels]{};
		for (int c = 0; c < fileSpec.iNumChannels; c++) {
			inputAudioData[c] = new float[fileBlockSize] {};
			outputAudioData[c] = new float[fileBlockSize] {};
		}

		// Process
		long long iNumFrames = fileBlockSize;
		while (!audioFileIn->isEof()) {
			if (audioFileIn->readData(inputAudioData, iNumFrames) != Error_t::kNoError) {
				throw Exception("Data reading error...");
			};
			// Place process function here...s
			if (audioFileOut->writeData(outputAudioData, iNumFrames) != Error_t::kNoError) {
				throw Exception("Data writing error...");
			};
		}

		cout << endl;
		cout << std::setw(labelFormat) << "Done processing..." << endl;
		cout << "\n-- CombFilter Effect -- " << endl;
		cout << endl;

		// Clean-up
		for (int c = 0; c < fileSpec.iNumChannels; c++) {
			delete[] inputAudioData[c];
			delete[] outputAudioData[c];
		}
		delete[] inputAudioData;
		delete[] outputAudioData;

		CAudioFileIf::destroy(audioFileOut);
		CAudioFileIf::destroy(audioFileOut);

		return 0;
	}
	catch (Exception& ex) {
		if (audioFileIn) {
			CAudioFileIf::destroy(audioFileIn);
		}
		if (audioFileOut) {
			CAudioFileIf::destroy(audioFileOut);
		}
		if (inputAudioData) {
			for (int c = 0; c < fileSpec.iNumChannels; c++) {
				delete[] inputAudioData[c];
			}
			delete[] inputAudioData;
		}
		if (outputAudioData) {
			for (int c = 0; c < fileSpec.iNumChannels; c++) {
				delete[] outputAudioData[c];
			}
			delete[] outputAudioData;
		}
		audioFileIn = nullptr;
		audioFileOut = nullptr;
		inputAudioData = nullptr;
		outputAudioData = nullptr;

		cout << "\n--------------------------" << endl;
		cout << ex.what() << endl;
		cout << "--------------------------" << endl;
		return -1;
	}
}

