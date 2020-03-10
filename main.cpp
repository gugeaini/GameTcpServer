#include<iostream>
#include"GameChannel.h"
#include"GameMsg.h"
#include"msg.pb.h"
#include"AOIWrold.h"
using namespace std;

class myPlayer :
	public Player
{
public:
	int x = 0;
	int y = 0;
	string name;
public:
	myPlayer(int _x,int _y,string _name):x(_x),y(_y),name(_name){}
	// 通过 Player 继承
	virtual int GetX() override
	{
		return x;
	}
	virtual int GetY() override
	{
		return y;
	}
};

int main()
{
//Tcp通道层测试代码
#if 0

	pb::SyncPid* pmsg = new pb::SyncPid();
	pmsg->set_pid(1);
	pmsg->set_username("guge");

	GameMsg pp(GameMsg::MSG_TYPE_LOGIN_ID_NAME, pmsg);
	string str1 = pp.serilize();
	cout << str1 << endl;
	for (auto mei : str1)
	{
		printf("%02x", mei);
	}
	puts("");
	
	char buf[] = { 0x08,0x01,0x12,0x04,0x67,0x75,0x67,0x65 };
	string str2(buf, sizeof(buf));

	GameMsg qq(GameMsg::MSG_TYPE_LOGIN_ID_NAME, str2);
	cout << dynamic_cast<pb::SyncPid*>(qq.pMsg)->pid() << endl;
	cout << dynamic_cast<pb::SyncPid*>(qq.pMsg)->username() << endl;
#endif

	AOIWrold w(20, 200, 50, 230, 6, 6);
	
	myPlayer p1(60, 107,"1");
	myPlayer p2(93, 112,"2");
	myPlayer p3(148, 134,"3");
	w.AddPlayer(&p1);
	w.AddPlayer(&p2);
	w.AddPlayer(&p3);

	auto std_list = w.GetsrdPlayers(&p3);
	for (auto &signle : std_list)
	{
		dynamic_cast<myPlayer*>(signle);
		std::cout << dynamic_cast<myPlayer*>(signle)->name << std::endl;
	}

	/*初始化框架*/
	ZinxKernel::ZinxKernelInit();
	/*添加Tcp通道到框架*/
	ZinxKernel::Zinx_Add_Channel(*(new ZinxTCPListen(8899, new GameConnFact())));

	ZinxKernel::Zinx_Run();
	/*去初始化框架*/
	ZinxKernel::ZinxKernelFini();

	return 0;
}