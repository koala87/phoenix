/**
 * based winmm.lib timeGetTime timeSetEvent timer
 * @author Yuchun Zhang
 * @date 2014.05.15
 */

#include "time/Timer.h"
#include "utility/Logger.h"
#include "utility/Utility.h"
#include "time/wheeltimer.h"


using namespace yiqiding::time;
using namespace yiqiding::utility;

bool Timer::start(uint32_t millsecond , WheelTimer t )
{
	if(_link != NULL)
		stop();
	_link = new Timer(millsecond , t , this);
	CTimerManager::getInstance()->add(_link);
	return true;
}




void Timer::stop()
{
	if(_link)
	{
		CTimerManager::getInstance()->del(_link);
		_link = NULL;
	}
}
