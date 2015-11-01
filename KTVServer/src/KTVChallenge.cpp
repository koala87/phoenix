#include "KTVChallenge.h"
#include "json/json.h"
#include "utility/Logger.h"
#include "KTVServer.h"
using namespace yiqiding::ktv;


KTVChallenge::KTVChallenge(Server *s , uint32_t fromboxid , uint32_t toboxid , uint32_t mid):_fromboxid(fromboxid),_toboxid(toboxid),_mid(mid),_server(s)
{
	memset(&_fromscore , 0 , sizeof(_fromscore));
	memset(&_toscore , 0 , sizeof(_toscore));
	_fromscore._normal = -1;
	_toscore._normal = -1;
	
}


void KTVChallenge::process()
{
	yiqiding::MutexGuard guard(_mutex);
	switch(_status)
	{
	case INIT_TIMEOUT://û�м���
		{
			std::auto_ptr<packet::Packet> pack(ManKTVChallenge::generatorNotifyJoin(-1));
			try {
				_server->getConnectionManager()->sendToBox(_fromboxid , pack.get());
			} catch (const std::exception& err) {

			}
		}
		yiqiding::utility::Logger::get("system")->log("fromboxid: " + yiqiding::utility::toString(_fromboxid) + " no join end");
		break;
	case JOIN_TIMEOUT://˫��û�л����ϴ�,״̬��Ϊ��������֪ͨ
		try{
			_server->getDatabase()->getInfoConnector()->insertChallenge(_fromboxid , _toboxid , 0 , 0 , 0 ,0 , _mid);
		}catch(const std::exception &err){
			yiqiding::utility::Logger::get("system")->log(yiqiding::utility::toString("sql:") + err.what());
		}

		yiqiding::utility::Logger::get("system")->log("fromboxid: " + yiqiding::utility::toString(_fromboxid) + " no two upload end");
		break;
	case UPLOAD_TIMEOUT://ֻ��һ�������ϴ����֣�״̬����
		{
			int fromwin = 0;
			int towin = 0;
			uint32_t boxid;

			//ȷ��Ҫ֪ͨ�Ļ����У������ϴ����ֵġ�
			if(_fromscore._normal >= 0)
			{
				boxid = _fromboxid;
			}
			else
			{
				boxid = _toboxid;
			}


			//ȷ��ʤ��
			if(_fromscore._score > _toscore._score)
			{
				fromwin = 1;
				towin = -1;	
			}
			else if(_fromscore._score < _toscore._score)
			{
				fromwin = -1;
				towin = 1;
			}
			else
			{
				fromwin = 0;
				towin = 0;
			}

			//�����ϴ����ֵĻ�����֪ͨ����һ���ϴ���ʱ��
			std::auto_ptr<packet::Packet> pack(ManKTVChallenge::generatorNotifyScoreTimerout());
			try {
				_server->getConnectionManager()->sendToBox(boxid , pack.get());
			} catch (const std::exception& err) {
				;
			}


			//�������ݿ�
			try{
			_server->getDatabase()->getInfoConnector()->insertChallenge(_fromboxid , _toboxid , fromwin , towin , _fromscore._score , _toscore._score , _mid);
			}catch(const std::exception &err){
				yiqiding::utility::Logger::get("system")->log(yiqiding::utility::toString("sql:") + err.what());
			}
			yiqiding::utility::Logger::get("system")->log("toboxid: " + yiqiding::utility::toString(_fromboxid) + " only one upload end game");
		}
		break;
	}

	_status = END_TIMEOUT;
	
}


void KTVChallenge::addtimer(GameState status)
{
	yiqiding::MutexGuard guard(_mutex);
	switch(status)
	{
	case INIT_TIMEOUT:
		start(30 * 1000, yiqiding::time::WHEEL_ONESHOT);
		break;
	case JOIN_TIMEOUT:
		stop();
		start(10 * 60 * 1000, yiqiding::time::WHEEL_ONESHOT);
		break;
	case UPLOAD_TIMEOUT:
		stop();
		start(5 * 1000 , yiqiding::time::WHEEL_ONESHOT);
		break;
	}

	_status = status;
}

bool KTVChallenge::setfromscore(const ChallengeScore &fromscore )
{
	yiqiding::MutexGuard guard(_mutex);
	_fromscore = fromscore;
	if(_toscore._normal >= 0)
		return true;
	return false;
}

bool KTVChallenge::settoscore(const ChallengeScore &toscore)
{
	yiqiding::MutexGuard guard(_mutex);
	_toscore = toscore;
	if(_fromscore._normal >= 0)
		return true;
	return false;
}

bool ManKTVChallenge::isInternelExist(uint32_t boxid)
{
	
// ���������remove_if
/*	std::list<KTVChallenge*>::iterator bt = std::remove_if(_lstChallenge.begin() , _lstChallenge.end() , RemoveEndPred());
	yiqiding::utility::Logger::get("system")->log("isInternelExist total:" + yiqiding::utility::toString(_lstChallenge.size()));
	while(bt != _lstChallenge.end())
	{
		(*bt)->stop();
		delete *bt;
		bt = _lstChallenge.erase(bt);
	}
*/

	for each(auto k in _lstChallenge)
	{
		if(k->getStatus() == KTVChallenge::END_TIMEOUT)
		{
			k->stop();
			delete k;
		}
	}

	_lstChallenge.remove_if(RemoveEndPred());



	std::list<KTVChallenge *>::iterator it = std::find_if(_lstChallenge.begin() , _lstChallenge.end(),FindExistPred(boxid) );
	bool confirm = (it != _lstChallenge.end()); 
	return it != _lstChallenge.end();
}

bool ManKTVChallenge::isExist(uint32_t boxid)
{
	yiqiding::MutexGuard guard(_mutex);
	
	return isInternelExist(boxid);
}

int ManKTVChallenge::showAllKGame(yiqiding::net::tel::ServerSend *srv)
{
	std::ostringstream out;
	{
		yiqiding::MutexGuard guard(_mutex);
		for each(auto k in _lstChallenge){
			out <<"from:" <<k->getFromBoxid() <<" to:"<< k->getToBoxid() <<" status:"<< k->getStatus() << " mid:" <<k->getmid()<<"\r\n"; 
		}
	}

	return srv->teleSend(out.str());
}

bool ManKTVChallenge::cancelChallenge(uint32_t fromboxid )
{
	yiqiding::MutexGuard guard(_mutex);

	yiqiding::utility::Logger::get("system")->log(" total: " + yiqiding::utility::toString(_lstChallenge.size()));
	std::list<KTVChallenge *>::iterator bt = std::find_if(_lstChallenge.begin() , _lstChallenge.end() , FindExistFPred(fromboxid));

	if(bt != _lstChallenge.end())
	{
		(*bt)->stop();
		delete *bt;
		_lstChallenge.erase(bt);
		yiqiding::utility::Logger::get("system")->log(yiqiding::utility::toString(fromboxid) + " cancel a game");
		return true;
	}
	return false;
}

bool ManKTVChallenge::addChallenge(Server *s , uint32_t mid , uint32_t fromboxid , uint32_t toboxid)
{
	yiqiding::MutexGuard guard(_mutex);
	if(isInternelExist(fromboxid) || isInternelExist(toboxid))
		return false;
	KTVChallenge *challenge = new KTVChallenge(s , fromboxid , toboxid , mid);
		
	_lstChallenge.push_back(challenge);
	challenge->addtimer(KTVChallenge::INIT_TIMEOUT);
	return true;
}

bool ManKTVChallenge::joinChanllenge(uint32_t fromboxid , uint32_t toboxid)
{
	yiqiding::MutexGuard guard(_mutex);
	std::list<KTVChallenge *>::iterator it = std::find_if(_lstChallenge.begin() , _lstChallenge.end(),FindExistFTPred(fromboxid,toboxid));
	if(it != _lstChallenge.end())
	{
		if((*it)->getStatus() == KTVChallenge::INIT_TIMEOUT)
		{
			(*it)->addtimer(KTVChallenge::JOIN_TIMEOUT);
			return true;
		}
	}
	return false;
}


bool ManKTVChallenge::rejectChanllenge(uint32_t fromboxid)
{
	yiqiding::MutexGuard guard(_mutex);
	std::list<KTVChallenge *>::iterator it = std::find_if(_lstChallenge.begin() , _lstChallenge.end(),FindExistPred(fromboxid));
	if(it != _lstChallenge.end() )
	{
		if((*it)->getStatus() == KTVChallenge::INIT_TIMEOUT)
		{
			(*it)->stop();
			delete *it;
			_lstChallenge.erase(it);		
			yiqiding::utility::Logger::get("system")->log("fromboxid: " + yiqiding::utility::toString(fromboxid) + " one reject game");
			return true;
		}
		
	}
	return false;
}

bool ManKTVChallenge::uploadChanllenge(Server *s , const ChallengeScore &score , uint32_t boxid)
{
	yiqiding::MutexGuard guard(_mutex);
	std::list<KTVChallenge *>::iterator it = std::find_if(_lstChallenge.begin() , _lstChallenge.end(),FindExistPred(boxid));
	if(it != _lstChallenge.end())
	{
	
		bool bend = false;
		if(boxid == (*it)->getFromBoxid())
			bend = (*it)->setfromscore(score);
		else
			bend = (*it)->settoscore(score);
		
		//jiesu
		if(bend)
		{

			int fromwin = 0;
			int towin = 0;
			//˫��������˫���ϴ�
			if(score._normal == 0)
			{
				if(boxid == (*it)->getFromBoxid()) { fromwin = 1; towin = -1;}
				else { fromwin = -1; towin = 1;}
			}
			else
			{
				if((*it)->getfromscore()._score > (*it)->gettoscore()._score)
				{
					fromwin = 1;
					towin = -1;
				}
				else if((*it)->getfromscore()._score == (*it)->gettoscore()._score)
				{
					fromwin = 0;
					towin = 0;
				}
				else
				{
					fromwin = -1;
					towin = 1;
				}

			}
			try{
				s->getDatabase()->getInfoConnector()->insertChallenge((*it)->getFromBoxid() ,(*it)->getToBoxid() , fromwin , towin , (*it)->getfromscore()._score , (*it)->gettoscore()._score , (*it)->getmid());
			}
			catch(const std::exception &err)
			{
				yiqiding::utility::Logger::get("system")->log(yiqiding::utility::toString("sql:") + err.what());
			}

			(*it)->stop();
			delete *it;
			_lstChallenge.erase(it);


			

			yiqiding::utility::Logger::get("system")->log("boxid:" + yiqiding::utility::toString(boxid) + " normal end");
			
		}
		else if(score._normal == 1)
		{
			(*it)->addtimer(KTVChallenge::UPLOAD_TIMEOUT);	
		}

		return true;
	}

	return false;
}



int ManKTVChallenge::getPeerBoxid(uint32_t boxid)
{
	yiqiding::MutexGuard guard(_mutex);
	std::list<KTVChallenge *>::iterator it = std::find_if(_lstChallenge.begin() , _lstChallenge.end(),FindExistPred(boxid));
	if(it != _lstChallenge.end())
	{
		if(boxid == (*it)->getFromBoxid())
			return (*it)->getToBoxid();
		else
			return (*it)->getFromBoxid();
	}
	return -1;
}





packet::Packet * ManKTVChallenge::generatorNotifyJoin(int join)
{
	packet::Packet * pack = new packet::Packet(packet::KTV_REQ_BOX_OK_CHALLENGE);
	Json::Value root;
	root["join"] = join;
	std::string msg = root.toStyledString();
	pack->setPayload(msg.c_str() , msg.length());
	return pack;
}

packet::Packet * ManKTVChallenge::generatorNotifyScoreTimerout()
{
	packet::Packet * pack = new packet::Packet(packet::KTV_REQ_BOX_GAME_SOCRE);
	Json::Value root;
	root["normal"] = -1;
	root["score"] = 0;
	root["yinzhun"] = 0;
	root["wending"] = 0;
	root["biaoxianli"] = 0;
	root["jiezou"] = 0;
	
	Json::Value jiepai;
	jiepai["perfect"] = 0;
	jiepai["great"] = 0;
	jiepai["good"] = 0;
	jiepai["ok"] = 0;
	jiepai["miss"] = 0;
	root["jiepai"] = jiepai;
	
	root["jiqiao"] = 0;

	std::string msg = root.toStyledString();
	pack->setPayload(msg.c_str() , msg.length());
	return pack;
}


