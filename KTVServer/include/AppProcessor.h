/**
 * App Request Processor
 * @author Yuchun Zhang
 * @date 2014.03.10
 */
#pragma once

#include "PacketProcessor.h"

namespace yiqiding { namespace ktv {
	class AppProcessor : public packet::Processor {

	protected:

		inline bool isAppIDValid(uint32_t box_id) { return (getConnection()->getAppID() == _pac->getDeviceID()) ? true : false; }
	public:
		AppProcessor(Server* server, AppConnection* conn, Packet* pac , const std::string &ip , int port);
		virtual ~AppProcessor()	{};
		virtual void onReceivePacket();
		virtual std::string getDir() { return "./app/";}
		
		void processScrollInfo();
		void processTurnMsgToBox();
		void processTurnMsgToBox2();
		void processReqUrl();
		void processOtherStatus();

		//port
		void processPortInfo();

		/*void processCheckTempCode();*/
		inline AppConnection*	getConnection()	{ return (AppConnection*)_conn; }
	};
}
}