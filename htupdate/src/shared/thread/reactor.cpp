#include <assert.h>
#include "reactor.h"
#include <boost/function.hpp>
#include "event_demultiplexer.h"
#include <list>
#include <boost/timer.hpp>
#include <boost/thread.hpp>
/// reactor的实现类
enum 
{
	REACTOR_WORK_THREAD_POOL_SIZE=0x05,
};

class ReactorImplementation
{
public:
    /// 构造函数
	 ReactorImplementation(	boost::shared_ptr<TaskPool> pTaskPool);
    /// 析构函数
    ~ReactorImplementation();

    /// 向reactor中注册关注事件evt的handler
    /// @param  handler 要注册的事件处理器
    /// @param  evt     要关注的事件
    /// @retval =0      注册成功
    /// @retval <0      注册出错
    int RegisterHandler(boost::shared_ptr<EventHandler> handler, event_t evt);

    /// 从reactor中移除handler
    /// @param  handler 要移除的事件处理器
    /// @retval =0      移除成功
    /// @retval <0      移除出错
    int RemoveHandler(boost::shared_ptr<EventHandler> handler);

    /// 处理事件,回调注册的handler中相应的事件处理函数
    /// @param  timeout 超时时间

    void HandleEvents(int timeout);

	void RegisterTimer(boost::function< void() > task,int millisecWait);
private:
	typedef std::pair<boost::function< void() >/* task*/,boost::system_time/* run_time*/ >  TimerDataType;
	std::list<TimerDataType > m_timers;
    EventDemultiplexer *                m_demultiplexer; ///< 事件多路分发器
    EventHandlerListType  m_handlers;      ///< 所有handler集合  
	boost::shared_ptr<TaskPool> m_tasks;
	boost::mutex m_mutex;
};

///////////////////////////////////////////////////////////////////////////////

/// 构造函数
Reactor::Reactor(boost::shared_ptr<TaskPool> pTaskPool/* =NULL */)
{
	m_reactor_impl = new ReactorImplementation(pTaskPool);
}

/// 析构函数
Reactor::~Reactor()
{
    delete m_reactor_impl;
}

/// 向reactor中注册关注事件evt的handler
/// @param  handler 要注册的事件处理器
/// @param  evt     要关注的事件
/// @retval =0      注册成功
/// @retval <0      注册出错
int Reactor::RegisterHandler(boost::shared_ptr<EventHandler>  handler, event_t evt)
{
    return m_reactor_impl->RegisterHandler(handler, evt);
}

/// 从reactor中移除handler
/// @param  handler 要移除的事件处理器
/// @retval =0      移除成功
/// @retval <0      移除出错
int Reactor::RemoveHandler(boost::shared_ptr<EventHandler> handler )
{
    return m_reactor_impl->RemoveHandler(handler);
}

/// 处理事件,回调注册的handler中相应的事件处理函数
/// @param  timeout 超时时间
void Reactor::HandleEvents(int timeout)
{
    m_reactor_impl->HandleEvents(timeout);
}
void Reactor::RegisterTimer(boost::function<void()> task, int ms)
{
	m_reactor_impl->RegisterTimer(task,ms);
}
///////////////////////////////////////////////////////////////////////////////


/// 构造函数
ReactorImplementation::ReactorImplementation(boost::shared_ptr<TaskPool> ProcessTaskThread)
{
#if defined(_WIN32)
    m_demultiplexer = new SelectDemultiplexer();
#elif defined(__linux__)
    m_demultiplexer = new EpollDemultiplexer();
#else
#error "目前还不支持该平台"
#endif // _WIN32
	if(ProcessTaskThread==NULL)
	{
	  ProcessTaskThread.reset(new TaskPool);
	}
	m_tasks=ProcessTaskThread;
	m_tasks->start(REACTOR_WORK_THREAD_POOL_SIZE);
}


/// 析构函数
ReactorImplementation::~ReactorImplementation()
{
    delete m_demultiplexer;
}

/// 向reactor中注册关注事件evt的handler
/// @param  handler 要注册的事件处理器
/// @param  evt     要关注的事件
/// @retval =0      注册成功
/// @retval <0      注册出错
int ReactorImplementation::RegisterHandler(boost::shared_ptr<EventHandler> handler , event_t evt)
{
	boost::mutex::scoped_lock _lock(m_mutex);
    handle_type handle = handler->GetHandle();
	EventHandlerListType::iterator it = m_handlers.find(handle);
    if (it == m_handlers.end())
    {
        m_handlers[handle] = handler;
    }
    return m_demultiplexer->RequestEvent(handle, evt);
}

/// 从reactor中移除handler
/// @param  handler 要移除的事件处理器
/// @retval =0      移除成功
/// @retval <0      移除出错
int ReactorImplementation::RemoveHandler(boost::shared_ptr<EventHandler> handler)
{
	boost::mutex::scoped_lock _lock(m_mutex);
    handle_type handle = handler->GetHandle();
    m_handlers.erase(handle);
    return m_demultiplexer->UnrequestEvent(handle);
}
void ReactorImplementation::RegisterTimer(boost::function< void() > task,int millisecWait)
{
	boost::mutex::scoped_lock _lock(m_mutex);
	m_timers.push_back(std::make_pair(task,boost::get_system_time()+boost::posix_time::millisec(millisecWait)));
}
/// 处理事件,回调注册的handler中相应的事件处理函数
/// @param  timeout 超时时间
void ReactorImplementation::HandleEvents(int timeout)
{
	boost::mutex::scoped_lock _lock(m_mutex);
	std::list<TimerDataType>::iterator itr=m_timers.begin();
	while(itr!=m_timers.end())
	{
		if(itr->second < boost::get_system_time())
		{
			TimerDataType::first_type Fun=itr->first;
			m_tasks->postTask(Fun);
			itr=m_timers.erase(itr);
		}
	}
	if(m_handlers.empty())
	{
		return ;
	}
    m_demultiplexer->WaitEvents(&m_handlers,m_tasks,timeout);
}