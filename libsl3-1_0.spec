#
# spec file for package libsl3
#
# Copyright (c) 2016 Harald Achitz <harald.achitz@gmail.com>
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

# Please submit bugfixes or comments via http://bugs.opensuse.org/
#


%define  major_version 1
%define  patch_version 0
%define  minor_version 13000

%define  libname libsl3
%define  so_name %{major_version}_%{patch_version}

%define  file_version %{major_version}.%{patch_version}.%{minor_version}

%define  archive_name %{libname}-%{file_version}

Name:       %{libname}-%{so_name}
Version:    %{minor_version}
Release:    1%{?dist}
Summary:    The convenient modern C++ interface for SQLite 3.x

Group:      Development/Libraries
License:    MPL-2.0
URL:        http://a4z.bitbucket.org/docs/libsl3/html/index.html
Source0:    https://bitbucket.org/a4z/%{libname}/downloads/%{archive_name}.tar.gz

BuildRequires:  gcc-c++
BuildRequires:  boost-devel
BuildRequires:  cmake
BuildRequires:  doxygen
BuildRequires:  graphviz


%description
libsl3 is the convenient modern C++ interface for SQLite 3.x.
The library is designed to enable a comfortable and efficient communication with
SQLite 3.x database based based on it's natural language, which is SQL.


%package devel
Summary:        Development files for %{name}
Group:          Development/Libraries
Requires:       %{name} = %{version}-%{release}

%description devel
This package contains development files for %{name}.

%package doc
BuildArch:      noarch
Summary:        Development documentation for %{name}
Group:          Development/Libraries

%description doc
This package contains development documentation files for %{name}.

%prep
%setup -qn %{archive_name}

%build
%cmake -Dsl3_BUILD_TESTS=OFF -Dsl3_BUILD_SHARED_LIB=ON -Dsl3_BUILD_STATIC_LIB=ON \
 -DCMAKE_CXX_FLAGS_NONE="-std=c++11 -pthread -DNDEBUG"
make %{?_smp_mflags} all doc


%install
%cmake_install


%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%files doc
%doc build/doc/html

%files devel
%{_libdir}/libsl3.so
%{_includedir}/sl3
%{_libdir}/libsl3.a

%files
%doc README.md 
%{_libdir}/libsl3.so.*

%changelog
* Mon Jul  4 2016 Harald Achitz <harald.achitz@gmail.com> - 0.1.0
- Initial package

 
