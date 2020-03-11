#pragma once
#include <zinx.h>
#include"AOIWrold.h"
#include"GameMsg.h"

/*单局游戏所以玩家名称临时文件*/
constexpr auto GAMEWROLD_FILENAME = "/tmp/gg_wroldGame_name";

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
	/*向所有人发送聊天消息*/
	GameMsg* CreateTalkBroadCast(string _content);

	/*1.处理聊天消息*/
	void ProcTalkMsg(string content);
	
	/*2.处理新位置*/
	void ProcMoveMsg(int _x, int _y, int _z, int _v);
	/*2.1视野出现*/
	void ViewAppear(GameRole* _pRole);
	/*2.2视野消失*/
	void ViewLost(GameRole* _pRole);
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

