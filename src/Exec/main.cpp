#pragma once

#include <iostream>
#include <iomanip>
#include <string>
#include <memory>
#include <vector>

#include "AudioFileIf.h"
#include "ErrorDef.h"
#include "Exception.h"
#include "Convolver.h"

using std::string;
using std::vector;
using std::unique_ptr;
using std::cout;
using std::endl;

int main(int argc, char* argv[])
{
	static const int fileBlockSize = 1023;
	const int labelFormat = 30;
	long long irLengthInFrames = 0;
	double irLengthInSeconds = 0;

	CAudioFileIf* audioFileIn = nullptr;
	CAudioFileIf* irFileIn = nullptr;
	CAudioFileIf* audioFileOut = nullptr;
	CAudioFileIf::FileSpec_t inputFileSpec;
	CAudioFileIf::FileSpec_t irFileSpec;

	float** inputAudioData = nullptr;
	float** irAudioData = nullptr;
	float** outputAudioData = nullptr;
	float** tailAudioData = nullptr;
	std::vector<std::unique_ptr<Convolver>> convolver;

	string inputFilePath{};
	string irFilePath{};
	string outputFilePath{};

	cout << "-- Convolver Effect -- " << endl;

	switch (argc) {
	case 3:
		irFilePath = argv[2];
	case 2:
		inputFilePath = argv[1];
		break;
	case 1:
		std::cout << "Enter your parameters: " << std::endl;
		break;
	default:
		std::cout << "Incorrect Number of arguments...";
		return -1;
	}

	try {
		// Open Input File
		cout << endl;
		cout << std::setw(labelFormat) << std::right << "Input WAV file: ";
		if (inputFilePath.empty()) {
			std::cin >> inputFilePath;
		}
		else {
			std::cout << inputFilePath << std::endl;
		}
		CAudioFileIf::create(audioFileIn);
		audioFileIn->openFile(inputFilePath, CAudioFileIf::FileIoType_t::kFileRead);
		if (!audioFileIn->isOpen()) {
			throw Exception("Couldn't open input file...");
		}
		audioFileIn->getFileSpec(inputFileSpec);
		if (inputFileSpec.iNumChannels != 1 && inputFileSpec.iNumChannels != 2) {
			throw Exception("Input file must be mono or stereo...");
		}

		// Open Ir File
		cout << std::setw(labelFormat) << "IR file: ";
		if (irFilePath.empty()) {
			std::cin >> irFilePath;
		}
		else {
			std::cout << irFilePath << std::endl;
		}
		CAudioFileIf::create(irFileIn);
		irFileIn->openFile(irFilePath, CAudioFileIf::FileIoType_t::kFileRead);
		if (!irFileIn->isOpen()) {
			throw Exception("Couldn't open ir file...");
		}
		irFileIn->getFileSpec(irFileSpec);
		irFileIn->getLength(irLengthInSeconds);
		if (irFileSpec.iNumChannels != 1 && irFileSpec.iNumChannels != 2) {
			throw Exception("IR file must be mono or stereo...");
		}
		
		// Check Compatibility
		if (irFileSpec.iNumChannels != inputFileSpec.iNumChannels) {
			throw Exception("Number of channels do not match...");
		}

		// Load Ir File
		irFileIn->getLength(irLengthInFrames);
		irAudioData = new float* [irFileSpec.iNumChannels]{};
		for (int c = 0; c < irFileSpec.iNumChannels; c++) {
			irAudioData[c] = new float[irLengthInFrames] {};
		}
		if (irFileIn->readData(irAudioData, irLengthInFrames) != Error_t::kNoError) {
			throw Exception("Error loading IR file...");
		}

		// Open Output File
		outputFilePath = inputFilePath;
		for (int i = 0; i < 4; i++) {
			outputFilePath.pop_back();
		}
		outputFilePath.append("Out.wav");
		CAudioFileIf::create(audioFileOut);
		audioFileOut->openFile(outputFilePath, CAudioFileIf::FileIoType_t::kFileWrite, &inputFileSpec);
		if (!audioFileOut->isOpen()) {
			throw Exception("Couldn't open output file...");
		}

		// Create Instances
		for (int c = 0; c < inputFileSpec.iNumChannels; c++) {
			convolver.emplace_back(new Convolver());
			convolver[c]->init(irAudioData[c], irLengthInFrames);
		}

		// Allocate memory
		inputAudioData = new float* [inputFileSpec.iNumChannels]{};
		outputAudioData = new float* [inputFileSpec.iNumChannels]{};
		tailAudioData = new float* [inputFileSpec.iNumChannels]{};
		for (int c = 0; c < inputFileSpec.iNumChannels; c++) {
			inputAudioData[c] = new float[fileBlockSize] {};
			outputAudioData[c] = new float[fileBlockSize] {};
			tailAudioData[c] = new float[convolver[c]->getTailLength()]{};
		}

		// Process
		long long iNumFrames = fileBlockSize;
		while (!audioFileIn->isEof()) {
			if (audioFileIn->readData(inputAudioData, iNumFrames) != Error_t::kNoError) {
				throw Exception("Data reading error...");
			};
			for (int c = 0; c < inputFileSpec.iNumChannels; c++) {
				convolver[c]->process(inputAudioData[c], outputAudioData[c], iNumFrames);
			}
			if (audioFileOut->writeData(outputAudioData, iNumFrames) != Error_t::kNoError) {
				throw Exception("Data writing error...");
			};
		}

		// Process tail
		for (int c = 0; c < inputFileSpec.iNumChannels; c++) {
			convolver[c]->flushBuffer(tailAudioData[c]);
		}
		audioFileOut->writeData(tailAudioData, convolver[0]->getTailLength());

		cout << endl;
		cout << std::setw(labelFormat) << "Done processing..." << endl;
		cout << "\n-- Convolver Effect -- " << endl;
		cout << endl;

		// Clean-up
		for (int c = 0; c < inputFileSpec.iNumChannels; c++) {
			delete[] inputAudioData[c];
			delete[] outputAudioData[c];
			delete[] tailAudioData[c];
		}
		for (int c = 0; c < irFileSpec.iNumChannels; c++) {
			delete[] irAudioData[c];
		}
		delete[] irAudioData;
		delete[] inputAudioData;
		delete[] outputAudioData;
		delete[] tailAudioData;

		CAudioFileIf::destroy(audioFileIn);
		CAudioFileIf::destroy(irFileIn);
		CAudioFileIf::destroy(audioFileOut);

		return 0;
	}
	catch (Exception& ex) {
		if (audioFileIn) {
			CAudioFileIf::destroy(audioFileIn);
		}
		if (irFileIn) {
			CAudioFileIf::destroy(irFileIn);
		}
		if (audioFileOut) {
			CAudioFileIf::destroy(audioFileOut);
		}
		if (inputAudioData) {
			for (int c = 0; c < inputFileSpec.iNumChannels; c++) {
				delete[] inputAudioData[c];
			}
			delete[] inputAudioData;
		}
		if (irAudioData) {
			for (int c = 0; c < irFileSpec.iNumChannels; c++) {
				delete[] irAudioData[c];
			}
			delete[] irAudioData;
		}
		if (outputAudioData) {
			for (int c = 0; c < inputFileSpec.iNumChannels; c++) {
				delete[] outputAudioData[c];
			}
			delete[] outputAudioData;
		}
		if (tailAudioData) {
			for (int c = 0; c < inputFileSpec.iNumChannels; c++) {
				delete[] tailAudioData[c];
			}
			delete[] tailAudioData;
		}
		audioFileIn = nullptr;
		irFileIn = nullptr;
		audioFileOut = nullptr;
		inputAudioData = nullptr;
		irAudioData = nullptr;
		outputAudioData = nullptr;
		tailAudioData = nullptr;

		cout << "\n--------------------------" << endl;
		cout << ex.what() << endl;
		cout << "--------------------------" << endl;
		return -1;
	}
}

