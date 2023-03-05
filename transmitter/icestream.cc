#include "icestream.hh"
IceStream::IceStream() : m_pLame(lame_init()), mStreamOpen(false) {
	if (!(m_pShout = shout_new()))
		throw TMException(0, nullptr, "shout_t allocation failed");
	if (shout_set_protocol(m_pShout, SHOUT_PROTOCOL_HTTP) != SHOUTERR_SUCCESS)
		throw TMException(0, m_pShout);
	if (shout_set_content_format(m_pShout, SHOUT_FORMAT_MP3,
			SHOUT_USAGE_UNKNOWN, NULL) != SHOUTERR_SUCCESS)
		throw TMException(0, m_pShout);
}
IceStream::~IceStream() { stop(); }
void IceStream::setHost(const char *hostIP) {
	if (shout_set_host(m_pShout, hostIP) != SHOUTERR_SUCCESS)
		throw TMException(0, m_pShout);
}
void IceStream::setPort(unsigned short &port) {
	if (shout_set_port(m_pShout, port) != SHOUTERR_SUCCESS)
		throw TMException(0, m_pShout);
}
void IceStream::setPassword(const char *pass) {
	if (shout_set_password(m_pShout, pass) != SHOUTERR_SUCCESS)
		throw TMException(0, m_pShout);
}
void IceStream::setMount(const char *mnt) {
	if (shout_set_mount(m_pShout, mnt) != SHOUTERR_SUCCESS)
		throw TMException(0, m_pShout); 
}
void IceStream::setUser(const char *src) {
	if (shout_set_user(m_pShout, src) != SHOUTERR_SUCCESS)
		throw TMException(0, m_pShout);
}
void IceStream::transmit(short *data) {
	if (!mStreamOpen) {
		if (shout_open(m_pShout) != SHOUTERR_SUCCESS)
			throw TMException(0, m_pShout);
		mStreamOpen = true;
	}
	unsigned char temp3[m_mp3BufLen];
	int converted = 0;
	if (mRight == -1) {
		short pcm[m_pSets->frmPerBuf];
		for (int f = 0; f < m_pSets->frmPerBuf; f++)
			pcm[f] = data[f * mDevChans + mLeft];
		converted = lame_encode_buffer(m_pLame, pcm, NULL, m_pSets->frmPerBuf,
				temp3, m_mp3BufLen);
	} else {
		short lpcm[m_pSets->frmPerBuf], rpcm[m_pSets->frmPerBuf];
		for (int f = 0; f < m_pSets->frmPerBuf; f++) {
			lpcm[f] = data[f * mDevChans + mLeft];
			rpcm[f] = data[f * mDevChans + mRight];
		}
		converted = lame_encode_buffer(m_pLame, lpcm, rpcm, m_pSets->frmPerBuf,
				temp3, m_mp3BufLen);
	}
	unsigned char mp3buf[converted];
	for (int i = 0; i < converted; i++) mp3buf[i] = temp3[i];
	if (shout_send(m_pShout, mp3buf, converted) != SHOUTERR_SUCCESS)
		throw TMException(0, m_pShout);
	shout_sync(m_pShout);
}
void IceStream::stop() {
	if (mStreamOpen) {
		shout_close(m_pShout);
		mStreamOpen = false;
	}
}

void IceStream::setupLame(unsigned short smpRate) {
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	 * LAME mp3 buffer size computing recomendations:
	 * num_samples*(bitrate/8)/samplerate + 4*1152*(bitrate/8)/samplerate + 512
	 * are optimized to:
	 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	int pcmBufLen = m_pSets->frmPerBuf * (mRight == -1 ? 1 : 2);
	m_mp3BufLen = (pcmBufLen * mBitRate + 4608 * mBitRate + 4096 * smpRate) 
			/ (8 * smpRate);
	//LAME settings here:
	lame_set_num_channels(m_pLame, mRight == -1 ? 1 : 2);
	lame_set_in_samplerate(m_pLame, smpRate);
	lame_set_VBR(m_pLame, vbr_off);
	lame_set_brate(m_pLame, mBitRate);
	lame_set_mode(m_pLame, mRight == -1 ? MONO : STEREO);
	lame_set_quality(m_pLame, 2);
	lame_set_out_samplerate(m_pLame, smpRate);
	lame_init_params(m_pLame);
}
