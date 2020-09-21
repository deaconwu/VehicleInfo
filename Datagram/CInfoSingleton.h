#ifndef __CINFO_SIGNLETON_H__
#define __CINFO_SIGNLETON_H__

template <typename T>

class CSingleton
{
public:
	CSingleton() {}
	~CSingleton()
	{
		if (NULL != m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = NULL;
		}
	}

	static T* GetInstance()
	{
		if(NULL == m_pInstance)
			m_pInstance = new T();

		return m_pInstance;
	}

protected:
	static T* m_pInstance;
};

#endif