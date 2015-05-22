#ifndef __ERROR_REQUEST_PROCESSOR__
#define __ERROR_REQUEST_PROCESSOR__

#include "DataBuffer.h"
#include "FilePro.h"
#include "RequestProcessor.h"

class ErrorRequestProcessor : public RequestProcessor
{
public:
	ErrorRequestProcessor();

	void setLastErrorCode(int code);
	
	void process(HttpRequest& req, DataBuffer<uint8_t>& buffer) override;
	
	bool isEligible(const HttpRequest& req) const override;

private:
	int code_;
};

#endif
