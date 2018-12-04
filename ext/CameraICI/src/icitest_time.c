////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2018 Intel Corporation
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom
// the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
// OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
// OR OTHER DEALINGS IN THE SOFTWARE.
//
// SPDX-License-Identifier: MIT
//
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include "icitest_time.h"

static double clock_diff(struct timespec startTime, struct timespec endTime)
{
	struct timespec diff;
	if ((endTime.tv_nsec - startTime.tv_nsec) >= 0)
	{
		diff.tv_sec = endTime.tv_sec - startTime.tv_sec;
		diff.tv_nsec = endTime.tv_nsec - startTime.tv_nsec;
	} else {
		diff.tv_sec = endTime.tv_sec - startTime.tv_sec - 1;
		diff.tv_nsec = 1000000000 + endTime.tv_nsec - startTime.tv_nsec;
	}
	return diff.tv_sec * 1000 + (double)(diff.tv_nsec) / 1000000;
}

static void print_time_measurement(const char* name,
		struct timespec start, struct timespec end)
{
	double diff;
	double ts;
	diff = clock_diff(start, end);
	ts = end.tv_sec + (double)end.tv_nsec/1000000000;

	printf("%-25s | %6.03f s | %6.02f ms\n", name, ts, diff);
}

void print_time_measurements()
{
	printf("IPUICI TEST TIME STATS\n");
	printf("%-25s | %-10s | %-6s\n", "Tracepoint",
			"System ts", "Time since app start");

	print_time_measurement("App start",
			time_measurements.app_start_time,
			time_measurements.app_start_time);
	print_time_measurement("IPU streamon",
			time_measurements.app_start_time,
			time_measurements.streamon_time);
	print_time_measurement("Weston ready",
			time_measurements.app_start_time,
			time_measurements.weston_init_time);
	print_time_measurement("EGL/GL setup",
			time_measurements.app_start_time,
			time_measurements.rendering_init_time);
	print_time_measurement("First frame received",
			time_measurements.app_start_time,
			time_measurements.first_frame_time);
	print_time_measurement("First frame displayed",
			time_measurements.app_start_time,
			time_measurements.first_frame_rendered_time);
}
