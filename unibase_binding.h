/*
 * Excelfore Universal Base Library
 * Copyright (C) 2019 Excelfore Corporation (https://excelfore.com)
 *
 * This file is part of Excelfore-unibase.
 *
 * Excelfore-unibase is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Excelfore-unibase is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Excelfore-unibase.  If not, see
 * <https://www.gnu.org/licenses/old-licenses/gpl-2.0.html>.
 */
/**
 * @defgroup unibase_binding bind unibase to a specific platform
 * @{
 * @file unibase_binding.h
 * @author Shiro Ninomiya <shiro@excelfore.com>
 * @copyright Copyright (C) 2019 Excelfore Corporation
 *
 * @brief functions to bind unibase to a specific platform
 *
 * - This layer support binding to a specific platform.
 * - The implementation is platform specific.
 * - Using this binding is optional, and for the unibase library,
 * a diferent binding layer can be used.
 */

#ifndef __UNIBASE_BINDING_H_
#define __UNIBASE_BINDING_H_

/**
 * @brief ubb_memory_out_init uses this value to allocate internal buffer when
 *	'mem' parameter is NULL.
 */
#define UBB_DEFAULT_DEBUG_LOG_MEMORY (64*1024)

/**
 * @brief ubb_memory_out always add this end mark at the end of printing.
 */
#define UBB_MEMOUT_ENDMARK "---###---"

/**
 * @brief ubb_default_initpara uses this string as 'ub_log_initstr'
 */
#define UBB_DEFAULT_LOG_INITSTR "4,ubase:45"

/**
 * @brief get a string from an environment variable
 */
#ifndef UBB_GETENV
#define UBB_GETENV getenv
#endif

/**
 * @brief initialize the internal memory_out function
 * @param mem	use this memory as a buffer for memory_out.
 *	if mem is NULL, allocate memory internally for 'size' bytes
 * @param size	size of memory to be used for memory_out
 * @return 0 on success, -1 on error
 * @note if this is not explicitly called, it is called internally with mem=NULL
 *	and size=UBB_DEFAULT_DEBUG_LOG_MEMORY
 */
int ubb_memory_out_init(char *mem, int size);

/**
 * @brief de-initialize memory_out function
 * @return 0 on success, -1 on error
 * @note this must be called whichever case of the explicit call or
 * 	the internal call of 'ubb_memory_out_init'
 */
int ubb_memory_out_close(void);

/**
 * @brief return the memory pointer of memory_out
 * @return memory pointer
 */
char *ubb_memory_out_buffer(void);

/**
 * @brief return the most recent output line in memory_out
 * @param str	pointer of string pointer to return result string
 * @param size	pointer of integer pointer to return result size
 * @return 0 on success, -1 on error
 */
int ubb_memory_out_lastline(char **str, int *size);

/**
 * @brief return the all written line data in the buffer
 * @param str	allocated pointer of the returned data
 * @param size	size of the returned data
 * @return 0 on success, -1 on error
 * @note returned pointer in *str must be freed by the caller.
 * 	the allocation size is the same as the size in ubb_memory_out_init
 */
int ubb_memory_out_alldata(char **rstr, int *size);

/**
 * @brief write the data in memory_out buffer into a file
 * @param fname	file name
 * @return 0 on success, -1 on error
 */
int ubb_memory_file_out(const char *fname);

/**
 * @brief return the default initialization parameters supported in the binding layer
 * @param init_para	pointer of unibase initialization parameter
 */
void ubb_default_initpara(unibase_init_para_t *init_para);

typedef uint64_t(*get64ts_t)(void);
/**
 * @brief set gptptime function for ub_gptp_gettime64
 * @param func	a function which returns 64-bit gptp time
 */
void ubb_set_gptp_gettime64(get64ts_t func);

/**
 * @brief initialize unibase with 'ubb_default_initpara'
 * @note ubb_memory_out is initialize with no memory. To use memory logging,
 *       'ubb_memory_out_init' needs to be called separately
 */
void ubb_unibase_easyinit(void);

#endif
/** @}*/
