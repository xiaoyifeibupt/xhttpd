#include <sstream>
#include "FS.h"
#include "FSRequestProcessor.h"

FSRequestProcessor::FSRequestProcessor() {

}

void FSRequestProcessor::fileContent(HttpRequest& req,
				FS::File& file, Buffer<uint8_t>& buffer) {
		
	Buffer<char> content;

	file.content(content);

	makeHttpResponse(req, content.data(), content.size(), buffer);
}

void FSRequestProcessor::dirContent(HttpRequest& req,
				FS::File& file, Buffer<uint8_t>& buffer) {
		
	FS::Directory dir(file.path());

	std::stringstream ss;
	FS::FilePtr f;

	ss << "<table>";

	while ((f = dir.nextFile())) {
		
		ss << "<tr><td><a href='" << f->name() <<
			(f->isDir() ? "/" : "") << "'>" << f->name() <<
			"</a></td><td>" << f->size() << "</td></tr>";
	}
	ss << "</table>";

	std::string content = ss.str();

	makeHttpResponse(req, content.data(), content.size(), buffer);
}

void FSRequestProcessor::process(HttpRequest& req, Buffer<uint8_t>& buffer) {
	if(req.path =="/" || req.path =="/index.html") {

		const std::string path = std::string("../html/index.html");

		FS::File file(path, path);
		fileContent(req, file, buffer);
		file.closefile();				

	}
	else if(req.path =="/favicon.ico") {

		const std::string path = std::string("../html/favicon.ico");
		FS::File file(path, path);

		fileContent(req, file, buffer);
	}
	else {

		const std::string path = std::string(".") + req.path;
		FS::File file(path, path);

		if (file.isRegular()) {
			fileContent(req, file, buffer);
		} else if (file.isDir()) {
			dirContent(req, file, buffer);
		}

	}
	
	
}

bool FSRequestProcessor::isEligible(const HttpRequest& req) const {
	
	(void)req;
	return true;
}
