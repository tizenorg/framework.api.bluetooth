#sbs-git:slp/api/bluetooth capi-network-bluetooth 0.1.0 686c444083e4197845c768e5dd034022b1dfa250
Name:       capi-network-bluetooth
Summary:    Network Bluetooth Framework
Version:    0.1.52
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
%if "%{?tizen_profile_name}" == "wearable"
BuildRequires:  pkgconfig(privacy-manager-client)
%endif

BuildRequires:  cmake


%description
Network Bluetooth Framework

%package devel
Summary:    Network Bluetooth Framework (DEV)
Group:      TO_BE/FILLED
Requires:   %{name} = %{version}-%{release}

%description devel
Network Bluetooth Framework (DEV).

%package test
Summary: Network Bluetooth Framework test application
Group:   TO_BE/FILLED
Requires: %{name} = %{version}-%{release}

%description test
This package is C-API test application.

%prep
%setup -q

%build
%if "%{?tizen_profile_name}" == "wearable"
export CFLAGS="$CFLAGS -DTIZEN_WEARABLE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_WEARABLE"
export FFLAGS="$FFLAGS -DTIZEN_WEARABLE"
%endif

#%if 0%{?sec_build_binary_debug_enable}
export CFLAGS="$CFLAGS -DTIZEN_DEBUG_ENABLE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_DEBUG_ENABLE"
export FFLAGS="$FFLAGS -DTIZEN_DEBUG_ENABLE"
#%endif

#%if 0%{?tizen_build_binary_release_type_eng}
export CFLAGS="$CFLAGS -DTIZEN_ENGINEER_MODE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_ENGINEER_MODE"
export FFLAGS="$FFLAGS -DTIZEN_ENGINEER_MODE"
#%endif

%if "%{?tizen_profile_name}" == "wearable"
export CFLAGS+=" -DTELEPHONY_DISABLED"
export CXXFLAGS+=" -DTELEPHONY_DISABLED"
export FFLAGS+=" -DTELEPHONY_DISABLED"
%endif

%cmake \
%if "%{?tizen_profile_name}" == "wearable"
	-DTIZEN_WEARABLE=YES \
%elseif "%{?tizen_profile_name}" == "mobile"
	-DTIZEN_WEARABLE=NO \
%endif

MAJORVER=`echo %{version} | awk 'BEGIN {FS="."}{print $1}'`
cmake . -DCMAKE_INSTALL_PREFIX=/usr -DFULLVER=%{version} -DMAJORVER=${MAJORVER}

make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install
install -D -m 0644 LICENSE %{buildroot}%{_datadir}/license/capi-network-bluetooth
install -D -m 0644 LICENSE %{buildroot}%{_datadir}/license/capi-network-bluetooth-devel

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%manifest bluetooth.manifest
%{_libdir}/libcapi-network-bluetooth.so.*
%{_datadir}/license/capi-network-bluetooth

%files test
%manifest bluetooth-test.manifest
%{_bindir}/bt_unit_test
%{_bindir}/bt_onoff
/etc/smack/accesses.d/capi-network-bluetooth-test.efl

%files devel
%{_includedir}/network/bluetooth.h
%{_includedir}/network/bluetooth_type.h
%{_libdir}/pkgconfig/capi-network-bluetooth.pc
%{_libdir}/libcapi-network-bluetooth.so
%{_datadir}/license/capi-network-bluetooth-devel

