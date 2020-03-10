#pragma once
#include <zinx.h>
#include"AOIWrold.h"
#include"GameMsg.h"
class GameProtocol;
class GameRole :
	public Irole, public Player
{
	float x = 0;	//长
	float y = 0;	//高
	float z = 0;	//宽
	float v = 0;	//视野
	int iPid = 0;
	std::string szName;
	/*向自己发送ID和名字*/
	GameMsg* CreateIDNameLogin();
	/*向自己发送周围玩家的位置*/
	GameMsg* CreateSrdPlayers();
	/*向周围玩家发送自己的位置*/
	GameMsg* CreateSelfPosion();
	/*向周围玩家发送下线消息*/
	GameMsg* CreateIDNameLogoff();
public:
	GameRole();
	virtual ~GameRole();

	// 通过 Irole 继承
	virtual bool Init() override;
	virtual UserData* ProcMsg(UserData& _poUserData) override;
	virtual void Fini() override;

	/*绑定的协议层对象*/
	GameProtocol* m_Protocol=NULL;

	// 通过 Player 继承
	virtual int GetX() override;
	virtual int GetY() override;
};

