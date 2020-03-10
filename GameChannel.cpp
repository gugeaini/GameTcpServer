#include "GameChannel.h"
#include"GameRole.h"

GameChannel::GameChannel(int _fd):ZinxTcpData(_fd)
{
}

GameChannel::~GameChannel()
{
	if (NULL != m_proto)
	{
		ZinxKernel::Zinx_Del_Proto(*m_proto);
		delete m_proto;
	}
}

AZinxHandler* GameChannel::GetInputNextStage(BytesMsg& _oInput)
{
	/*返回协议层对象*/
	return m_proto;
}

ZinxTcpData* GameConnFact::CreateTcpDataChannel(int _fd)
{
	/*创建通道层对象*/
	GameChannel* pGameChannel = new GameChannel(_fd);
	/*创建协议层对象*/
	GameProtocol* pProtocol= new GameProtocol();
	/*创建玩家对象*/
	GameRole* pRole = new GameRole();

	/*绑定通道层 和 协议层*/
	pGameChannel->m_proto = pProtocol;
	pProtocol->m_Channel = pGameChannel;

	/*绑定协议层 和 业务层*/
	pProtocol->m_Role = pRole;
	pRole->m_Protocol = pProtocol;

	/*将协议对象添加到kernel*/
	ZinxKernel::Zinx_Add_Proto(*pProtocol);
	/*将玩家对象添加到kernel*/
	ZinxKernel::Zinx_Add_Role(*pRole);

	return pGameChannel;
}
