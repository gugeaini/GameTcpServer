#include "GameRole.h"
#include<iostream>
#include"GameMsg.h"
#include"msg.pb.h"
#include"GameProtocol.h"
#include"GameChannel.h"
#include<algorithm>
#include"RandomName.h"
#include"ZinxTimer.h"

RandomName random_name;
using namespace std;
/*定时器推出类*/
class ExitTimer :
	public TimerTask
{
	// 通过 TimerTask 继承
	virtual void Proc() override
	{
		ZinxKernel::Zinx_Exit();
	}
	virtual int GetTimeSec() override
	{
		/*秒数*/
		return 20;
	}
};
static ExitTimer g_exit_Timer;	//全局定时退出单例



/*创建游戏地图世界对象*/
static AOIWrold wrold(0, 600, 0, 600, 30, 30);
/*向自己发送ID和名字*/
GameMsg* GameRole::CreateIDNameLogin()
{
	pb::SyncPid* pPid = new pb::SyncPid();
	pPid->set_pid(iPid);
	pPid->set_username(szName);

	GameMsg* pMsg = new GameMsg(GameMsg::MSG_TYPE_LOGIN_ID_NAME, pPid);

	return pMsg;
}
/*向自己发送周围玩家的位置*/
GameMsg* GameRole::CreateSrdPlayers()
{
	pb::SyncPlayers* pMsg = new pb::SyncPlayers();

	/*获取周围玩家链*/
	auto srdList = wrold.GetsrdPlayers(this);
	/*遍历链把每位玩家信息组装*/
	for (auto single : srdList)
	{
		auto pPlayer = pMsg->add_ps();
		
		/*每位玩家的id 和 名字*/
		auto pRole = dynamic_cast<GameRole*>(single);
		pPlayer->set_pid(pRole->iPid);
		pPlayer->set_username(pRole->szName);
		
		/*每位玩家的坐标*/
		auto pPostion = pPlayer->mutable_p();
		pPostion->set_x(pRole->x);
		pPostion->set_y(pRole->y);
		pPostion->set_z(pRole->z);
		pPostion->set_v(pRole->v);
	}
	GameMsg* pRet = new GameMsg(GameMsg::MSG_TYPE_SRD_POSITION, pMsg);

	return pRet;
}
/*向周围玩家发送自己的初始位置*/
GameMsg* GameRole::CreateSelfPosion()
{
	pb::BroadCast* pMsg = new pb::BroadCast();
	pMsg->set_pid(iPid);
	pMsg->set_username(szName);
	/* 1 聊天内容	2 初始位置	3 新位置*/
	pMsg->set_tp(2);
	auto pPosition = pMsg->mutable_p();
	pPosition->set_x(x);
	pPosition->set_y(y);
	pPosition->set_z(z);
	pPosition->set_v(v);

	GameMsg* pRet = new GameMsg(GameMsg::MSG_TYPE_BROADCAST, pMsg);

	return pRet;
}
/*向周围玩家发送下线消息*/
GameMsg* GameRole::CreateIDNameLogoff()
{
	pb::SyncPid* pPid = new pb::SyncPid();
	pPid->set_pid(iPid);
	pPid->set_username(szName);

	GameMsg* pMsg = new GameMsg(GameMsg::MSG_TYPE_LOGOFF_ID_NAME, pPid);

	return pMsg;

}
/*向所有人发送聊天消息*/
GameMsg* GameRole::CreateTalkBroadCast(string _content)
{
	pb::BroadCast* pMsg = new pb::BroadCast();
	pMsg->set_pid(iPid);
	pMsg->set_username(szName);
	pMsg->set_tp(1);
	pMsg->set_content(_content);

	GameMsg* pRet = new GameMsg(GameMsg::MSG_TYPE_BROADCAST, pMsg);

	return pRet;
}

/*1.处理聊天消息*/
void GameRole::ProcTalkMsg(string content)
{
	auto role_list = ZinxKernel::Zinx_GetAllRole();
	/*发送给所有玩家*/
	for (auto pRole : role_list)
	{
		/*组装消息内容*/
		auto pMsg = CreateTalkBroadCast(content);
		auto pGameRole = dynamic_cast<GameRole*>(pRole);
		/*发送出去*/
		ZinxKernel::Zinx_SendOut(*pMsg, *(pGameRole->m_Protocol));
	}
}

/*2.处理新位置*/
void GameRole::ProcMoveMsg(int _x, int _y, int _z, int _v)
{
	/*1.跨网格处理*/
	/*获取原来周围玩家链*/
	auto oldList = wrold.GetsrdPlayers(this);
	/*摘除旧格子	更新当前坐标	添加进新格子，后获取新周围玩家链*/
	wrold.delPlayer(this);
	x = _x;
	y = _y;
	z = _z;
	v = _v;
	wrold.AddPlayer(this);
	auto newList = wrold.GetsrdPlayers(this);
	/*遍历新格子，若有元素不属于旧格子，视野展现*/
	for (auto single : newList)
	{
		/*算法库中 给一个容器起始和结束范围，查找value*/
		if (oldList.end() == find(oldList.begin(), oldList.end(), single))
		{
			/*没找到，添加视野*/
			ViewAppear(dynamic_cast<GameRole*>(single));
		}
	}
	/*遍历旧格子，若有元素不属于新格子，视野消失*/
	for (auto single : oldList)
	{
		/*算法库中 给一个容器起始和结束范围，查找value*/
		if (newList.end() == find(newList.begin(), newList.end(), single))
		{
			/*没找到，视野消失*/
			ViewLost(dynamic_cast<GameRole*>(single));
		}
	}

	/*2.广播新位置给周围玩家*/

	/*向周围玩家发送自己的位置*/
	auto srdList = wrold.GetsrdPlayers(this);
	for (auto single : srdList)
	{
		/*组装数据*/
		pb::BroadCast* pMsg = new pb::BroadCast();
		auto pPos = pMsg->mutable_p();
		pPos->set_x(_x);
		pPos->set_y(_y);
		pPos->set_z(_z);
		pPos->set_v(_v);
		pMsg->set_pid(iPid);
		pMsg->set_username(szName);
		pMsg->set_tp(4);

		/*发送给每位周围玩家*/
		GameRole* pRole = dynamic_cast<GameRole*>(single);
		ZinxKernel::Zinx_SendOut(*(new GameMsg(GameMsg::MSG_TYPE_BROADCAST,pMsg)), *(pRole->m_Protocol));
	}
	
	

}
/*2.1视野出现*/
void GameRole::ViewAppear(GameRole* _pRole)
{
	/*向自己发参数的200消息*/
	auto pMsg = _pRole->CreateSelfPosion();
	ZinxKernel::Zinx_SendOut(*pMsg, *m_Protocol);

	/*向参数玩家发自己的200消息*/
	pMsg = CreateSelfPosion();
	ZinxKernel::Zinx_SendOut(*pMsg, *(_pRole->m_Protocol));
}
/*2.2视野消失*/
void GameRole::ViewLost(GameRole* _pRole)
{
	/*向自己发参数的201消息*/
	auto pMsg = _pRole->CreateIDNameLogoff();
	ZinxKernel::Zinx_SendOut(*pMsg, *m_Protocol);

	/*向参数玩家发自己的201消息*/
	pMsg = CreateIDNameLogoff();
	ZinxKernel::Zinx_SendOut(*pMsg, *(_pRole->m_Protocol));
}

static default_random_engine random_engine(time(NULL));

GameRole::GameRole()
{
	szName = random_name.GetName();
	x = 150 + random_engine() % 50;
	z = 150 + random_engine() % 50;
	
}
GameRole::~GameRole()
{
	random_name.Release(szName);
}
bool GameRole::Init()
{
	/*当前玩家总数==0*/
	if (ZinxKernel::Zinx_GetAllRole().size() <= 0)
	{
		TimerMngHandler::Getinstance().DelTask(&g_exit_Timer);
	}

	int bRet = false;
	/*获取id*/
	iPid = m_Protocol->m_Channel->GetFd();
	/*添加自己到世界*/
	bRet = wrold.AddPlayer(this);

	if (true == bRet)
	{
		/*向自己发送ID和名字*/
		auto pMsg = CreateIDNameLogin();
		ZinxKernel::Zinx_SendOut(*pMsg, *m_Protocol);
		/*向自己发送周围玩家的位置*/
		pMsg = CreateSrdPlayers();
		ZinxKernel::Zinx_SendOut(*pMsg, *m_Protocol);
		/*向周围玩家发送自己的位置*/
		auto srdList = wrold.GetsrdPlayers(this);
		for (auto single : srdList)
		{
			pMsg = CreateSelfPosion();
			auto pRole = dynamic_cast<GameRole*>(single);
			ZinxKernel::Zinx_SendOut(*pMsg, *(pRole->m_Protocol));
		}
	}

	return bRet;
}

UserData* GameRole::ProcMsg(UserData& _poUserData)
{
	GET_REF2DATA(MultiMsg, input, _poUserData);

	for (auto _pMsg : input.m_Msgs)
	{

//测试代码 查看客户端消息类型和内容
#if 1
		cout << "type is " << _pMsg->enMsgType << endl;
		cout << _pMsg->pMsg->Utf8DebugString() << endl;
#endif // 0
		auto NewPos = dynamic_cast<pb::Position*>(_pMsg->pMsg);
		switch (_pMsg->enMsgType)
		{
		case GameMsg::MSG_TYPE_CHAT_CONTENT:
			ProcTalkMsg(dynamic_cast<pb::Talk*>(_pMsg->pMsg)->content());
			break;
		case GameMsg::MSG_TYPE_NEW_POSITION:
			ProcMoveMsg(NewPos->x(), NewPos->y(), NewPos->z(), NewPos->v());
			break;
		default:
			break;
		}

	}


	return nullptr;
}

void GameRole::Fini()
{
	/*向周围玩家发送下线消息*/
	auto srdList = wrold.GetsrdPlayers(this);
	for (auto single : srdList)
	{
		auto pMsg = CreateIDNameLogoff();
		auto pRole = dynamic_cast<GameRole*>(single);
		ZinxKernel::Zinx_SendOut(*pMsg, *(pRole->m_Protocol));
	}
	/*在游戏世界中摘除玩家*/
	wrold.delPlayer(this);

	/*退出服务器程序*/
	auto pRoleList = ZinxKernel::Zinx_GetAllRole();
	/*当前玩家总数==1*/
	if (pRoleList.size() <= 1)
	{
		/*启动定时器*/
		TimerMngHandler::Getinstance().AddTask(&g_exit_Timer);
	}
}

int GameRole::GetX()
{
	return (int)x;
} 

int GameRole::GetY()
{
	return (int)y;
}
