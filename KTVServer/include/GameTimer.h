/*
	K�����ʵ��
	2015-6-12 ��ʱʵ��զ��ô���ӡ�������Щ�ԲС��µļ�GameTimer2.
*/

#include "time/Timer.h"
#include "Thread.h"
#include "ThreadW1Rn.h"
#include "PacketProcessor.h"
#include "KTVServer.h"
#include <string>
#include <set>
#include <map>
#include <vector>
#pragma  once

namespace yiqiding{ namespace ktv{
	class MidName;
	class BoxScore;
}}



namespace yiqiding{ namespace ktv{ namespace game{



enum EventState
{
	GAME_INIT = 0,				//��ʼ״̬��
	GAME_NOTIFY_TIMER = 1,		//��ʱ��֪ͨ״̬��
	GAME_JOIN_TIMER = 2,		//��ʱ������״̬	
	GAME_START_TIMER = 3,		//��ʱ����ʼ״̬
	GAME_SCORE_TIMER = 4,		//��ʱ����һ������״̬
	GAME_END_TIMER  = 5,		//��ʱ������״̬
	GAME_END	=	6,			//����
};



const uint32_t TIME_START_AFTER_NOW = 5 * 60;			//��ʼʱ����ڵ�ǰʱ��5����
const uint32_t TIME_BEFORE_START = 1 * 60;				//��ʼʱ��ǰ1����
const uint32_t TIME_END_AFTER_GAME = 15;				//�ϴ����ֺ�15��
const uint32_t TIME_PER_SING_GAME_TIME = 10 * 60;		//ÿ�׸��ʱ��Ϊ10����


class GameTimer:public yiqiding::time::Timer
{
private:
	yiqiding::Mutex							_mutex;

	EventState								_state;
	yiqiding::ktv::Server*					_server;	
		

	uint32_t								_kid;					//k�������id
	bool									_type;					//���� true ����
	std::vector<MidName>					_mids;					//k������ĸ���id��
	std::set<uint32_t>						_notifytimes;			//֪ͨʱ����,����
	uint32_t								_begin;					//��ʼʱ��
	std::string								_notifymessage;			//����
	std::string								_notifytitle;			//����


	std::set<uint32_t> _joins;						//�μ��б�
	std::vector<BoxScore> _scores;					//����
	friend class SingerGame;

public:
	GameTimer(yiqiding::ktv::Server * s , uint32_t kid ):_server(s) , _kid(kid){ }
	~GameTimer(){}

	
	
	void beginSingeGame(bool type ,
		const std::vector<MidName> &mids ,
		const std::set<uint32_t> &notifytimes ,
		const std::string &notifymessage ,
		const std::string &notifytitle, 
		uint32_t begin );


	bool start(uint32_t second , yiqiding::time::WheelTimer t) { return yiqiding::time::Timer::start(second * 1000 , t);}


	static void sendOpenSingGame(yiqiding::ktv::packet::Processor *pro , packet::Packet *pac , ktv::KTVConnection *conn , uint32_t kid , bool confirm);
	static void sendModifySingGame(yiqiding::ktv::packet::Processor *pro , packet::Packet *pac , ktv::KTVConnection *conn , bool confirm);
	void sendNotify(bool flag );
	bool sendJoin( );
	void sendStart();
	
	void sendScoreError(int state);

	
	void sendScoreNoBodyJoin(){ sendScoreError(1);}
	void sendScoreNoBodyUpLoad(){sendScoreError(2);}
	void sendScoreNoBodyOnline(){ sendScoreError(3); }
	void sendScoreNormal();

	//Getter
	uint32_t getKid() const { return _kid; }
	const std::vector<MidName> & getMids() const{ return _mids;}
	bool getType() const {return _type;}
	const std::set<uint32_t> & getNotifyTimes() const{ return _notifytimes;}
	const std::string & getNotifyTitle() const { return _notifytitle;}
	const std::string & getNotifyMessage() const { return _notifymessage;}
	uint32_t getStartTime() const {return _begin;}
	EventState	getState() const { return _state;}

	//@thread safe
	void process();
	void joinSingeGame(uint32_t boxid);	
	void scoreSingeGame(uint32_t boxid , uint32_t score);
	bool modifySingeGame(const std::vector<MidName> &mids ,
		const std::set<uint32_t> &notifytimes ,
		const std::string &notifymessage ,
		const std::string &notifytitle, 
		uint32_t begin );
	bool checkTime(uint32_t starttime , int midNum);
	

	bool canDel();
	bool canJoin();
	bool canUpLoad();
	bool canModify();


	 
};


class SingerGame
{
private:
	// single
	static SingerGame*				_instance;	
	static yiqiding::MutexWR		_wr;
	static uint32_t					_kid;
	std::map<uint32_t , GameTimer *>  _mapTimer;
	
	~SingerGame();
	 void GCGameTimer();
	
public: 

	//@thread safe

	// single 
	static SingerGame* getInstace();
	static void unLoad();
	static yiqiding::MutexWR & getMutexWR () {return _wr;}
	static uint32_t getNewKid();

	// file i/o
	void load(yiqiding::ktv::Server *s ,const std::string &path);
	void save(const std::string &path);

	//  operator
	void add(GameTimer *gt);
	bool del(int kid);

	bool checkTime(uint32_t startTime , int midNum);

	bool checkTimeExcept(uint32_t startTime , int midNum , int exceptKid);

	//call start_get @non thread , must use with getMutexWR
	 GameTimer * get(int kid);

	
	
	 std::map<uint32_t , GameTimer *> & getTimers() { return _mapTimer;}
};

}}}
