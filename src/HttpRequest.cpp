#include <iostream>

#include "HttpRequest.h"

HttpRequest::HttpRequest(const std::string& req) {
	
	hdr["Host"] = "";
	hdr["User-Agent"] = "";
	hdr["Connection"] = "";
	hdr["Content-Length"] = "";

	parse(req);
}

std::string HttpRequest::versionToStr() const {
	
	if (version == Version::v1_1) {
		return "1.1";
	}

	//TODO: what about 0.9?
	return "1.0";
}

/* TODO: it's very ugly parsing. Need to optimize it for more methods/header/...*/
int HttpRequest::parse(const std::string& req) {
	
	size_t pos = 0;

	if (req.substr(pos, 3) == "GET") {
		method = Method::GET;		
		pos += 4;
		size_t end = req.find(" ", 4);
		path = req.substr(pos, end - pos);
		pos = end + 1;
		size_t parame = path.find_first_of("?");
		if(parame < end) {
			get_query = path.substr(parame + 1);
			path = path.substr(0, parame);
		}

	}
	else if (req.substr(pos, 4) == "POST") {
		method = Method::POST;
		pos += 5;
		size_t end = req.find(" ", 5);
		path = req.substr(pos, end - pos);
		pos = end + 1;
	}
	else {
		return -1;
	}	

	if (req.substr(pos, 5) != "HTTP/")
		return -2;
	pos += 5;

	std::string ver = req.substr(pos, 3);
	if (ver == "1.0") {
		version = Version::v1_0;
	} else if (ver == "1.1") {
		version = Version::v1_1;
	} else {
		return -3;
	}

	for (auto it = hdr.begin(); it != hdr.end(); ++it) {
		
		const std::string& hdr = it->first;
		size_t pos = req.find(hdr);

		pos += hdr.size() + 2;
		size_t end = req.find("\n", pos);
		std::string value = req.substr(pos, end - pos - 1);
		
		if (value.back() == '\r') {
			
			value.erase(value.size() - 1, 1);
		}

		it->second = value;
	}

	size_t location = req.find("\r\n\r\n");
	if(location != std::string::npos && method == Method::POST)
		get_query = req.substr(location + 4);

	return 0;
}
