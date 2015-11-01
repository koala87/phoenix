/**
 * based winmm.lib timeGetTime timeSetEvent timer
 * @author Yuchun Zhang
 * @date 2014.05.15
 */

#pragma once
#include <cstdint>
#include <WinSock2.h>
#include <windows.h>
#include "ThreadW1Rn.h"
#include "time/wheeltimer.h"

#include <map>


namespace yiqiding { namespace time{



	class Timer: public stNodeLink
	{
	private:
		stNodeLink	*_link;
		stNodeLink  *_father;

		Timer(uint32_t millsecond , WheelTimer t , stNodeLink *father):stNodeLink(millsecond , t) , _father(father)  , _link(NULL){}
		
	public:
		Timer():stNodeLink(0 , WHEEL_ONESHOT),_link(NULL){}
		virtual ~Timer(){ stop();  }

		bool start(uint32_t millsecond , WheelTimer t);
		void stop();


		virtual void process(){
			if(_father != NULL)
			_father->process();
		}

	};

}}