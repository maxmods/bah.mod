#ifndef _GCC_WIN_STRINGS
#define _GCC_WIN_STRINGS

#if defined(_PS3) || defined(__PS3__) || defined(SN_TARGET_PS3)
                                                                                                                                                                                                   
#else
	#if (defined(__GNUC__)  || defined(__GCCXML__)) && !defined(_WIN32)
		#ifndef _stricmp   	 	 
			int _stricmp(const char* s1, const char* s2);  	 	 
		#endif 
		int _strnicmp(const char* s1, const char* s2, size_t n);
		char *_strlwr(char * str );
		#ifndef _vsnprintf
		#define _vsnprintf vsnprintf
		#endif 
	#endif
#endif

#endif // _GCC_WIN_STRINGS
