#pragma once
#include<ZinxTCP.h>
#include"GameProtocol.h"

/*通道层--Tcp数据类*/
class GameChannel :
	public ZinxTcpData
{
public:
	GameChannel(int _fd);
	virtual ~GameChannel();
	GameProtocol* m_proto = NULL;	//每个客户端tcp连接绑定一个协议层处理
	// 返回协议层对象
	virtual AZinxHandler* GetInputNextStage(BytesMsg& _oInput) override;
};

/*通道层--Tcp连接工厂类*/
class GameConnFact :
	public IZinxTcpConnFact
{
	// new 一个Tcp数据类（GameChannel）
	virtual ZinxTcpData* CreateTcpDataChannel(int _fd) override;
};