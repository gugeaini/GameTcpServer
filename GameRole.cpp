#include "GameRole.h"
#include<iostream>
#include"GameMsg.h"
#include"msg.pb.h"
#include"GameProtocol.h"
#include"GameChannel.h"

using namespace std;
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
/*向周围玩家发送自己的位置*/
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
GameRole::GameRole()
{
	szName = "GUGE";
	x = 150;
	z = 150;
}
GameRole::~GameRole()
{
}
bool GameRole::Init()
{
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
		cout << "type is " << _pMsg->enMsgType << endl;
		cout << _pMsg->pMsg->Utf8DebugString() << endl;
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

	wrold.delPlayer(this);
}

int GameRole::GetX()
{
	return (int)x;
} 

int GameRole::GetY()
{
	return (int)y;
}
