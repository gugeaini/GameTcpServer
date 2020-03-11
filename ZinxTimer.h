#pragma once
#include<zinx.h>
#include<list>
#include<vector>

/*通道层--定时器类*/
class ZinxTimerChannel:
	public Ichannel
{
	int m_timerFd = -1;

	// 通过 Ichannel 继承
	virtual bool Init() override;
	virtual bool ReadFd(std::string& _input) override;
	virtual bool WriteFd(std::string& _output) override;
	virtual void Fini() override;
	virtual int GetFd() override;
	virtual std::string GetChannelInfo() override;
	virtual AZinxHandler* GetInputNextStage(BytesMsg& _oInput) override;
};

/*自定义业务层--定时任务基类*/
class TimerTask
{
public:
	/*任务动作*/
	virtual void Proc() = 0;
	/*返回初始化 定时时长*/
	virtual int GetTimeSec() = 0;
	/*所剩圈数*/
	int count = -1;
};

/*Azinx层--定时任务管理类*/
class TimerMngHandler :
	public AZinxHandler
{
	/*任务容器	时间转轮模式*/
	std::vector<std::list<TimerTask*>> m_TimerTask_list;	
	/*时间转轮指针 默认从0开始*/
	int m_index = 0;
	static TimerMngHandler single;
	/*创建十个齿轮的时间转轮*/
	TimerMngHandler();
public:
	/*根据定时事件，遍历定时任务*/
	virtual IZinxMsg* InternelHandle(IZinxMsg& _oInput) override;
	/*下一个环节 目前没有++ */
	virtual AZinxHandler* GetNextHandler(IZinxMsg& _oNextMsg) override;

	static TimerMngHandler& Getinstance(){
		return single;
	}
	/*添加任务*/
	void AddTask(TimerTask* _pTask);
	/*删除任务*/
	void DelTask(TimerTask* _pTask);

};