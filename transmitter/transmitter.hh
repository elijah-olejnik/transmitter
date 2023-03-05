#pragma once
#include "icestream.hh"
class Transmitter {
public:
	static int inputCallBack(const void *inBuffer, void *outBuffer,
			unsigned long framesPerBuffer,
			const PaStreamCallbackTimeInfo *timeInfo,
			PaStreamCallbackFlags statusFlags, void* userData) {
		return reinterpret_cast<Transmitter*>(userData)->input(inBuffer,
				outBuffer, framesPerBuffer, timeInfo, statusFlags);
	}
	explicit Transmitter();
	~Transmitter();
	void addStream(const IceStream &s);
	void start();
	void stop();
	PaStreamParameters config;
	unsigned short sampleRate, bufferSize;
private:
	int input(const void *inBuffer, void *outBuffer,
			unsigned long framesPerBuffer,
			const PaStreamCallbackTimeInfo *timeInfo,
			PaStreamCallbackFlags statusFlags);
	PaStream *m_pInStream;
	std::vector<IceStream> mOutStreams;
	boost::asio::thread_pool *m_pStreamPool;
};
