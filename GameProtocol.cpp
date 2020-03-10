#include "GameProtocol.h"
#include<iostream>
#include"GameMsg.h"
#include"GameChannel.h"
#include"msg.pb.h"
#include"GameRole.h"

using namespace std;

GameProtocol::GameProtocol()
{
}

GameProtocol::~GameProtocol()
{
	if (NULL != m_Role)
	{
		ZinxKernel::Zinx_Del_Role(*m_Role);
		delete m_Role;
	}
}

/*输入参数是通道层传来的原始报文
返回值是转换后的Msg消息对象
转换方式，TCP粘包处理*/
UserData* GameProtocol::raw2request(std::string _szInput)
{
	//一条报文：|（内容长度）4字节 |（ID）4字节 |（内容） 长度字节... |
	MultiMsg* pRet = new MultiMsg();
	/*连接报文*/
	m_szLast.append(_szInput);

	while (1)
	{
		/*判断长度*/
		if (m_szLast.size() < 8)
		{
			break;
		}
		/*解析报文前4字节	读取消息内容长度*/
		int iLength = 0;
		iLength |= m_szLast[0] << 0;
		iLength |= m_szLast[1] << 8;
		iLength |= m_szLast[2] << 16;
		iLength |= m_szLast[3] << 24;

		/*解析报文中4字节	读取id*/
		int id = 0;
		id |= m_szLast[4] << 0;
		id |= m_szLast[5] << 8;
		id |= m_szLast[6] << 16;
		id |= m_szLast[7] << 24;

		/*本条报文内容长度 不足完整 结束*/
		if (m_szLast.size() - 8 < iLength)
		{
			break;
		}
		/*有一条完整的报文	构造一条用户请求*/
		GameMsg* pMsg = new GameMsg((GameMsg::MSG_TYPE)id, m_szLast.substr(8, iLength));
		pRet->m_Msgs.push_back(pMsg);

		/*滑窗本条报文*/
		//m_szLast = m_szLast.substr(8 + iLength, m_szLast.size() - 8 - iLength);
		//从第0个开始叉掉8+iLength
		m_szLast.erase(0, 8 + iLength);
	}
//测试代码
#if 0
	for (auto single : pRet->m_Msgs)
	{
		cout<<single->pMsg->Utf8DebugString() << endl;
	}
	/*new 一个消息对象测试一下能否接受*/
	pb::Talk* pcsTalk = new pb::Talk();
	pcsTalk->set_content("hello");
	GameMsg* pcsMsg = new GameMsg(GameMsg::MSG_TYPE_CHAT_CONTENT, pcsTalk);
	ZinxKernel::Zinx_SendOut(*(pcsMsg), *this);
#endif
	return pRet;
}

/*
业务层发送回来的Msg消息对象
转换用户请求成原始报文发往通道层*/
std::string* GameProtocol::response2raw(UserData& _oUserData)
{
	GET_REF2DATA(GameMsg, oOutput, _oUserData);
	int id = 0;
	int iLength = 0;
	string MsgContent;

	/*Msg消息对象取出*/
	id = oOutput.enMsgType;
	MsgContent = oOutput.serilize();
	iLength = MsgContent.size();

	/*拼接原始报文 |（内容长度）4字节 |（ID）4字节 |（内容） 长度字节... |*/
	auto pret = new string();
	/*（内容长度）*/
	pret->push_back((iLength >> 0) & 0xff);
	pret->push_back((iLength >> 8) & 0xff);
	pret->push_back((iLength >> 16) & 0xff);
	pret->push_back((iLength >> 24) & 0xff);
	/*（ID）*/
	pret->push_back((id >> 0) & 0xff);
	pret->push_back((id >> 8) & 0xff);
	pret->push_back((id >> 16) & 0xff);
	pret->push_back((id >> 24) & 0xff);
	/*（内容）*/
	pret->append(MsgContent);

	/*返回可发送报文*/
	return pret;
}

Irole* GameProtocol::GetMsgProcessor(UserDataMsg& _oUserDataMsg)
{
	return m_Role;
}
/*返回数据发送到的通道层*/
Ichannel* GameProtocol::GetMsgSender(BytesMsg& _oBytes)
{
	return m_Channel;
}
