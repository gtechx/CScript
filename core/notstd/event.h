#pragma once

// C++11��variable_condition����һЩʹ���ϵ�ȱ�ݣ����Ի����ṩ
// ����win32��event���������һЩ�������߳�ͬ������

#include "config.h"
#if defined(PLATFORM_WINDOWS)
#include "event-win32.h"
#else
#include "event-posix.h"
#endif
