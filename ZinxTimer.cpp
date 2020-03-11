#include "ZinxTimer.h"
#include<sys/timerfd.h>
#include<unistd.h>

/*创建定时文件描述符*/
bool ZinxTimerChannel::Init()
{
	bool bRet = false;
	/*创建定时文件描述符*/
	int fd=timerfd_create(CLOCK_MONOTONIC, 0);
	if (0 <= fd)
	{
		/*设置定时时间*/
		struct itimerspec itpec {
			{1, 0}, { 1,0 }
		};
		if (0 == timerfd_settime(fd, 0, &itpec, NULL))
		{
			m_timerFd = fd;
			bRet = true;
		}
	}
	return bRet;
}
/*通过文件描述符判断超时次数*/
bool ZinxTimerChannel::ReadFd(std::string& _input)
{
	bool bRet = false;
	char buf[8] = { 0 };
	if (sizeof(buf) == read(m_timerFd, buf, sizeof(buf)))
	{
		_input.assign(buf, sizeof(buf));
		bRet = true;
	}
	return bRet;
}

bool ZinxTimerChannel::WriteFd(std::string& _output)
{
	return false;
}
/*关闭文件描述符*/
void ZinxTimerChannel::Fini()
{
	close(m_timerFd);
	m_timerFd = -1;
}
/*返回文件描述符*/
int ZinxTimerChannel::GetFd()
{
	return m_timerFd;
}
/*返回关键字*/
std::string ZinxTimerChannel::GetChannelInfo()
{
	return "TimerChannel";
}
/*超时后交给-->业务层定时任务管理类*/
AZinxHandler* ZinxTimerChannel::GetInputNextStage(BytesMsg& _oInput)
{
	return &TimerMngHandler::Getinstance();
}

TimerMngHandler TimerMngHandler::single;
TimerMngHandler::TimerMngHandler()
{
	/*创建十个齿轮的时间转轮*/
	for (int i = 0; i < 10; ++i)
	{
		std::list<TimerTask*> tmp;	//空的齿轮链
		m_TimerTask_list.push_back(tmp);
	}
}
/*根据定时事件，遍历定时任务*/
IZinxMsg* TimerMngHandler::InternelHandle(IZinxMsg& _oInput)
{
	/*获取（定时器）channel层传来的超时次数*/
	unsigned long iTimeoutCount = 0;
	GET_REF2DATA(BytesMsg, obyes, _oInput);
	obyes.szData.copy((char*)&iTimeoutCount, sizeof(iTimeoutCount), 0);

	/*超时次数执行*/
	while (iTimeoutCount-- > 0)
	{
		/*定义一个任务缓存区*/
		std::list<TimerTask*> m_cache;
		/*转轮指针前进一步*/
		m_index = (m_index + 1) % 10;
		/*遍历当前齿轮上的任务，0 >= 圈数的执行任务*/
		for (auto itr = m_TimerTask_list[m_index].begin(); itr != m_TimerTask_list[m_index].end();)
		{
			if (0 >= (*itr)->count)
			{
				/*结束圈了*/
				m_cache.push_back(*itr);	//缓存起来待会在执行 防止在任务中的销毁节点动作影响到AddTask
				auto tmp = itr;
				itr = m_TimerTask_list[m_index].erase(itr);
				AddTask(*tmp);
			}
			else
			{
				/*不是结束圈，圈数-1*/
				(*itr)->count--;
				++itr;
			}
		}

		/*执行缓冲区任务*/
		for (auto task : m_cache)
		{
			task->Proc();
		}
	}
	
	return nullptr;
}

AZinxHandler* TimerMngHandler::GetNextHandler(IZinxMsg& _oNextMsg)
{
	return nullptr;
}

void TimerMngHandler::AddTask(TimerTask* _pTask)
{
	/*计算当前任务需要放在哪个次轮上*/
	int index = (_pTask->GetTimeSec() + m_index) % 10;
	/*计算所需圈数*/
	_pTask->count = _pTask->GetTimeSec() / 10;
	/*把任务放到该齿轮*/
	m_TimerTask_list[index].push_back(_pTask);
}

void TimerMngHandler::DelTask(TimerTask* _pTask)
{
	for (auto &pTask_list : m_TimerTask_list)
	{
		for (auto pTask : pTask_list)
		{
			if (pTask == _pTask)
			{
				pTask_list.remove(_pTask);
				return;
			}
		}
	}

}
