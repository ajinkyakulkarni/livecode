/* Copyright (C) 2015 Runtime Revolution Ltd.
 
 This file is part of LiveCode.
 
 LiveCode is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License v3 as published by the Free
 Software Foundation.
 
 LiveCode is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 for more details.
 
 You should have received a copy of the GNU General Public License
 along with LiveCode.  If not see <http://www.gnu.org/licenses/>.  */

#ifndef __LIBBROWSER_INTERNAL_H__
#define __LIBBROWSER_INTERNAL_H__

#include <libbrowser.h>

////////////////////////////////////////////////////////////////////////////////
// C++ Implementation Class

// Browser implementation base class
class MCBrowserBase : public MCBrowser
{
public:
	MCBrowserBase() : m_event_handler(nil), m_javascript_handler(nil)
	{
	}
	
	virtual ~MCBrowserBase()
	{
	}
	
	void SetEventHandler(MCBrowserEventHandler *p_handler);
	void SetJavaScriptHandler(MCBrowserJavaScriptHandler *p_handler);
	
	void OnNavigationBegin(bool p_in_frame, const char *p_url);
	void OnNavigationComplete(bool p_in_frame, const char *p_url);
	void OnNavigationFailed(bool p_in_frame, const char *p_url, const char *p_error);
	void OnDocumentLoadBegin(bool p_in_frame, const char *p_url);
	void OnDocumentLoadComplete(bool p_in_frame, const char *p_url);
	void OnDocumentLoadFailed(bool p_in_frame, const char *p_url, const char *p_error);
	
	void OnJavaScriptCall(const char *p_handler, MCBrowserListRef p_params);
	
	static bool BrowserListAdd(MCBrowser *p_browser);
	static void BrowserListRemove(MCBrowser *p_browser);
	static bool BrowserListFindWithView(void *p_view, MCBrowser *&r_browser);

private:
	struct MCBrowserListEntry
	{
		MCBrowser *browser;
		
		MCBrowserListEntry *next;
	};
	
	static MCBrowserListEntry *s_browser_list;
	
	MCBrowserEventHandler *m_event_handler;
	MCBrowserJavaScriptHandler *m_javascript_handler;
};

////////////////////////////////////////////////////////////////////////////////

typedef bool (*MCBrowserFactoryCreationFunc)(MCBrowserFactoryRef &r_factory);
struct MCBrowserFactoryMap
{
	const char *factory_id;
	MCBrowserFactoryRef instance;
	
	MCBrowserFactoryCreationFunc constructor;
};

extern MCBrowserFactoryMap s_factory_list[];

////////////////////////////////////////////////////////////////////////////////

bool MCBrowserAddRunloopAction(MCBrowserRunloopCallback p_callback, void *p_context);
void MCBrowserRemoveRunloopAction(MCBrowserRunloopCallback p_callback, void *p_context);
bool MCBrowserRunloopWait();

bool MCBrowserMemoryAllocate(size_t p_size, void *&r_mem);
bool MCBrowserMemoryReallocate(void *p_mem, size_t p_new_size, void *& r_new_mem);
void MCBrowserMemoryDeallocate(void *p_mem);
void MCBrowserMemoryClear(void *p_mem, size_t p_size);

inline bool MCBrowserMemoryAllocateClear(size_t p_size, void *&r_mem)
{
	void *t_mem;
	if (!MCBrowserMemoryAllocate(p_size, t_mem))
		return false;
	MCBrowserMemoryClear(t_mem, p_size);
	r_mem = t_mem;
	return true;
}

template <class T>
inline bool MCBrowserMemoryNew(T*&r_obj)
{
	return MCBrowserMemoryAllocateClear(sizeof(T), (void*&)r_obj);
}

template <class T>
inline void MCBrowserMemoryDelete(T* p_obj)
{
	MCBrowserMemoryDeallocate(p_obj);
}

template<typename T>
inline bool MCBrowserMemoryReallocate(T *p_block, size_t p_new_size, T*& r_new_block)
{
	return MCBrowserMemoryReallocate((void*)p_block, p_new_size, (void*&)r_new_block);
}

template <class T>
inline bool MCBrowserMemoryNewArray(uindex_t p_count, T*&r_array)
{
	return MCBrowserMemoryAllocateClear(sizeof(T) * p_count, (void*&)r_array);
}

template <class T>
inline bool MCBrowserMemoryResizeArray(uindex_t p_new_count, T*& x_array, uindex_t& x_count)
{
	if (MCBrowserMemoryReallocate(x_array, p_new_count * sizeof(T), x_array))
	{
		if (p_new_count > x_count)
			MCBrowserMemoryClear((uint8_t *)(x_array) + x_count * sizeof(T), (p_new_count - x_count) * sizeof(T));
		x_count = p_new_count;
		return true;
	}
	return false;
}

template <class T>
inline void MCBrowserMemoryDeleteArray(T*p_array)
{
	MCBrowserMemoryDeallocate(p_array);
}

////////////////////////////////////////////////////////////////////////////////

#endif //__LIBBROWSER_INTERNAL_H__
