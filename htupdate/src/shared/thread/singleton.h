#ifndef _SINGLETON_H_
#define _SINGLETON_H_
#include <boost/thread.hpp>
template<class TYPE>
class Singleton
{
public:

	template<class _TYPE>
	class Garbo
	{
	public:
		~Garbo()
		{
			if(Singleton<_TYPE>::m_t)
			{
				delete Singleton<_TYPE>::m_t;
				Singleton<_TYPE>::m_t=NULL;
			}
		}
	};
	static TYPE* GetInstance()
	{
		if(m_t==NULL)
		{
			boost::mutex::scoped_lock _AutoLock(m_mutex);
			static Garbo<TYPE> _garbo; 
			

			if(m_t==NULL)
			{
				m_t=new TYPE();
				m_isInit=true;
			}
		}
		while(!m_isInit){}
		return m_t;
	}
	~Singleton()
	{
		delete Singleton<TYPE>::m_t;
		Singleton<TYPE>::m_t=NULL;
	}
	
	
private:
	static TYPE* m_t;
	static bool m_isInit;
	static boost::mutex m_mutex;
};
template<class TYPE>
TYPE* Singleton<TYPE>::m_t=NULL;

template<class TYPE>
TYPE* Singleton<TYPE>::m_isInit=false;



#define GetLockFunInstance() _GetInstance().type
#endif