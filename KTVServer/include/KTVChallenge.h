#pragma once
#include <stdint.h>
#include <list>
#include "Thread.h"
#include "time/Timer.h"
#include "Packet.h"
#include "net/TelNet.h"


namespace yiqiding{ namespace ktv{

	
	class Server;


	struct ScoreJiePai
	{
		int _perfect;
		int _great;
		int _good;
		int _ok;
		int _miss;
	};

	
	struct ChallengeScore
	{
		int _normal;		//1 0 -1
		int _score;
		int _yinzhun;
		int _wending;
		int _biaoxianli;
		int _jiezou;
		ScoreJiePai _jiepai;
		int _jiqiao;

	};
	
	
	
	class KTVChallenge:public yiqiding::time::Timer
	{

	public:
		enum GameState{
			INIT_TIMEOUT,
			JOIN_TIMEOUT,
			UPLOAD_TIMEOUT,
			END_TIMEOUT,
		};

	private:
		uint32_t						_fromboxid;
		ChallengeScore					_fromscore;
		uint32_t						_toboxid;
		ChallengeScore					_toscore;
		uint32_t						_mid;
		GameState						_status;
		yiqiding::Mutex					_mutex;
		yiqiding::ktv::Server			*_server;

	public:
		KTVChallenge(yiqiding::ktv::Server *s , uint32_t fromboxid , uint32_t toboxid , uint32_t mid);
		uint32_t getFromBoxid()				{return _fromboxid;}
		uint32_t getToBoxid()				{return _toboxid;}
		GameState getStatus()				{return _status;}
		virtual void process();
		void addtimer(GameState status);
		
		//�����Ƿ����,������ϴ����֡�
		const ChallengeScore & getfromscore() { return _fromscore;}
		const ChallengeScore & gettoscore()	  { return _toscore;}
		bool setfromscore(const ChallengeScore &fromscore ); 
		bool settoscore(const ChallengeScore &toscore);		
		uint32_t	getmid()				{ return _mid;}
};


	class FindExistPred
	{
		uint32_t _boxid;
	public:
		FindExistPred(uint32_t boxid):_boxid(boxid){}

		//id �� ������ �� �ѽ��ܵĽ����� �С�
		bool operator()(KTVChallenge *challenge)
		{
			return (challenge->getFromBoxid() == _boxid || (challenge->getToBoxid() == _boxid && challenge->getStatus() != KTVChallenge::INIT_TIMEOUT));
		}
	};

	class FindExistFPred
	{
		uint32_t _fromboxid;
	public:
		FindExistFPred(uint32_t boxid):_fromboxid(boxid){}
		bool operator()(KTVChallenge *challenge)
		{
			return (challenge->getFromBoxid() == _fromboxid && challenge->getStatus() == KTVChallenge::INIT_TIMEOUT);
		}
	};


	class FindExistFTPred
	{
		uint32_t _toboxid;
		uint32_t _fromboxid;
	public:
		FindExistFTPred( uint32_t fromboxid,uint32_t toboxid):_toboxid(toboxid),_fromboxid(fromboxid){}
		bool operator()(KTVChallenge *challenge)
		{
			return challenge->getFromBoxid() == _fromboxid && challenge->getToBoxid() == _toboxid;
		}

	};

	class RemoveEndPred
	{
	public:
		bool operator()(KTVChallenge * challenge)
		{
			return challenge->getStatus() == KTVChallenge::END_TIMEOUT;
		}

	};	




	class ManKTVChallenge
	{
	private:
		std::list<KTVChallenge *>		_lstChallenge;
		yiqiding::Mutex					_mutex;
		
		bool isInternelExist(uint32_t boxid);

	public:
		//���
		bool addChallenge(yiqiding::ktv::Server *s ,uint32_t mid , uint32_t fromboxid , uint32_t toboxid);
		//ȡ��
		bool cancelChallenge(uint32_t fromboxid );

		//�Ƿ����
		bool isExist(uint32_t boxid);
		//����
		bool joinChanllenge(uint32_t fromboxid , uint32_t toboxid);
		//�ܾ�
		bool rejectChanllenge(uint32_t fromboxid);
		//�ϴ�
		bool uploadChanllenge(Server *s , const ChallengeScore &score , uint32_t boxid);
		//��öԶ˵�boxid
		int getPeerBoxid(uint32_t boxid);
		//��ʾ
		int showAllKGame(yiqiding::net::tel::ServerSend *srv);
		
		static packet::Packet * generatorNotifyJoin(int join);
		static packet::Packet * generatorNotifyScoreTimerout();
		
};


}}
