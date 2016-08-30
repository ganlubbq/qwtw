Name:           qwtw
Version:    	1.0.1    
Release:        1%{?dist}
Summary:       2D plotting library 

License:       LGPLv2 
URL:            https://github.com/ig-or/qwtw
Source0:       https://github.com/ig-or/qwtw.tar.gz

BuildRequires:  libtool, cmake >= 3.5.0, gcc-c++, python
BuildRequires:	qt5-qt3d-devel, qt5-qtsvg-devel, qt5-qtbase-devel >= 5.6.1, qt5-qtwebkit-devel
BuildRequires:	qt5-qtwebengine-devel, qt5-qtwebchannel-devel, qt5-qtwebsockets-devel
BuildRequires:  qt5-qtxmlpatterns-devel
BuildRequires:  qt5-qtxmlpatterns-devel
BuildRequires:  marble-astro-devel,  marble-widget-qt5-devel
BuildRequires:  qwt-qt5 >= 6.1.2, qwt-qt5-devel
 
Requires:       qt5-qtbase >= 5.6.1, qt5-qtbase-gui, qwt-qt5 >= 6.1.2, marble-astro, marble-widget-qt5

%description 
qwt-based 2D plotting library


%package        devel
Summary:        Development files for %{name}
Requires:       %{name}%{?_isa} = %{version}-%{release}

%description    devel
The %{name}-devel package contains libraries and header files for
developing applications that use %{name}.


%prep
%autosetup


%build
cd qwtw/c_lib
rm -R -f build
mkdir build
cd build
rm -R -f *
PATH=%{_libdir}/qt5/bin:$PATH
%cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release  ../.
make  %{?_smp_mflags}

%install
rm -rf $RPM_BUILD_ROOT
cd qwtw/c_lib/build
make install DESTDIR=%{buildroot}

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig


%files
%license LICENSE
%doc README.md
%{_libdir}/*.so.*

%files devel
%doc README.md
%{_includedir}/*
%{_libdir}/*.so


%changelog
* Mon Aug 29 2016 makerpm
- 
