#include "transmitter.hh"
Transmitter::Transmitter() {}
Transmitter::~Transmitter() {
	m_pStreamPool->join();
	delete m_pStreamPool; 
}
void Transmitter::addStream(const IceStream &s) { mOutStreams.push_back(s); }
void Transmitter::start() {
	m_pStreamPool = new boost::asio::thread_pool(mOutStreams.size());
    PaError ret = Pa_OpenStream(&m_pInStream, &config, NULL, sampleRate,
			bufferSize, paClipOff, &inputCallBack, this);
	if (ret != paNoError) throw TMException(ret);
	ret = Pa_StartStream(m_pInStream);
	if (ret != paNoError) throw TMException(ret);
}
void Transmitter::stop() {
    PaError ret = Pa_StopStream(m_pInStream);
	if (ret != paNoError) throw TMException(ret);
    ret = Pa_CloseStream(m_pInStream);
	if (ret != paNoError) throw TMException(ret);
	for (IceStream &s : mOutStreams) s.stop();
}
int Transmitter::input(const void *inBuffer, void *outBuffer,
		unsigned long framesPerBuffer,
		const PaStreamCallbackTimeInfo *timeInfo,
		PaStreamCallbackFlags statusFlags) {
	const short *buf = reinterpret_cast<const short*>(inBuffer);
	for (IceStream &s : mOutStreams) {
		boost::asio::post(*m_pStreamPool,
				boost::bind(&IceStream::transmit, s, buf));
	}
	return paContinue;
}
