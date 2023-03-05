#include "transmitter.hh"
void split(std::string &src, std::string *out) {
	std::string chunk;
	for (int i = 0; i < 2; i++) {
		std::getline(std::stringstream(src), chunk, '=');
		out[i] = chunk;
	}
}
void setup(Settings &global, std::vector<IceStream> &broads,
	std::vector<Transmitter> &engines) {
	std::ifstream file("transmitter.conf");
	if (file.is_open()) {
		std::string line, val[2];
		while (std::getline(file, line)) {
			if (line.empty() || line[0] == '#') continue;
			else if (line[0] == '[' && line.find('.') == std::string::npos) {
				//transmitter create
				IceStream stream = IceStream(&global);
				do {
					std::getline(file, line);
					split(line, val);
					if (val[0] == "host") stream.setHost(val[1].c_str());
					else if (val[0] == "port")
						stream.setPort(std::stoi(val[1]));
					else if (val[0] == "mountpoint")
						stream.setMount(val[1].c_str());
					else if (val[0] == "source")
						stream.setUser(val[1].c_str());
					else if (val[0] == "password")
						stream.setPassword(val[1].c_str());
					else if (val[0] == "audiointerface")
						stream.setInputDeviceIndex(std::stoi(val[1]), 8);
					else if (val[0] == "channels") {
						if (val[1].find(',') != std::string::npos) {
							std::string chunk;
							int ch[2];
							for (int i = 0; i < 2; i++) {
								std::getline(std::stringstream(val[1]), chunk, ',');
								ch[i] = std::stoi(chunk);
							}
							stream.setChannels(ch[0], ch[1]);
						} else stream.setChannels(std::stoi(val[1]));
					}
					else if (val[0] == "bitrate")
						stream.setBitRate(std::stoi(val[1]));
				} while (!line.empty());
				stream.setupLame();
				broads.push_back(stream);
			} else {
				split(line, val);
				if (val[0] == "samplerate") global.smpRate = std::stoi(val[1]);
				if (val[0] == "buffer") global.frmPerBuf = std::stoi(val[1]);
			}
		}
		file.close();
	} else throw TMException(0, nullptr, "can't open conf file");
}
int main(int argc, char* argv[]) {
	Settings cfg;
	std::vector<IceStream> streams;
	std::vector<Transmitter> sources;
	shout_init();
	try {
		PaError ret = Pa_Initialize();
		if (ret != paNoError) throw TMException(ret);
		setup(cfg, streams, sources);
		for (Transmitter &t : sources) t.start();
		//sleep here or wait for a signal
		int wait = 0;
		std::cin >> wait;
		for (Transmitter &t : sources) t.stop();
		ret = Pa_Terminate();
		if (ret != paNoError) throw TMException(ret);
		shout_shutdown();
		return 0;
	} catch (TMException tme) {
		std::cout << "FAILURE: " << tme.what() << std::endl;
		for (Transmitter &t : sources) t.stop();
		Pa_Terminate();
		shout_shutdown();
		return -1;
	}
}
