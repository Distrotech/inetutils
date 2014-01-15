#
# Copyright (C) 2009, 2010, 2011, 2012, 2013, 2014 Free Software
# Foundation, Inc.
#
# This file is part of GNU Inetutils.
#
# GNU Inetutils is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or (at
# your option) any later version.
#
# GNU Inetutils is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see `http://www.gnu.org/licenses/'.

news-check-regexp = '^Version $(VERSION_REGEXP):'

_makefile_at_at_check_exceptions = ' && !/PATHDEFS_MAKE/'

_build-aux = build-aux

local-checks-to-skip = \
	sc_cast_of_x_alloc_return_value \
	sc_copyright_check \
	sc_program_name \
	sc_prohibit_always_true_header_tests \
	sc_prohibit_assert_without_use \
	sc_prohibit_doubled_word \
	sc_prohibit_error_without_use \
	sc_prohibit_have_config_h \
	sc_prohibit_magic_number_exit \
	sc_prohibit_strncpy \
	sc_prohibit_undesirable_word_seq \
	sc_prohibit_xalloc_without_use \
	sc_error_message_period \
	sc_error_message_uppercase \
	sc_immutable_NEWS \
	sc_m4_quote_check \
	sc_prohibit_atoi_atof \
	sc_prohibit_stat_st_blocks \
	sc_prohibit_strcmp \
	sc_unmarked_diagnostics \
	sc_bindtextdomain \
	sc_assignment_in_if

exclude_file_name_regexp--sc_prohibit_have_config_h = \
	^libinetutils/libinetutils.h$$

htmldir = ../www-$(PACKAGE)

manual_title = GNU network utilities

web-coverage:
	rm -fv `find $(htmldir)/coverage -type f | grep -v CVS`
	cp -rv doc/coverage/* $(htmldir)/coverage/

upload-web-coverage:
	cd $(htmldir) && \
		cvs commit -m "Update." coverage

release-prep-hook =

sc_unsigned_char:
	@prohibit=u''_char \
	halt='don'\''t use u''_char; instead use unsigned char'	\
	  $(_sc_search_regexp)

sc_unsigned_long:
	@prohibit=u''_long \
	halt='don'\''t use u''_long; instead use unsigned long'	\
	  $(_sc_search_regexp)

sc_unsigned_short:
	@prohibit=u''_short \
	halt='don'\''t use u''_char; instead use unsigned short' \
	  $(_sc_search_regexp)

sc_assignment_in_if:
	prohibit='\<if *\(.* = ' halt="don't use assignments in if statements"	\
	  $(_sc_search_regexp)
