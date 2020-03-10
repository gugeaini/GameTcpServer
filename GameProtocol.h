#pragma once
#include <zinx.h>
#include<string>

/*声明TPC通道层类*/
class GameChannel;
class GameRole;
class GameProtocol :
	public Iprotocol
{
	std::string m_szLast;
public:
	GameProtocol();
	virtual ~GameProtocol();
	GameChannel* m_Channel = NULL;
	GameRole* m_Role = NULL;
	//协议  ==  一条报文： | （内容长度）4字节 | （ID）4字节 | （内容） 长度字节... |
	virtual UserData* raw2request(std::string _szInput) override;
	virtual std::string* response2raw(UserData& _oUserData) override;
	virtual Irole* GetMsgProcessor(UserDataMsg& _oUserDataMsg) override;
	virtual Ichannel* GetMsgSender(BytesMsg& _oBytes) override;
};

