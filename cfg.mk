#
# Copyright (C) 2009, 2010 Free Software Foundation, Inc.
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

local-checks-to-skip = \
	sc_error_message_period \
	sc_error_message_uppercase \
	sc_immutable_NEWS \
	sc_m4_quote_check \
	sc_obsolete_symbols \
	sc_program_name \
	sc_prohibit_assert_without_use \
	sc_prohibit_atoi_atof \
	sc_prohibit_have_config_h \
	sc_prohibit_magic_number_exit \
	sc_prohibit_signal_without_use \
	sc_prohibit_stat_st_blocks \
	sc_prohibit_strcmp \
	sc_space_tab \
	sc_trailing_blank \
	sc_unmarked_diagnostics \
	sc_useless_cpp_parens \
	sc_makefile_check

htmldir = ../www-$(PACKAGE)

web-coverage:
	rm -fv `find $(htmldir)/coverage -type f | grep -v CVS`
	cp -rv doc/coverage/* $(htmldir)/coverage/

upload-web-coverage:
	cd $(htmldir) && \
		cvs commit -m "Update." coverage

release-prep-hook =
