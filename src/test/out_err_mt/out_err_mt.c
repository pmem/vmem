/*
 * Copyright 2015-2019, Intel Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *
 *     * Neither the name of the copyright holder nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * out_err_mt.c -- unit test for error messages
 */

#include <sys/types.h>
#include <stdarg.h>
#include <errno.h>
#include "unittest.h"
#include "valgrind_internal.h"
#include "util.h"

#define NUM_THREADS 16

static void
print_errors(const char *msg)
{
	UT_OUT("%s", msg);
	UT_OUT("VMEM: %s", vmem_errormsg());
}

static void
check_errors(unsigned ver)
{
	int ret;
	int err_need;
	int err_found;

	ret = sscanf(vmem_errormsg(),
		"libvmem major version mismatch (need %d, found %d)",
		&err_need, &err_found);
	UT_ASSERTeq(ret, 2);
	UT_ASSERTeq(err_need, ver);
	UT_ASSERTeq(err_found, VMEM_MAJOR_VERSION);
}

static void *
do_test(void *arg)
{
	unsigned ver = *(unsigned *)arg;

	vmem_check_version(ver, 0);
	check_errors(ver);

	return NULL;
}

static void
run_mt_test(void *(*worker)(void *))
{
	os_thread_t thread[NUM_THREADS];
	unsigned ver[NUM_THREADS];

	for (unsigned i = 0; i < NUM_THREADS; ++i) {
		ver[i] = 10000 + i;
		PTHREAD_CREATE(&thread[i], NULL, worker, &ver[i]);
	}
	for (unsigned i = 0; i < NUM_THREADS; ++i) {
		PTHREAD_JOIN(&thread[i], NULL);
	}
}

int
main(int argc, char *argv[])
{
	START(argc, argv, "out_err_mt");

	if (argc != 2)
		UT_FATAL("usage: %s dir", argv[0]);

	print_errors("start");

	VMEM *vmp = vmem_create(argv[1], VMEM_MIN_POOL);
	UT_ASSERT(vmp);

	vmem_check_version(10005, 0);
	print_errors("version check");

	VMEM *vmp2 = vmem_create_in_region(NULL, 1);
	UT_ASSERTeq(vmp2, NULL);
	print_errors("vmem_create_in_region");

	run_mt_test(do_test);

	vmem_delete(vmp);

	DONE(NULL);
}
