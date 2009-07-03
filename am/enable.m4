dnl Copyright (C) 1996, 1997, 1998, 2002, 2004, 2005, 2007,
dnl 2009 Free Software Foundation, Inc.
dnl
dnl Written Joel N. Weber II <devnull@gnu.org>.
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

AC_DEFUN([IU_ENABLE_FOO],
 [AC_ARG_ENABLE($1, AS_HELP_STRING([--disable-$1], [don't compile $1]), ,
                [enable_]$1[=$enable_]$2)
[if test "$enable_$1" = yes; then 
   $1_BUILD=$1
   $1_INSTALL_HOOK="install-$1-hook"
else
   $1_BUILD=''
   $1_INSTALL_HOOK=''
fi;]
  AC_SUBST([$1_BUILD])
  AC_SUBST([$1_INSTALL_HOOK])
])

AC_DEFUN([IU_ENABLE_CLIENT], [IU_ENABLE_FOO($1, clients)])
AC_DEFUN([IU_ENABLE_SERVER], [IU_ENABLE_FOO($1, servers)])
