#pragma once
#include "global.hh"
class IceStream {
public:
	explicit IceStream();
	~IceStream();
	void setHost(const char *hostIP);
	void setPort(unsigned short port);
	void setPassword(const char *pass);
	void setMount(const char *mnt);
	void setUser(const char *src);
	void setBitRate(unsigned char kbits);
	void setChannels(short left, short right = -1);
	void setupLame();
	void transmit(short *data);
	void stop();
private:
	shout_t *m_pShout;
	lame_global_flags *m_pLame;
	int m_mp3BufLen; 
	unsigned short mBitRate, mDevChans;
	short mLeft, mRight;
	bool mStreamOpen;
};
