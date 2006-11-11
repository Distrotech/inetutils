/* Copyright (C) 2004, 2006 Free Software Foundation, Inc.

   This file is part of GNU Inetutils.

   GNU Inetutils is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   GNU Inetutils is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GNU Inetutils; see the file COPYING.  If not, write
   to the Free Software Foundation, Inc., 51 Franklin Street,
   Fifth Floor, Boston, MA 02110-1301 USA. */

#define DEFAULT_PING_COUNT 4

void tvsub (struct timeval *out, struct timeval *in);
double nabs (double a);
double nsqrt (double a, double prec);

void show_license (void);

size_t ping_cvt_number (const char *optarg, size_t maxval, int allow_zero);

void init_data_buffer (unsigned char *pat, int len);

void decode_pattern (const char *text, int *pattern_len,
		     unsigned char *pattern_data);
