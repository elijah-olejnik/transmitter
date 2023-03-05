#pragma once
#include "global.hh"
class IceStream {
public:
	explicit IceStream();
	~IceStream();
	void setHost(const char *hostIP);
	void setPort(const unsigned short &port);
	void setPassword(const char *pass);
	void setMount(const char *mnt);
	void setUser(const char *src);
	void setupLame(unsigned short &smpRate, unsigned short bufSize,
			unsigned short devChans);
	void transmit(short *data);
	void stop();
	unsigned short bitRate;
	char chLeft, chRight = -1;
private:
	shout_t *m_pShout;
	lame_global_flags *m_pLame;
	unsigned short m_pcmFrames, m_mp3BufLen;
	unsigned char mDevChans;
	bool mStreamOpen;
};
