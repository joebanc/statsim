dnl ### begin block 00_header[./mysql++.bkl] ###
dnl
dnl This macro was generated by
dnl Bakefile 0.2.3 (http://www.bakefile.org)
dnl Do not modify, all changes will be overwritten!

BAKEFILE_AUTOCONF_INC_M4_VERSION="0.2.3"

dnl ### begin block 20_COND_BUILD_DEBUG[./mysql++.bkl] ###
    COND_BUILD_DEBUG="#"
    if test "x$BUILD" = "xdebug" ; then
        COND_BUILD_DEBUG=""
    fi
    AC_SUBST(COND_BUILD_DEBUG)
dnl ### begin block 20_COND_BUILD_RELEASE[./mysql++.bkl] ###
    COND_BUILD_RELEASE="#"
    if test "x$BUILD" = "xrelease" ; then
        COND_BUILD_RELEASE=""
    fi
    AC_SUBST(COND_BUILD_RELEASE)
dnl ### begin block 20_COND_DEPS_TRACKING_0[./mysql++.bkl] ###
    COND_DEPS_TRACKING_0="#"
    if test "x$DEPS_TRACKING" = "x0" ; then
        COND_DEPS_TRACKING_0=""
    fi
    AC_SUBST(COND_DEPS_TRACKING_0)
dnl ### begin block 20_COND_DEPS_TRACKING_1[./mysql++.bkl] ###
    COND_DEPS_TRACKING_1="#"
    if test "x$DEPS_TRACKING" = "x1" ; then
        COND_DEPS_TRACKING_1=""
    fi
    AC_SUBST(COND_DEPS_TRACKING_1)
dnl ### begin block 20_COND_PLATFORM_MACOSX_0_USE_SOVERCYGWIN_0_USE_SOVERSION_1[./mysql++.bkl] ###
    COND_PLATFORM_MACOSX_0_USE_SOVERCYGWIN_0_USE_SOVERSION_1="#"
    if test "x$PLATFORM_MACOSX" = "x0" -a "x$USE_SOVERCYGWIN" = "x0" -a "x$USE_SOVERSION" = "x1" ; then
        COND_PLATFORM_MACOSX_0_USE_SOVERCYGWIN_0_USE_SOVERSION_1=""
    fi
    AC_SUBST(COND_PLATFORM_MACOSX_0_USE_SOVERCYGWIN_0_USE_SOVERSION_1)
dnl ### begin block 20_COND_PLATFORM_MACOSX_0_USE_SOVERSION_1[./mysql++.bkl] ###
    COND_PLATFORM_MACOSX_0_USE_SOVERSION_1="#"
    if test "x$PLATFORM_MACOSX" = "x0" -a "x$USE_SOVERSION" = "x1" ; then
        COND_PLATFORM_MACOSX_0_USE_SOVERSION_1=""
    fi
    AC_SUBST(COND_PLATFORM_MACOSX_0_USE_SOVERSION_1)
dnl ### begin block 20_COND_PLATFORM_MACOSX_1[./mysql++.bkl] ###
    COND_PLATFORM_MACOSX_1="#"
    if test "x$PLATFORM_MACOSX" = "x1" ; then
        COND_PLATFORM_MACOSX_1=""
    fi
    AC_SUBST(COND_PLATFORM_MACOSX_1)
dnl ### begin block 20_COND_PLATFORM_MACOSX_1_USE_SOVERSION_1[./mysql++.bkl] ###
    COND_PLATFORM_MACOSX_1_USE_SOVERSION_1="#"
    if test "x$PLATFORM_MACOSX" = "x1" -a "x$USE_SOVERSION" = "x1" ; then
        COND_PLATFORM_MACOSX_1_USE_SOVERSION_1=""
    fi
    AC_SUBST(COND_PLATFORM_MACOSX_1_USE_SOVERSION_1)
dnl ### begin block 20_COND_PLATFORM_MAC_0[./mysql++.bkl] ###
    COND_PLATFORM_MAC_0="#"
    if test "x$PLATFORM_MAC" = "x0" ; then
        COND_PLATFORM_MAC_0=""
    fi
    AC_SUBST(COND_PLATFORM_MAC_0)
dnl ### begin block 20_COND_PLATFORM_MAC_1[./mysql++.bkl] ###
    COND_PLATFORM_MAC_1="#"
    if test "x$PLATFORM_MAC" = "x1" ; then
        COND_PLATFORM_MAC_1=""
    fi
    AC_SUBST(COND_PLATFORM_MAC_1)
dnl ### begin block 20_COND_PLATFORM_OS2_1[./mysql++.bkl] ###
    COND_PLATFORM_OS2_1="#"
    if test "x$PLATFORM_OS2" = "x1" ; then
        COND_PLATFORM_OS2_1=""
    fi
    AC_SUBST(COND_PLATFORM_OS2_1)
dnl ### begin block 20_COND_USE_SOSYMLINKS_1[./mysql++.bkl] ###
    COND_USE_SOSYMLINKS_1="#"
    if test "x$USE_SOSYMLINKS" = "x1" ; then
        COND_USE_SOSYMLINKS_1=""
    fi
    AC_SUBST(COND_USE_SOSYMLINKS_1)
dnl ### begin block 20_COND_USE_SOVERCYGWIN_1_USE_SOVERSION_1[./mysql++.bkl] ###
    COND_USE_SOVERCYGWIN_1_USE_SOVERSION_1="#"
    if test "x$USE_SOVERCYGWIN" = "x1" -a "x$USE_SOVERSION" = "x1" ; then
        COND_USE_SOVERCYGWIN_1_USE_SOVERSION_1=""
    fi
    AC_SUBST(COND_USE_SOVERCYGWIN_1_USE_SOVERSION_1)
dnl ### begin block 20_COND_USE_SOVERLINUX_1[./mysql++.bkl] ###
    COND_USE_SOVERLINUX_1="#"
    if test "x$USE_SOVERLINUX" = "x1" ; then
        COND_USE_SOVERLINUX_1=""
    fi
    AC_SUBST(COND_USE_SOVERLINUX_1)
dnl ### begin block 20_COND_USE_SOVERSION_0[./mysql++.bkl] ###
    COND_USE_SOVERSION_0="#"
    if test "x$USE_SOVERSION" = "x0" ; then
        COND_USE_SOVERSION_0=""
    fi
    AC_SUBST(COND_USE_SOVERSION_0)
dnl ### begin block 20_COND_USE_SOVERSOLARIS_1[./mysql++.bkl] ###
    COND_USE_SOVERSOLARIS_1="#"
    if test "x$USE_SOVERSOLARIS" = "x1" ; then
        COND_USE_SOVERSOLARIS_1=""
    fi
    AC_SUBST(COND_USE_SOVERSOLARIS_1)
dnl ### begin block 20_COND_WINDOWS_IMPLIB_1[./mysql++.bkl] ###
    COND_WINDOWS_IMPLIB_1="#"
    if test "x$WINDOWS_IMPLIB" = "x1" ; then
        COND_WINDOWS_IMPLIB_1=""
    fi
    AC_SUBST(COND_WINDOWS_IMPLIB_1)
