#sbs-git:slp/api/bluetooth capi-network-bluetooth 0.1.0 686c444083e4197845c768e5dd034022b1dfa250
Name:       capi-network-bluetooth
Summary:    Network Bluetooth Framework
Version:    0.1.40
Release:    1
Group:      TO_BE/FILLED_IN
License:    Apache License, Version 2.0
Source0:    %{name}-%{version}.tar.gz
Requires(post): eglibc
Requires(postun): eglibc

BuildRequires:  pkgconfig(dbus-glib-1)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(vconf)
BuildRequires:  pkgconfig(bluetooth-api)
BuildRequires:  pkgconfig(capi-base-common)

BuildRequires:  cmake


%description
Network Bluetooth Framework

%package devel
Summary:    Network Bluetooth Framework (DEV)
Group:      TO_BE/FILLED
Requires:   %{name} = %{version}-%{release}

%description devel
Network Bluetooth Framework (DEV).

%prep
%setup -q

%build
MAJORVER=`echo %{version} | awk 'BEGIN {FS="."}{print $1}'`
%cmake . -DFULLVER=%{version} -DMAJORVER=${MAJORVER}

make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install


%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%manifest bluetooth.manifest
%{_libdir}/libcapi-network-bluetooth.so.*

%files devel
%{_includedir}/network/bluetooth.h
%{_libdir}/pkgconfig/capi-network-bluetooth.pc
%{_libdir}/libcapi-network-bluetooth.so

