#include <sstream>
#include <sys/types.h>
#include <unistd.h>
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
	if(req.method == HttpRequest::Method::GET && req.get_query.size() == 0) {
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

			const std::string path = std::string("../html") + req.path;
			FS::File file(path, path);

			if (file.isRegular()) {
				fileContent(req, file, buffer);
			} else if (file.isDir()) {
				dirContent(req, file, buffer);
			}
		}
	}
	else {
		int cgi_output[2];
		int cgi_input[2];
		pid_t pid;
		int pidstatus;
		if(pipe(cgi_output) < 0 || pipe(cgi_input) < 0 || (pid = fork()) < 0)
			THROW_SYSTEM_ERROR();
		if (pid == 0) {
			// 把 STDOUT 重定向到 cgi_output 的写入端
			dup2(cgi_output[1], 1);
			// 把 STDIN 重定向到 cgi_input 的读取端
			dup2(cgi_input[0], 0);
			// 关闭 cgi_input 的写入端 和 cgi_output 的读取端
			close(cgi_output[0]);
			close(cgi_input[1]);
			
		}

	}
	
	
}

bool FSRequestProcessor::isEligible(const HttpRequest& req) const {
	
	(void)req;
	return true;
}
