Summary:    C++ wrapper for the MySQL C API
Name:       mysql++
Version:    3.0.6
Release:    1%{?dist}
License:    LGPL
Group:      Development/Libraries
URL:        http://tangentsoft.net/mysql++/
Source0:    http://tangentsoft.net/mysql++/releases/mysql++-%{version}.tar.gz
BuildRoot:  %(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)
BuildRequires: mysql-devel

%description
MySQL++ makes working with MySQL server queries as easy as working
with STL containers.  This package contains only the libraries needed to
run MySQL++-based programs.  If you are building your own MySQL++-based
programs, you also need to install the -devel package.

%package devel
Summary:   MySQL++ developer files (headers, examples, etc.)
Group:     Development/Libraries
Requires:  mysql++ = %{version}-%{release}, mysql-devel

%description devel
These are the files needed to compile MySQL++ based programs, plus
some sample code to get you started.  If you aren't building your own
programs, you probably don't need to install this package.

%package manuals
Summary:   MySQL++ user and reference manuals
Group:     Development/Libraries

%description manuals
This is the MySQL++ documentation.  It's a separate RPM just because
it's so large, and it doesn't change with every release.

%prep
%setup -q

# Fakery for nodoc case
if [ ! -e doc/README-devel-RPM.txt ]
then
	touch doc/README-devel-RPM.txt
	touch doc/README-doc-RPM.txt
	touch doc/README-manuals-RPM.txt
	%{__mkdir_p} doc/html doc/pdf
fi

%build
%configure --disable-dependency-tracking
%{__make} %{?_smp_mflags}

%install
rm -rf %{buildroot} doc/examples

%{__mkdir_p} %{buildroot}{%{_libdir},%{_includedir}}

%{__make} DESTDIR=%{buildroot} install

# Copy example programs to doc directory
%{__mkdir_p} doc/examples
%{__install} -m644 examples/*.{cpp,h} doc/examples/
%{__install} -m644 config.h doc/examples/
sed -i -e s@../config.h@config.h@ doc/examples/threads.h

# Fix up simple example Makefile to allow it to build on the install
# system, as opposed to the system where the Makefile was created.
%{__sed} -e 's@./examples/@@' \
  -e 's@^CPPFLAGS :=.*$@CPPFLAGS := $(shell mysql_config --cflags)@' \
  -e 's@^LDFLAGS :=.*$@LDFLAGS := $(shell mysql_config --libs)@' \
  -e 's@ -Ilib@@' \
  -e '/^all:/s/test_[a-z,_]* //g' \
  Makefile.simple > doc/examples/Makefile

%clean
rm -rf %{buildroot} doc/examples

%post -p /sbin/ldconfig 

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%doc ChangeLog COPYING.txt CREDITS.txt LICENSE.txt README.txt
%{_libdir}/libmysqlpp.so.*

%files devel
%defattr(-,root,root,-)
%doc doc/examples doc/README-devel-RPM.txt README-examples.txt Wishlist
%{_includedir}/mysql++
%{_libdir}/libmysqlpp.so

%files manuals
%defattr(-,root,root,-)
%doc doc/html doc/pdf doc/README-manuals-RPM.txt

%changelog
* Mon Jul 2 2007 Warren Young <mysqlpp@etr-usa.com> 2.3.0-1
- Reflected changes to doc dir layout in manuals sub-package

* Mon Mar 19 2007 Warren Young <mysqlpp@etr-usa.com> 2.2.1-4
- Reorganized locations of generated documentation

* Mon Mar 19 2007 Warren Young <mysqlpp@etr-usa.com> 2.2.1-3
- Merge of Remi Collet's spec file with official one

* Sun Mar 18 2007 Remi Collet <rpms@FamilleCollet.com> 2.2.1-2
- find perm on common.h
- soname mysql++-2.2.1-bkl.patch

* Wed Feb 28 2007 Remi Collet <rpms@FamilleCollet.com> 2.2.1-1
- Initial spec for Extras

* Wed Feb 28 2007 Remi Collet <rpms@FamilleCollet.com> 2.2.1-1.fc{3-6}.remi
- update to version 2.2.1

* Thu Jan 25 2007 Remi Collet <rpms@FamilleCollet.com> 2.2.0-1.fc{3-6}.remi
- update to version 2.2.0

* Mon Nov 13 2006 Remi Collet <rpms@FamilleCollet.com> 2.1.1.fc6.remi
- FC6.x86_64 build
- dynamic (sed) patch for Makefile (use mysql_config)

* Thu Nov 02 2006 Remi Collet <rpms@FamilleCollet.com> 2.1.1.fc6.remi
- FC6 build

* Sat Apr  8 2006 Remi Collet <rpms@FamilleCollet.com> 2.1.1.fc{3,4,5}.remi
- update to version 2.1.1

* Sat Nov 26 2005 Remi Collet <remi.collet@univ-reims.fr> 2.0.7-1.fc3.remi - 2.0.7-1.fc4.remi
- update to version 2.0.4
- build with mysql-5.0.15 (requires libmysqlclient.so.15)

* Sun Sep  4 2005 Remi Collet <remi.collet@univ-reims.fr> 2.0.4-1.FC4.remi
- version 2.0.4

* Sat Aug 20 2005 Remi Collet <remi.collet@univ-reims.fr> 2.0.2-1.FC4.remi
- built for FC4
- spec cleanning...

* Thu Jun 16 2005 Remi Collet <Remi.Collet@univ-reims.fr> 1.7.40-1.FC3.remi
- built for FC3 and MySQL 4.1.11
- examples in /usr/share/doc/mysql++-%%{version}/examples

* Sat Apr 30 2005 Warren Young <mysqlpp@etr-usa.com> 1.7.34-1
- Split manuals out into their own sub-package.

* Thu Mar 10 2005 Warren Young <mysqlpp@etr-usa.com> 1.7.32-1
- Disabled building of examples, to speed RPM build.

* Fri Nov 05 2004 Warren Young <mysqlpp@etr-usa.com> 1.7.21-1
- Split out -devel subpackage, which now includes the examples

* Wed Aug 18 2004 Warren Young <mysqlpp@etr-usa.com> 1.7.11-1
- Removed examples from documentation.
- Limited documentation to just the generated files, not the sources.

* Wed Apr 16 2003 Tuan Hoang <tqhoang@bigfoot.com> 1.7.9-4
- Added gcc 3.2.2 patch.
- Packaged using Red Hat Linux 8.0 and 9.

* Thu Nov 14 2002 Tuan Hoang <tqhoang@bigfoot.com> 1.7.9-3
- Changed the version passed to libtool.

* Mon Oct 28 2002 Tuan Hoang <tqhoang@bigfoot.com> 1.7.9-2
- Updated the version numbering of the library to be 1.7.9.
- Packaged using Red Hat Linux 8.0.

* Thu Oct 17 2002 Philipp Berndt <philipp.berndt@gmx.net>
- packaged
