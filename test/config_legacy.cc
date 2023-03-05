#include <iostream>
#include <fstream>
#include "dspsettings.h"
#include "icestream.h"
using namespace std;
const string inputError = "\nEntered value not supported, try again: ";
const string cfgFileName = "config.ini";

bool check(int code, bool check, string msg) {
	if (code < 0) {
		cerr << Pa_GetErrorText(code) << endl;
		return false;
	}
	if (!check) {
		cerr << msg << endl;
		return check;
	}
	return true;
}

int main() {
	if (!check(Pa_Initialize(), true, "")) return -1;
	PaDeviceIndex maxDevs = Pa_GetDeviceCount();
	if (!check(maxDevs, maxDevs > 0, "No audio devices detected!")) {
		if (!check(Pa_Terminate(), true, "")) return -4;
		return -2;
	}
	vector<pair<int, const PaDeviceInfo*>> inDevs;
	for (int i = 0; i < maxDevs; i++) {
		const PaDeviceInfo *paNfo = Pa_GetDeviceInfo(i);
		if (paNfo->maxInputChannels > 0) {
			pair<int, const PaDeviceInfo*> device;
			device.first = i;
			device.second = paNfo;
			inDevs.push_back(device);
		}
	}
	if (!check(
				0, inDevs.size() > 0,
				"No input capable audio devices detected!"
				)) {
		if (!check(Pa_Terminate(), true, "")) return -4;
		return -3;
	}
	cout << endl << "The following input capable audio interfaces detected:"
		<< endl << endl;
	for (pair<int, const PaDeviceInfo*> dev : inDevs) {
		cout << "ID: " << dev.first << " Name: " << dev.second->name
			<< " Inputs: " << dev.second->maxInputChannels << endl;
	}
	DSPSettings dspSetup;
	int intPut = 0;
	cout << endl << "Enter samplerate (from 32000 to 48000): ";
	bool c = false;
	while (!c) {
		cin >> intPut;
		switch (intPut) {
			case 32000:
				c = true;
				break;
			case 44100:
				c = true;
				break;
			case 48000:
				c = true;
				break;
			default:
				cout << inputError;
				break;
		}
	}
	c = false;
	dspSetup.setSampleRate(intPut);
	intPut = 0;
	cout << "Enter bit depth (16 or 24): ";
	while (!c) {
		cin >> intPut;
		switch (intPut) {
			case 16:
				dspSetup.setBitDepth(paInt16);
				c = true;
				break;
			case 24:
				dspSetup.setBitDepth(paInt24);
				c = true;
				break;
			default:
				cout << inputError;
				break;
		}
	}
	c = false;
	intPut = 0;
	cout << "Enter buffer size: ";
	while (!c) {
		cin >> intPut;
		if (intPut < 32) cout << inputError;
		else c = true;
	}
	c = false;
	dspSetup.setFramesPerBuffer(intPut);
	intPut = 0;
	cout << "Enter the desired number of the broadcast streams: ";
	while (!c) {
		cin >> intPut;
		if (intPut < 0) cout << inputError;
		else c = true;
	}
	c = false;
	vector<IceStream> iStreams;
	for (int i = 0; i < intPut; i++) {
		IceStream iStr;
		string input;
		cout << "Enter stream " << i + 1 << " mount name: ";
		cin >> input;
		iStr.setMount(input);
		cout << "Enter host IP:port: ";
		cin >> input;
		iStr.setHost(input);
		cout << "Enter password: ";
		cin >> input;
		iStr.setPassword(input);
		int nInput = 0;
		while (!c) {
			cout << "Enter audio interface index: ";
			cin >> nInput;
			for (pair<int, const PaDeviceInfo*> i : inDevs)
				if (i.first == nInput) c = true;
		}
		iStr.setDeviceIndex(nInput);
		c = false;
		cout << "Enter 1 for mono 2 for stereo: ";
		while (!c) {
			cin >> nInput;
			if (nInput > 0 && nInput < 3) c = true;
			else cout << inputError;
		}
		vector<int> chSelection;
		for (int i = 0; i < nInput; i++) {
			string ch = "mono";
			if (nInput > 1 && i < 1) ch = "left";
			else if (i > 1) ch = "right";
			cout << "Choose " << ch << " channel: ";
			int chNum = 0;
			cin >> chNum;
			chSelection.push_back(chNum);
		}
		iStr.setChannels(chSelection);
		iStreams.push_back(iStr);
	}
	ifstream f(cfgFileName);
	if (f.good()) remove(cfgFileName.c_str());
	ofstream config;
	config.open(cfgFileName);
	config << "[global]" << endl;
	config << "samplerate = " << dspSetup.getSampleRate() << endl;
	config << "bitrate = " << dspSetup.getBitDepth() << endl;
	config << "buffer = " << dspSetup.getFramesPerBuffer() << endl;
	config << "streams = " << intPut << endl << endl;
	for (IceStream s : iStreams) {
		config << "[" << s.getMount() << "]" << endl;
		config << "host = " << s.getHost() << endl;
		config << "password = " << s.getPassword() << endl;
		config << "audiointerface = " << s.getDeviceIndex() << endl;
		config << "channels = " << s.getChannels().size() << endl;
		for (int i = 0; i < s.getChannels().size(); i++)
			config << "channel_" << i + 1 << " = " << s.getChannels()[i] << endl;
		config << endl;
	}
	config.close();
	if (!check(Pa_Terminate(), true, "")) return -4;
	return 0;
}
