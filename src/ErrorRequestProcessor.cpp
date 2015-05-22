#include <system_error>

#include "ErrorRequestProcessor.h"

ErrorRequestProcessor::ErrorRequestProcessor() {

}

void ErrorRequestProcessor::process(HttpRequest& req, DataBuffer<uint8_t>& buffer) {
	
	std::string content;
	DataBuffer<char> content404;
	HttpStatus status;

	if (code_ == EACCES) {
		status = FORBIDDEN;
		content = "403 ";

		content += req.path;
	
		makeHttpResponse(req, content.data(), content.size(), buffer, status);
	} else if (code_ == ENOENT) {
		status = NOT_FOUND;
//		content = "404 ";		
		const std::string path = std::string("../html/404.html");

		FilePro::File file(path, path);
		file.content(content404);
		makeHttpResponse(req, content404.data(), content404.size(), buffer, status);


	} else {
		status = INTERNAL_ERROR;
		content = "500 ";
		
		content += req.path;
	
		makeHttpResponse(req, content.data(), content.size(), buffer, status);
	}

	
	
	(void)req;

}

bool ErrorRequestProcessor::isEligible(const HttpRequest& req) const {
	(void)req;
	return true;
}

void ErrorRequestProcessor::setLastErrorCode(int code) {
	code_ = code;
}
