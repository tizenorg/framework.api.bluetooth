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
%if %{_repository}=="wearable"
%if 0%{?tizen_build_binary_release_type_eng}
export CFLAGS="$CFLAGS -DTIZEN_ENGINEER_MODE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_ENGINEER_MODE"
export FFLAGS="$FFLAGS -DTIZEN_ENGINEER_MODE"
%endif
%endif
MAJORVER=`echo %{version} | awk 'BEGIN {FS="."}{print $1}'`

%if %{_repository}=="wearable"
cd wearable
cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix} -DFULLVER=%{version} -DMAJORVER=${MAJORVER}
%elseif %{_repository}=="mobile"
cd mobile
%cmake . -DFULLVER=%{version} -DMAJORVER=${MAJORVER}
%endif

make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}

%if %{_repository}=="wearable"
cd wearable
%elseif %{_repository}=="mobile"
cd mobile
%endif

%make_install
%if %{_repository}=="wearable"
install -D -m 0644 LICENSE.APLv2 %{buildroot}%{_datadir}/license/capi-network-bluetooth
install -D -m 0644 LICENSE.APLv2 %{buildroot}%{_datadir}/license/capi-network-bluetooth-devel
%elseif %{_repository}=="mobile"
mkdir -p %{buildroot}/usr/share/license
cp LICENSE.APLv2 %{buildroot}/usr/share/license/%{name}
%endif

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%if %{_repository}=="wearable"
%manifest wearable/bluetooth.manifest
%{_libdir}/libcapi-network-bluetooth.so.*
%{_datadir}/license/capi-network-bluetooth
%{_bindir}/bt_unit_test
%elseif %{_repository}=="mobile"
%manifest wearable/bluetooth.manifest
%{_libdir}/libcapi-network-bluetooth.so.*
/usr/share/license/%{name}
%endif

%files devel
%{_includedir}/network/bluetooth.h
%{_libdir}/pkgconfig/capi-network-bluetooth.pc
%{_libdir}/libcapi-network-bluetooth.so
%if %{_repository}=="wearable"
%{_datadir}/license/capi-network-bluetooth-devel
%endif

