#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_
#include <BlockQueue.h>
#include <assert.h>


class TaskPool
{
public:
	TaskPool():m_isDestory(false)
	{
		
	}
	void start(int thread_count)
	{
		if(m_gourp.size()!=0&&thread_count < m_gourp.size())
		{
			return ;
		}
		thread_count = m_gourp.size()-thread_count;
		for (int i=0;i!=thread_count;i++)
		{
			m_gourp.create_thread(boost::bind(&TaskPool::workerThread,this));
		}
		return ;
	}
	void postTask(boost::function<void()> taskFun)
	{
		m_taskQueue.dequeue(taskFun);
	}
	void workerThread()
	{
		while(!m_isDestory)
		{
			boost::function<void()> taskfun=NULL;
			if(m_taskQueue.dequeue(taskfun))
				taskfun();
		}
	}
	void destory()
	{
		m_isDestory=true;
		m_taskQueue.destory();
		m_gourp.interrupt_all();
	}
	~TaskPool()
	{
		destory();
	}
private:
	boost::thread_group m_gourp;
	TaskPool(TaskPool& task);
	TaskPool& operator=(const TaskPool& task);
	BlockQueue<boost::function<void()> > m_taskQueue;
	bool m_isDestory;
};
#endif