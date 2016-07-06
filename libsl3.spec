Name:       libsl3
Version:    0.1.0
Release:    1%{?dist}
Summary:    The convenient modern C++ interface for SQLite 3.x

Group:      Development/Libraries
License:    MPL-2.0
URL:        http://a4z.bitbucket.org/docs/libsl3/html/index.html
Source0:    https://bitbucket.org/a4z/%{name}/downloads/%{name}-%{version}.tar.gz

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
%setup -q

%build
%cmake . -Dsl3_BUILD_TESTS=OFF -Dsl3_BUILD_SHARED_LIB=ON -Dsl3_BUILD_STATIC_LIB=ON \
 -DCMAKE_CXX_FLAGS_RELWITHDEBINFO="-std=c++11 -DNDEBUG"
make %{?_smp_mflags} all doc


%install
make DESTDIR=%{buildroot} install

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%files doc
%doc doc/html

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

