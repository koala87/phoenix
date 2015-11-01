#pragma once

#include "zmq.h"
#include "zmq_utils.h"
#include <string>

namespace yiqiding{ namespace net{

	class ZMQClient{

	private:
		static void *_ctx;
		void *_s;

	public:
		bool connect(const std::string connect_to);
		bool sendmsg(const void *data , int len);
		static void initContext();
		static void releaseContext();
		ZMQClient();
		~ZMQClient();
	};


}}

