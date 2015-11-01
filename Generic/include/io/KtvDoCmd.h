#pragma once

#include <string>
#include "time/Timer.h"
#include "utility/Utility.h"
#include "assert.h"
#include <string>
#include "sql/MySQL.h"
#include <vector>

namespace yiqiding{ namespace io{


	class KTVDoCmd
	{
	public:
	
static bool doCmd(const std::string &cmd , const std::string &mode , int &code);
		
	};



	

	struct TimerofRound
	{
		//日//周//月
		enum RoundTimerType{
			RoundDay,
			RoundWeek,
			RoundMonth,
		};

		RoundTimerType type;

		int hour;
		int min;
		int sec;

		union Index{
			int indexOfWeek;//[0,6]
			int indexofMonth;//[-31,30] 
		};
		Index index;
		static TimerofRound makeTimebyday(int hour , int min , int sec);
																		 /* 0 - */
		static TimerofRound makeTimebyweek(int hour , int min , int sec , int indexofWeek);
		static TimerofRound makeTimebyMonth(int hour , int min , int sec , int indexofMonth);
	};

	



	//定时任务.周期.时间.
	template<class Pr>
	class TimerPlan:private yiqiding::time::Timer{
	public:
		TimerPlan(const TimerofRound &tr ):_tr(tr){}
		void start(){ Timer::start(calNextTime()*1000 , yiqiding::time::WHEEL_ONESHOT);  }
		void process() {  if(_ppr!= NULL) (*_ppr)(); else _pr(); start();}
		void setPt(Pr *pr) { _ppr = pr;}
	private:
		Pr				_pr;
		Pr				*_ppr;
		TimerofRound	_tr;
	protected:
		uint32_t	calNextTime(){

					   TimerofRound tr(_tr);
		if(tr.type == TimerofRound::RoundDay)
		{
			#define SECONDOFDAY	(3600*24) 
			TIME_ZONE_INFORMATION out;
			GetTimeZoneInformation(&out);
			uint32_t now = ::time(NULL);
			now += out.Bias * -60;
			uint32_t nowSecond = now % SECONDOFDAY;
			uint32_t setSecond = tr.hour * 3600 + tr.min * 60 + tr.sec;
			if(setSecond > nowSecond)
				return setSecond - nowSecond;
			else
				return SECONDOFDAY -(nowSecond - setSecond);
		}
		else if (tr.type == TimerofRound::RoundWeek)
		{
			#define SECONDOFWEEK (3600*24*7)
			#define SECONDEOF4	 (3600*24*4)
			TIME_ZONE_INFORMATION out;
			GetTimeZoneInformation(&out);
			uint32_t now = ::time(NULL);
			now += SECONDEOF4;
			now += out.Bias * -60;
			uint32_t nowSecond = now % SECONDOFWEEK;
			uint32_t setSecond = tr.index.indexOfWeek * 24 * 3600 + tr.hour * 3600 + tr.min * 60 + tr.sec;
			if(setSecond > nowSecond)
				return setSecond - nowSecond;
			else
				return SECONDOFWEEK - (nowSecond - setSecond);
		}
		else{
			time_t now = ::time(NULL);
			tm *tm0 = localtime(&now);

			int index = tr.index.indexofMonth;	
			assert(index <= 27);
			int days = yiqiding::utility::getdayOfMonth(tm0->tm_year + 1900 , tm0->tm_mon + 1);
			if(tr.index.indexofMonth < 0)
				index = days + tr.index.indexofMonth;
			assert(index >= 0);
			int month = tm0->tm_mon;
			int year = tm0->tm_year;
			while(true)
			{
				tm tm_tmp;
				memset(&tm_tmp , 0 , sizeof(tm_tmp));
				tm_tmp.tm_hour = tr.hour;
				tm_tmp.tm_mday = index + 1;
				tm_tmp.tm_mon = month;
				tm_tmp.tm_year = year;
				tm_tmp.tm_min = tr.min;
				tm_tmp.tm_sec = tr.sec;

				time_t setSecond = mktime(&tm_tmp);
				if(setSecond > now)
					return setSecond - now;
			
				if(month == 11)
				{
					month = 0;
					++year;
				}
				else
				{
					++month;
				}
			}
	}
}	  
	};




	


	class MysqlDataBackup{

		struct SQLInfo
		{
			std::string username;
			std::string pwd;
			std::string dbname;
			std::string hostname;
			int port;
		};
		SQLInfo			_master;
		std::vector<SQLInfo> _slave; 
	public:
		void operator ()();
		void setMasterInfo(const std::string &hostname = "localhost" ,const std::string &usrname = "yiqiding" , const std::string &pwd  = "ktv_dev",
			const std::string &dbname = "yiqiding_ktv"  , int port = 3306);
		void addSlaveInfo(const std::string &hostname , const std::string &usrname = "yiqiding" , const std::string &pwd = "ktv_dev" ,
			const std::string &dbname = "yiqiding_ktv", int port = 3306);
	};

}}