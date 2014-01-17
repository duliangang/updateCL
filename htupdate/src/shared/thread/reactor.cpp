#include <assert.h>
#include "reactor.h"
#include <boost/function.hpp>
#include "event_demultiplexer.h"
#include <list>
#include <boost/timer.hpp>
#include <boost/thread.hpp>
/// reactor��ʵ����
enum 
{
	REACTOR_WORK_THREAD_POOL_SIZE=0x05,
};

class ReactorImplementation
{
public:
    /// ���캯��
	 ReactorImplementation(	boost::shared_ptr<TaskPool> pTaskPool);
    /// ��������
    ~ReactorImplementation();

    /// ��reactor��ע���ע�¼�evt��handler
    /// @param  handler Ҫע����¼�������
    /// @param  evt     Ҫ��ע���¼�
    /// @retval =0      ע��ɹ�
    /// @retval <0      ע�����
    int RegisterHandler(boost::shared_ptr<EventHandler> handler, event_t evt);

    /// ��reactor���Ƴ�handler
    /// @param  handler Ҫ�Ƴ����¼�������
    /// @retval =0      �Ƴ��ɹ�
    /// @retval <0      �Ƴ�����
    int RemoveHandler(boost::shared_ptr<EventHandler> handler);

    /// �����¼�,�ص�ע���handler����Ӧ���¼�������
    /// @param  timeout ��ʱʱ��

    void HandleEvents(int timeout);

	void RegisterTimer(boost::function< void() > task,int millisecWait);
private:
	typedef std::pair<boost::function< void() >/* task*/,boost::system_time/* run_time*/ >  TimerDataType;
	std::list<TimerDataType > m_timers;
    EventDemultiplexer *                m_demultiplexer; ///< �¼���·�ַ���
    EventHandlerListType  m_handlers;      ///< ����handler����  
	boost::shared_ptr<TaskPool> m_tasks;
	boost::mutex m_mutex;
};

///////////////////////////////////////////////////////////////////////////////

/// ���캯��
Reactor::Reactor(boost::shared_ptr<TaskPool> pTaskPool/* =NULL */)
{
	m_reactor_impl = new ReactorImplementation(pTaskPool);
}

/// ��������
Reactor::~Reactor()
{
    delete m_reactor_impl;
}

/// ��reactor��ע���ע�¼�evt��handler
/// @param  handler Ҫע����¼�������
/// @param  evt     Ҫ��ע���¼�
/// @retval =0      ע��ɹ�
/// @retval <0      ע�����
int Reactor::RegisterHandler(boost::shared_ptr<EventHandler>  handler, event_t evt)
{
    return m_reactor_impl->RegisterHandler(handler, evt);
}

/// ��reactor���Ƴ�handler
/// @param  handler Ҫ�Ƴ����¼�������
/// @retval =0      �Ƴ��ɹ�
/// @retval <0      �Ƴ�����
int Reactor::RemoveHandler(boost::shared_ptr<EventHandler> handler )
{
    return m_reactor_impl->RemoveHandler(handler);
}

/// �����¼�,�ص�ע���handler����Ӧ���¼�������
/// @param  timeout ��ʱʱ��
void Reactor::HandleEvents(int timeout)
{
    m_reactor_impl->HandleEvents(timeout);
}
void Reactor::RegisterTimer(boost::function<void()> task, int ms)
{
	m_reactor_impl->RegisterTimer(task,ms);
}
///////////////////////////////////////////////////////////////////////////////


/// ���캯��
ReactorImplementation::ReactorImplementation(boost::shared_ptr<TaskPool> ProcessTaskThread)
{
#if defined(_WIN32)
    m_demultiplexer = new SelectDemultiplexer();
#elif defined(__linux__)
    m_demultiplexer = new EpollDemultiplexer();
#else
#error "Ŀǰ����֧�ָ�ƽ̨"
#endif // _WIN32
	if(ProcessTaskThread==NULL)
	{
	  ProcessTaskThread.reset(new TaskPool);
	}
	m_tasks=ProcessTaskThread;
	m_tasks->start(REACTOR_WORK_THREAD_POOL_SIZE);
}


/// ��������
ReactorImplementation::~ReactorImplementation()
{
    delete m_demultiplexer;
}

/// ��reactor��ע���ע�¼�evt��handler
/// @param  handler Ҫע����¼�������
/// @param  evt     Ҫ��ע���¼�
/// @retval =0      ע��ɹ�
/// @retval <0      ע�����
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

/// ��reactor���Ƴ�handler
/// @param  handler Ҫ�Ƴ����¼�������
/// @retval =0      �Ƴ��ɹ�
/// @retval <0      �Ƴ�����
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
/// �����¼�,�ص�ע���handler����Ӧ���¼�������
/// @param  timeout ��ʱʱ��
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