/**
 * @file   qq_debug.h
 * @author Xiang Wang <xiang_wang@trendmicro.com.cn>
 * @date   Sun May 13 22:12:25 2012
 *
 * @brief
 *
 *
 */

#ifndef __QQ_DEBUG_H__
#define __QQ_DEBUG_H__

extern  struct tm* get_currenttime();

extern void debug_info(const char* format , ...);

extern void debug_error(const char* format , ...);

#endif
