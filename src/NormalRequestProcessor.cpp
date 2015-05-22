#include <sstream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include "FilePro.h"
#include "NormalRequestProcessor.h"

NormalRequestProcessor::NormalRequestProcessor() {

}

void NormalRequestProcessor::fileContent(HttpRequest& req,
				FilePro::File& file, DataBuffer<uint8_t>& buffer) {
		
	DataBuffer<char> content;

	file.content(content);

	makeHttpResponse(req, content.data(), content.size(), buffer);
}

void NormalRequestProcessor::dirContent(HttpRequest& req,
				FilePro::File& file, DataBuffer<uint8_t>& buffer) {
		
	FilePro::Directory dir(file.path());

	std::stringstream ss;
	FilePro::FilePtr f;

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

void NormalRequestProcessor::process(HttpRequest& req, DataBuffer<uint8_t>& buffer) {
	if(req.method == HttpRequest::Method::GET && req.get_query.size() == 0) {
		if(req.path =="/" || req.path =="/index.html") {

			const std::string path = std::string("../html/index.html");

			FilePro::File file(path, path);
			fileContent(req, file, buffer);
			file.closefile();				

		}
		else if(req.path =="/favicon.ico") {

			const std::string path = std::string("../html/favicon.ico");
			FilePro::File file(path, path);

			fileContent(req, file, buffer);

		}
		else {

			const std::string path = std::string("../html") + req.path;
			FilePro::File file(path, path);

			if (file.isRegular()) {
				fileContent(req, file, buffer);
			} else if (file.isDir()) {
				dirContent(req, file, buffer);
			}
		}
	}
	else {
		DataBuffer<char> conbuf;
		int cgi_output[2];
		int cgi_input[2];
		pid_t pid;
		int pidstatus;
		if(pipe(cgi_output) < 0 || pipe(cgi_input) < 0) {
			THROW_SYSTEM_ERROR();
		}
		if ((pid = fork()) < 0){
			THROW_SYSTEM_ERROR();
		}
		if (pid == 0) {
			// 把 STDOUT 重定向到 cgi_output 的写入端
			dup2(cgi_output[1], 1);
			// 把 STDIN 重定向到 cgi_input 的读取端
			dup2(cgi_input[0], 0);
			// 关闭 cgi_input 的写入端 和 cgi_output 的读取端
			close(cgi_output[0]);
			close(cgi_input[1]);
			std::string meth_env = "REQUEST_METHOD=";
			if(req.method == HttpRequest::Method::GET) {
				std::string query_env = "QUERY_STRING=" + req.get_query;
				char *cquery_env = new char[query_env.size()];
				strcpy(cquery_env, query_env.c_str());
				putenv(cquery_env);
				meth_env += "GET";
			}
			else {
				std::string length_env = "CONTENT_LENGTH=" + req.hdr["Content-Length"];
				char *clength_env = new char[length_env.size()];
				strcpy(clength_env, length_env.c_str());
				putenv(clength_env);
				meth_env += "POST";
			}
			char *cmeth_env = new char[meth_env.size()];
			strcpy(cmeth_env, meth_env.c_str());
			putenv(cmeth_env);
			execl(req.path.c_str(), req.path.c_str(), NULL);
			exit(0);

		}
		else {
			// 关闭 cgi_input 的读取端 和 cgi_output 的写入端 
			close(cgi_output[1]);
			close(cgi_input[0]);
			if(req.method == HttpRequest::Method::POST) {
				char *postquery = new char[req.get_query.size()];
				strcpy(postquery, req.get_query.c_str());
				int postquerySize = atoi(req.hdr["Content-Length"].c_str());
				size_t wrnum = write(cgi_input[1], postquery, postquerySize);
			}
			char *c;

			while (read(cgi_output[0], c, 1) > 0) {
				if (c != NULL){
					conbuf.append(c, 1);
				}
				c = NULL;					
			}

			// 关闭管道
			close(cgi_output[0]);
			close(cgi_input[1]);
			// 等待子进程
			waitpid(pid, &pidstatus, 0);
		}

		makeHttpResponse(req, conbuf.data(), conbuf.size(), buffer);
	}
	
	
}

bool NormalRequestProcessor::isEligible(const HttpRequest& req) const {
	
	(void)req;
	return true;
}
