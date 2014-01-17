//#include <iostream>
//#include <boost/thread/mutex.hpp>
//#include <boost/bind.hpp>
//#include <boost/thread.hpp>
//#include <assert.h>
//
//enum Task_Statu
//{
//	TASK_STATU_CREATE=0,
//	TASK_STATU_START=1,
//	TASK_STATU_STOP=2,
//	TASK_STATU_END=3,
//};
//class Task
//{
//public:
//	Task();
//	bool start();
//	virtual void svr()=0;
//	Task_Statu statu(){return m_statu;}
//	void wait();
//	bool wait(size_t millisec);
//	void interrupt();
//	virtual ~Task(){}
//private:
//	Task(Task& other);
//	Task& operator=(Task& other);
//	void thread_start()
//	{
//		svr();
//		boost::recursive_mutex::scoped_lock _lock(m_mutex);
//		m_statu=TASK_STATU_END;
//	}
//	Task_Statu m_statu;
//	boost::recursive_mutex m_mutex;
//	boost::shared_ptr<boost::thread> m_pThread;
//};
//Task::Task()
//{
//	m_statu=TASK_STATU_CREATE;
//}
//void Task::interrupt()
//{
//	if(m_pThread.get()!=NULL)m_pThread->interrupt();
//}
//void Task::wait()
//{
//	if(m_pThread.get()!=NULL)
//	 m_pThread->join();
//}
//bool Task::wait(size_t millisec)
//{
//	if(m_pThread.get()==NULL)return false;
//	return m_pThread->timed_join(boost::get_system_time()+boost::posix_time::millisec(millisec));
//}
//bool Task::start()
//{
//	boost::recursive_mutex::scoped_lock _lock(m_mutex);
//  if(m_statu!=TASK_STATU_CREATE&&m_statu!=TASK_STATU_END)
//  {
//	  return false;
//  }
//  m_statu=TASK_STATU_START;
//  m_pThread.reset(new boost::thread(boost::bind(&Task::thread_start,this)));
//  return true;
//}
//
//
//
//class _TestTask:public Task
//{
//public:
//	_TestTask(int millisectimer):timer_t(millisectimer)
//	{
//
//	}
//	virtual void svr()
//	{
//		boost::thread::sleep(boost::get_system_time()+boost::posix_time::millisec(timer_t));
//		result="ok";
//	}
//	int timer_t;
//	std::string result;
//};
//#define BOOST_TEST_INCLUDE
//#ifdef BOOST_TEST_INCLUDE
//#define BOOST_TEST_MODULE TaskTest
//#include <boost/test/included/unit_test.hpp>
//#include <iostream>
//#include <vector>
//struct _Fixture 
//{
//	_Fixture() 
//	{ 
//					std::cout << "setup TaskTest " << std::endl;
//	}
//	~_Fixture() { std::cout << "teardown" << std::endl; }
//
//};
//
//BOOST_AUTO_TEST_SUITE( test )
//
//BOOST_FIXTURE_TEST_CASE(ThreadPool_task_case, _Fixture )
//{
//	int sleepTime=300;
//	int waitTime= 400;
//	int runcount=10;
//	while(runcount>0)
//	{
//		boost::shared_ptr<_TestTask> testArray[10];
//		for (int i=0;i!=_countof(testArray);i++)
//		{
//			testArray[i].reset(new _TestTask(sleepTime));
//		}
//		for (int i=0;i!=_countof(testArray);i++)
//		{
//			testArray[i]->start();
//		}
//
//		boost::thread::sleep(boost::get_system_time()+boost::posix_time::millisec(waitTime));
//		for (int i=0;i!=_countof(testArray);i++)
//		{
//			BOOST_CHECK(testArray[i]->wait(0));
//			BOOST_CHECK(testArray[i]->result=="ok");
//		}
//		runcount--;
//	}
//	while(runcount>0)
//	{
//		boost::shared_ptr<_TestTask> testArray[10];
//		for (int i=0;i!=_countof(testArray);i++)
//		{
//			testArray[i].reset(new _TestTask(sleepTime));
//		}
//		for (int i=0;i!=_countof(testArray);i++)
//		{
//			testArray[i]->start();
//		}
//
//		for (int i=0;i!=_countof(testArray);i++)
//		{
//			testArray[i]->interrupt();
//			BOOST_CHECK(testArray[i]->wait(0));
//		}
//		runcount--;
//	}
//  }
//
//BOOST_AUTO_TEST_SUITE_END()
//#endif
