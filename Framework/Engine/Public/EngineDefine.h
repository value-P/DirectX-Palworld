#pragma once

/********************
	Win32 매크로
********************/
#define WIN32_LEAN_AND_MEAN 


/********************
	디버그 매크로
********************/
#define MESSAGE_BOX(_message)							\
		MessageBox(NULL, _message, L"Error", MB_OK)		\

// 표현식이 참인지 확인합니다.
#define CHECK(_expression)								\
		assert(_expression);							\

#define CHECKF(_expression, _message)					\
		if (!(_expression))								\
		{												\
			MESSAGE_BOX(_message);						\
			CHECK(_expression);							\
		}												\

// HRESULT 반환 값이 참인지 확인합니다.
#define ENSURE(_hr)										\
		(_hr);											\

#define ENSUREF(_hr, _message)							\
		if (!SUCCEEDED(_hr))							\
		{												\
			MESSAGE_BOX(_message);						\
			ENSURE(_hr);								\
		}												\


/********************
	dll 매크로
********************/
#ifdef ENGINE_EXPORTS
#define ENGINE_DLL _declspec(dllexport)
#else
#define ENGINE_DLL _declspec(dllimport)
#endif


/********************
	싱글턴 매크로
********************/
#define DELETE_COPY(_classType)								\
		public:												\
		_classType() = default;								\
		virtual ~_classType() = default;					\
															\
		_classType(const _classType&) = delete;				\
		_classType(_classType&&) = delete;					\
															\
		_classType& operator=(const _classType&) = delete;	\
		_classType& operator=(_classType&&) = delete;		\


#define DECLARE_SINGLETON(_classType)						\
		DELETE_COPY(_classType)								\
		private:											\
		static std::shared_ptr<_classType> m_spInstance;	\
															\
		public:												\
		static std::shared_ptr<_classType>& GetInstance();	\


#define IMPLEMENT_SINGLETON(_classType)									\
		std::shared_ptr<_classType> _classType::m_spInstance = nullptr;	\
																		\
		std::shared_ptr<_classType>& _classType::GetInstance()			\
		{																\
			if (!m_spInstance)											\
			{															\
				m_spInstance = std::make_shared<_classType>();			\
			}															\
			return m_spInstance;										\
		}

/********************
	Release 매크로
********************/
#define SharedNull(_sharedptr)	if(_sharedptr) { _sharedptr = nullptr;}