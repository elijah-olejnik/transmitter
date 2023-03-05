#include <iostream>
#include <vector>
#include <boost/bind/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
boost::mutex io_mutex;
class Stream {
public:
    explicit Stream(const unsigned char &id) : mID(id) {}
    virtual ~Stream() {}
    void send(short *value) {
		boost::unique_lock<boost::mutex> lock(io_mutex);
		std::cout << "Within a class: " << value[mID] << std::endl;
    }
private:
	unsigned char mID;
};
class Transmitter {
public:
	Transmitter(const std::vector<Stream> &s) : mS(s) {
		mSNum = mS.size();
		pmThreads = new boost::asio::thread_pool(mSNum);
	}
	~Transmitter() {
		pmThreads->join();
		delete pmThreads;
	}
	void transmit(short *data) {
		for (Stream s : mS) 
			boost::asio::post(*pmThreads, boost::bind(&Stream::send, s, data));
	}
private:
	std::vector<Stream> mS;
	boost::asio::thread_pool *pmThreads;
	unsigned char mSNum;
};
int main() {
	Stream stream(1);
	Stream stream2(2);
	std::vector<Stream> streams;
	streams.push_back(stream);
	streams.push_back(stream2);
	Transmitter t(streams);
	short arr[] = {0, -10, 20, 100};
	t.transmit(arr);
	return 0;
}
