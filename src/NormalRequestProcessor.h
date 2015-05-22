#ifndef __NORMAL_REQUEST_PROCESSOR_H__
#define __NORMAL_REQUEST_PROCESSOR_H__

#include "DataBuffer.h"
#include "FilePro.h"
#include "RequestProcessor.h"

class NormalRequestProcessor : public RequestProcessor
{
public:
	NormalRequestProcessor();

	void process(HttpRequest& req, DataBuffer<uint8_t>& buffer) override;
	bool isEligible(const HttpRequest& req) const override;

private:
	void fileContent(HttpRequest& req, FilePro::File& file, DataBuffer<uint8_t>& buffer);
	void dirContent(HttpRequest& req, FilePro::File& file, DataBuffer<uint8_t>& buffer);

};

#endif
