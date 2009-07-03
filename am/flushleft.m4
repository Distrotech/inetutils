dnl Copyright (C) 1996, 1997, 1998, 2002, 2004, 2005, 2007,
dnl 2009 Free Software Foundation, Inc.
dnl
dnl Mostly written by Miles Bader <miles@gnu.ai.mit.edu>
dnl
dnl This program is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 3, or (at your option)
dnl any later version.
dnl
dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl
dnl You should have received a copy of the GNU General Public License
dnl along with this program.  If not, see <http://www.gnu.org/licenses/>.
dnl

dnl IU_FLUSHLEFT -- remove all whitespace at the beginning of lines
dnl This is useful for c-code which may include cpp statements
dnl
AC_DEFUN([IU_FLUSHLEFT],
 [m4_changequote(`,')dnl
m4_bpatsubst(`$1', `^[ 	]+')
m4_changequote([,])])dnl
