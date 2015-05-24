#include <system_error>

#include "ErrorRequestProcessor.h"

ErrorRequestProcessor::ErrorRequestProcessor() {

}

void ErrorRequestProcessor::process(HttpRequest& req, DataBuffer<uint8_t>& buffer) {
	
	DataBuffer<char> content;
	HttpStatus status;

	if (code_ == EACCES) {
		status = FORBIDDEN;
		const std::string path = std::string("../html/403.html");
		FilePro::File file(path, path);
		file.content(content);

	} else if (code_ == ENOENT) {
		status = NOT_FOUND;	
		const std::string path = std::string("../html/404.html");
		FilePro::File file(path, path);
		file.content(content);

	} else {
		status = INTERNAL_ERROR;
		const std::string path = std::string("../html/500.html");
		FilePro::File file(path, path);
		file.content(content);
		
	}

	
	makeHttpResponse(req, content.data(), content.size(), buffer, status);	
	
	(void)req;

}

bool ErrorRequestProcessor::isEligible(const HttpRequest& req) const {
	(void)req;
	return true;
}

void ErrorRequestProcessor::setLastErrorCode(int code) {
	code_ = code;
}
