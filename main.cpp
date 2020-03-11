#include<iostream>
#include"GameChannel.h"
#include"GameMsg.h"
#include"msg.pb.h"
#include"AOIWrold.h"
#include"GameRole.h"
#include"RandomName.h"
#include"ZinxTimer.h"

extern RandomName random_name;
using namespace std;



//测试游戏类
#if 0
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
#endif

/*守护进程带 监控进程*/
void Daemon()
{
	//1 fork
	pid_t pid = fork();
	if (0 > pid)
	{
		//失败
		exit(-1);
	}
	//2 杀死父进程
	if (0 < pid)
	{
		exit(0);
	}
	//3	设置会话
	setsid();
	//4 修改工作目录
	//chdir(getenv("HOME"));//切换到家目录
	//5 重定向文件描述符
	int fd = open("null.guge",O_RDWR|O_CREAT,0666);
	if (fd > 0)
	{
		dup2(fd, 0);
		dup2(fd, 1);
		dup2(fd, 2);
		close(fd);
	}

	/*进程监控*/
	while (1)
	{
		pid_t childPid = fork();
		if (childPid < 0)
		{
			exit(-1);
		}

		//父进程
		if (childPid > 0)
		{
			int istatus;
			wait(&istatus);
			if (0 == istatus)
			{
				/*服务器正常退出*/
				exit(0);
			}
		}
		else
		{
			/*重启服务器*/
			break;
		}
	}
}

int main()
{
	Daemon();

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

//游戏添加测试代码
#if 0
	
	AOIWrold w(0, 400, 0, 400, 20, 20);
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
#endif

	random_name.LoadFile();
	/*初始化框架*/
	ZinxKernel::ZinxKernelInit();
	/*定时器通道*/
	ZinxKernel::Zinx_Add_Channel(*(new ZinxTimerChannel()));
	/*添加Tcp通道到框架*/
	ZinxKernel::Zinx_Add_Channel(*(new ZinxTCPListen(8899, new GameConnFact())));

	ZinxKernel::Zinx_Run();
	/*去初始化框架*/
	ZinxKernel::ZinxKernelFini();

	return 0;
}