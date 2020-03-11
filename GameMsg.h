#pragma once
#include<iostream>
#include <zinx.h>
#include<google/protobuf/message.h>
#include"msg.pb.h"
#include<list>
using namespace std;

class GameMsg :
	public UserData
{
public:
	/*用户的请求信息*/
	google::protobuf::Message* pMsg = NULL;
	enum MSG_TYPE
	{
		MSG_TYPE_LOGIN_ID_NAME = 1,	//登录消息
		MSG_TYPE_CHAT_CONTENT = 2,	//聊天消息
		MSG_TYPE_NEW_POSITION = 3,	//新位置
		MSG_TYPE_BROADCAST = 200,		//广播
		MSG_TYPE_LOGOFF_ID_NAME = 201,//下线消息
		MSG_TYPE_SRD_POSITION = 202	//周围玩家的位置
	}enMsgType;
	/*已知消息内容创建消息对象*/
	GameMsg(MSG_TYPE _type, google::protobuf::Message* _pMsg);
	/*将字节流内容转换成消息结构*/
	GameMsg(MSG_TYPE _type, string _stream);

	/*序列化本消息*/
	string serilize();

	virtual ~GameMsg();
};

class MultiMsg :
	public UserData
{
public:
	list<GameMsg*> m_Msgs;
	MultiMsg();
	virtual ~MultiMsg();
};