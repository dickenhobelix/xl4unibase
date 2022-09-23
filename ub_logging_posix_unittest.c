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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <setjmp.h>
#include <cmocka.h>
#include "unibase.h"
#include "unibase_binding.h"

#define UB_LOGCAT 0
#define UB_LOGTSTYPE UB_CLOCK_MONOTONIC

static char memoutbuf[1025];
static const char *level_mark[]=DBGMSG_LEVEL_MARK;
static const char *endmark=UBB_MEMOUT_ENDMARK;

typedef struct omdata {
	int pnum1;
	int pnum2;
}omdata_t;

static void *direct_buffer_check(void *ptr)
{
	char pstr[64], qstr[80];
	int i,c,v;
	int pnum=*((int*)ptr);

	sprintf(pstr, "%s:%s:%d\n", level_mark[UBL_INFO], "ubase", pnum);
	sprintf(qstr, "%s%s", pstr, endmark);
	v=strlen(pstr);
	c=sizeof(memoutbuf)/v-1;
	for(i=0;i<c;i++){
		UB_LOG(UBL_INFO, "%d\n", pnum);
		if(strcmp(qstr, memoutbuf+i*v)) break;
	}
	assert_int_equal(c, i);
	UB_LOG(UBL_INFO, "%d\n", pnum); // this writes at the top
	assert_string_equal(qstr, memoutbuf);
	return NULL;
}

static void *out_lastline_check(void *ptr)
{
	char pstr1[64],pstr2[64];
	char *rstr;
	int rsize;
	int i,c,v;
	omdata_t *od=(omdata_t *)ptr;

	/* if many threads run this, the messages are mixed up.
	 * but it shouldn't break a string at middle */
	sprintf(pstr1, "%s:%s:%d\n", level_mark[UBL_INFO], "ubase", od->pnum1);
	sprintf(pstr2, "%s:%s:%d\n", level_mark[UBL_INFO], "ubase", od->pnum2);
	v=strlen(pstr1);
	c=sizeof(memoutbuf)/v-1;
	for(i=0;i<c;i++){
		UB_LOG(UBL_INFO, "%d\n", od->pnum1);
		assert_int_equal(0,ubb_memory_out_lastline(&rstr, &rsize));
		assert_int_equal(strlen(pstr1), rsize);
		if(memcmp(pstr1, rstr, rsize) && memcmp(pstr2, rstr, rsize)){
			rstr[rsize]=0;
			printf("pstr1=#%s#, pstr2=#%s#, rstr=#%s#\n",
			       pstr1, pstr2, rstr);
			assert_false(true);
		}
	}
	return NULL;
}

static void test_ub_log_memout(void **state)
{
	int pnum=99999;
	direct_buffer_check(&pnum);
}

static void test_ub_log_memout_thread(void **state)
{
	omdata_t od1={99999,11111};
	omdata_t od2={11111,99999};
	pthread_t th[100];
	int i;
	memoutbuf[sizeof(memoutbuf)-1]=0x5a;
	for(i=0;i<50;i++){
		pthread_create(&th[i*2], NULL, out_lastline_check, &od1);
		pthread_create(&th[i*2+1], NULL, out_lastline_check, &od2);
	}
	for(i=0;i<100;i++){
		pthread_join(th[i], NULL);
	}
	// check no buffer overflow happened
	assert_int_equal(0x5a, memoutbuf[sizeof(memoutbuf)-1]);
}

static void test_ub_log_memout_all(void **state)
{
	char pstr1[64];
	int i, res;
	uint32_t c, v;
	char *tstr="0123456789";
	char *str1;
	int size1;

	assert_int_equal(987654321,ub_gptp_gettime64());

	sprintf(pstr1, "%s:%s:%s\n", level_mark[UBL_INFO], "ubase", tstr);
	v=strlen(pstr1);
	c=sizeof(memoutbuf)/v-1;
	if(sizeof(memoutbuf)-(c*v+strlen(endmark)+1)>=v) c++;
	printf("%s:buffer size=%d, string size=%d, %d time in a buffer\n",
	       __func__, (int)sizeof(memoutbuf), v, c);
	res=ubb_memory_out_alldata(&str1,&size1);
	assert_null(str1);
	assert_int_not_equal(res, 0);

	UB_LOG(UBL_INFO, "%s\n", tstr);
	res=ubb_memory_out_alldata(&str1,&size1);
	assert_int_equal(res, 0);
	assert_memory_equal(pstr1, str1, size1);
	free(str1);

	for(i=0;i<(int)c-1;i++){
		UB_LOG(UBL_INFO, "%s\n", tstr);
	}
	res=ubb_memory_out_alldata(&str1,&size1);
	assert_int_equal(res, 0);
	assert_int_equal(size1, v*c);
	for(i=0;i<(int)c;i++){
		assert_memory_equal(pstr1, str1+v*i, v);
	}
	free(str1);

	UB_LOG(UBL_INFO, "%s\n", tstr);
	res=ubb_memory_out_alldata(&str1,&size1);
	assert_int_equal(res, 0);
	assert_memory_equal(pstr1+strlen(endmark)+1, str1, v-strlen(endmark)-1);
	for(i=0;i<(int)c-2;i++){
		assert_memory_equal(pstr1, str1+v-strlen(endmark)-1+v*i, v);
	}
	free(str1);
}

static uint64_t gptp_dummy_time64(void)
{
	return 987654321;
}

static int setup(void **state)
{
	unibase_init_para_t init_para;

	ubb_default_initpara(&init_para);
	init_para.ub_log_initstr="4,ubase:35";
	unibase_init(&init_para);
	ubb_memory_out_init(memoutbuf, sizeof(memoutbuf)-1);
	ubb_set_gptp_gettime64(gptp_dummy_time64);
	return 0;
}

typedef struct envstrd{
	int llevel;
	char *modname;
} envstrd_t;

static int setup_env1(void **state)
{
	unibase_init_para_t init_para;
	static envstrd_t envsd;

	ubb_default_initpara(&init_para);
	init_para.ub_log_initstr=UBL_OVERRIDE_ISTR("4,ubase:44,mod1:44,mod2:44,mod3:44",
						   "UBL_LOGTEST");
	unibase_init(&init_para);
	ubb_memory_out_init(memoutbuf, sizeof(memoutbuf)-1);
	envsd.llevel=UBL_INFO;
	envsd.modname="mod3";
	*state=&envsd;
	return 0;
}

static int setup_env2(void **state)
{
	static envstrd_t envsd;
	setenv("UBL_LOGTEST", "4,ubase:44,mod1:44,mod2:44,mod3:46", 1);
	setup_env1(state);
	envsd.llevel=UBL_DEBUG;
	envsd.modname="mod3";
	*state=&envsd;
	return 0;
}

static int setup_env3(void **state)
{
	static envstrd_t envsd;
	setenv("UBL_LOGTEST", "mod3:46", 1);
	setup_env1(state);
	envsd.llevel=UBL_DEBUG;
	envsd.modname="mod3";
	*state=&envsd;
	return 0;
}

static int setup_env4(void **state)
{
	static envstrd_t envsd;
	setenv("UBL_LOGTEST", "mod2:46", 1);
	setup_env1(state);
	envsd.llevel=UBL_DEBUG;
	envsd.modname="mod2";
	*state=&envsd;
	return 0;
}

#undef UB_LOGCAT
#define UB_LOGCAT 2
static int test_ub_log2(int llevel, int pnum)
{
	return UB_LOG(llevel, "%d\n", pnum);
}

#undef UB_LOGCAT
#define UB_LOGCAT 3
static int test_ub_log3(int llevel, int pnum)
{
	return UB_LOG(llevel, "%d\n", pnum);
}

static void test_ub_log_env_cat(void **state)
{
	char pstr[64], qstr[80];
	int v;
	int pnum=100;
	envstrd_t *envsd=(envstrd_t *)*state;

	sprintf(pstr, "%s:%s:%d\n", level_mark[UBL_INFO], envsd->modname, pnum);
	sprintf(qstr, "%s%s", pstr, endmark);
	v=strlen(pstr);
	if(!strcmp(envsd->modname,"mod2"))
		test_ub_log2(UBL_INFO, pnum);
	else
		test_ub_log3(UBL_INFO, pnum);
	assert_string_equal(qstr, memoutbuf);

	sprintf(pstr, "%s:%s:%d\n", level_mark[UBL_DEBUG], envsd->modname, pnum);
	sprintf(qstr, "%s%s", pstr, endmark);
	if(!strcmp(envsd->modname,"mod2"))
		test_ub_log2(UBL_DEBUG, pnum);
	else
		test_ub_log3(UBL_DEBUG, pnum);
	if(envsd->llevel==UBL_INFO){
		assert_string_equal(endmark, memoutbuf+v);
	}else{
		assert_string_equal(qstr, memoutbuf+v);
	}
}

static int teardown(void **state)
{
	ubb_memory_out_close();
	unibase_close();
	return 0;
}

int main(int argc, char *argv[])
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test_prestate_setup_teardown(test_ub_log_memout, setup,
							 teardown, NULL),
		cmocka_unit_test_prestate_setup_teardown(test_ub_log_memout_thread, setup,
							 teardown, NULL),
		cmocka_unit_test_prestate_setup_teardown(test_ub_log_memout_all, setup,
							 teardown, NULL),
		cmocka_unit_test_prestate_setup_teardown(test_ub_log_env_cat, setup_env1,
							 teardown, NULL),
		cmocka_unit_test_prestate_setup_teardown(test_ub_log_env_cat, setup_env2,
							 teardown, NULL),
		cmocka_unit_test_prestate_setup_teardown(test_ub_log_env_cat, setup_env3,
							 teardown, NULL),
		cmocka_unit_test_prestate_setup_teardown(test_ub_log_env_cat, setup_env4,
							 teardown, NULL),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
