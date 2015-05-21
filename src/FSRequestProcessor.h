#ifndef __FS_REQUEST_PROCESSOR_H__
#define __FS_REQUEST_PROCESSOR_H__

#include "DataBuffer.h"
#include "FS.h"
#include "RequestProcessor.h"

class FSRequestProcessor : public RequestProcessor
{
public:
	FSRequestProcessor();

	void process(HttpRequest& req, DataBuffer<uint8_t>& buffer) override;
	bool isEligible(const HttpRequest& req) const override;

private:
	void fileContent(HttpRequest& req, FS::File& file, DataBuffer<uint8_t>& buffer);
	void dirContent(HttpRequest& req, FS::File& file, DataBuffer<uint8_t>& buffer);

};

#endif
