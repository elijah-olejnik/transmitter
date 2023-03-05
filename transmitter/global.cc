#include "global.hh"
TMException::TMException(int paCode, shout_t *str, const char *msg)
: code(paCode), stream(str), message((char*)msg) {
	if (code != 0) message = (char*)Pa_GetErrorText(code);
	if (stream) message = (char*)shout_get_error(stream);
}
char *TMException::what() { return message; }
