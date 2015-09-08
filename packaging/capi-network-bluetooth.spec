#sbs-git:slp/api/bluetooth capi-network-bluetooth 0.1.0 686c444083e4197845c768e5dd034022b1dfa250
Name:       capi-network-bluetooth
Summary:    Network Bluetooth Framework
Version:    0.1.53
Release:    1
Group:      TO_BE/FILLED_IN
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz
Requires(post): eglibc
Requires(postun): eglibc

BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(vconf)
BuildRequires:  pkgconfig(bluetooth-api)
BuildRequires:  pkgconfig(capi-base-common)
%if "%{?tizen_profile_name}" == "wearable"
BuildRequires:  pkgconfig(privacy-manager-client)
%endif

BuildRequires:  cmake
BuildRequires:  model-build-features


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

%package gatt-test
Summary: Gatt Test Application
Group:   TO_BE/FILLED
Requires: %{name} = %{version}-%{release}

%description gatt-test
This package is C-API gatt-test application.

%package infinite-boot-test
Summary: Network Bluetooth Framework infinite reboot test application
Group:   TO_BE/FILLED

%description infinite-boot-test
This package is made to test BT on while infinte boot

%package infinite-spp-test-server
Summary: Network Bluetooth Framework infinite spp test application as server role
Group:   TO_BE/FILLED

%description infinite-spp-test-server
This package is made to test SPP throughput infintely as server role

%package infinite-spp-test-client
Summary: Network Bluetooth Framework infinite spp test application as client role
Group:   TO_BE/FILLED

%description infinite-spp-test-client
This package is made to test SPP throughput infintely as client role

%prep
%setup -q

%build
export CFLAGS="$CFLAGS -DTIZEN_DEBUG_ENABLE -DTIZEN_ENGINEER_MODE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_DEBUG_ENABLE -DTIZEN_ENGINEER_MODE"
export FFLAGS="$FFLAGS -DTIZEN_DEBUG_ENABLE -DTIZEN_ENGINEER_MODE"

%if "%{?tizen_profile_name}" == "wearable"
export CFLAGS="$CFLAGS -DTIZEN_WEARABLE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_WEARABLE"
export FFLAGS="$FFLAGS -DTIZEN_WEARABLE"
%endif

%if "%{?tizen_profile_name}" == "mobile"
export CFLAGS="$CFLAGS -DBT_ENABLE_LEGACY_GATT_CLIENT"
export CXXFLAGS="$CXXFLAGS -DBT_ENABLE_LEGACY_GATT_CLIENT"
export FFLAGS="$FFLAGS -DBT_ENABLE_LEGACY_GATT_CLIENT"
%endif

%if %{?model_build_feature_bt_disable_all}
export CFLAGS="$CFLAGS -DTIZEN_BT_DISABLE -DTIZEN_LE_DISABLE -DTIZEN_GATT_DISABLE
			-DTIZEN_OPP_SERVER_DISABLE -DTIZEN_OPP_CLIENT_DISABLE
			-DTIZEN_PAN_DISABLE -DTIZEN_HID_DISABLE -DTELEPHONY_DISABLED
			-DTIZEN_AVRCP_DISABLE -DTIZEN_HDP_DISABLE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_BT_DISABLE -DTIZEN_LE_DISABLE -DTIZEN_GATT_DISABLE
			-DTIZEN_OPP_SERVER_DISABLE -DTIZEN_OPP_CLIENT_DISABLE
			-DTIZEN_PAN_DISABLE -DTIZEN_HID_DISABLE -DTELEPHONY_DISABLED
			-DTIZEN_AVRCP_DISABLE -DTIZEN_HDP_DISABLE"
export FFLAGS="$FFLAGS -DTIZEN_BT_DISABLE -DTIZEN_LE_DISABLE -DTIZEN_GATT_DISABLE
			-DTIZEN_OPP_SERVER_DISABLE -DTIZEN_OPP_CLIENT_DISABLE
			-DTIZEN_PAN_DISABLE -DTIZEN_HID_DISABLE -DTELEPHONY_DISABLED
			-DTIZEN_AVRCP_DISABLE -DTIZEN_HDP_DISABLE"
%else
%if %{?model_build_feature_bt_disable_le}
export CFLAGS="$CFLAGS -DTIZEN_LE_DISABLE -DTIZEN_GATT_DISABLE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_LE_DISABLE -DTIZEN_GATT_DISABLE"
export FFLAGS="$FFLAGS -DTIZEN_LE_DISABLE -DTIZEN_GATT_DISABLE"
%endif

%if %{?model_build_feature_bt_disable_opp}
export CFLAGS="$CFLAGS -DTIZEN_OPP_SERVER_DISABLE -DTIZEN_OPP_CLIENT_DISABLE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_OPP_SERVER_DISABLE -DTIZEN_OPP_CLIENT_DISABLE"
export FFLAGS="$FFLAGS -DTIZEN_OPP_SERVER_DISABLE -DTIZEN_OPP_CLIENT_DISABLE"
%endif

%if %{?model_build_feature_bt_disable_tethering}
export CFLAGS="$CFLAGS -DTIZEN_PAN_DISABLE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_PAN_DISABLE"
export FFLAGS="$FFLAGS -DTIZEN_PAN_DISABLE"
%endif

%if %{?model_build_feature_bt_disable_hid}
export CFLAGS="$CFLAGS -DTIZEN_HID_DISABLE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_HID_DISABLE"
export FFLAGS="$FFLAGS -DTIZEN_HID_DISABLE"
%endif

%if %{?model_build_feature_bt_disable_audio_call}
export CFLAGS="$CFLAGS -DTELEPHONY_DISABLED -DTIZEN_HFP_DISABLE"
export CXXFLAGS="$CXXFLAGS -DTELEPHONY_DISABLED -DTIZEN_HFP_DISABLE"
export FFLAGS="$FFLAGS -DTELEPHONY_DISABLED -DTIZEN_HFP_DISABLE"
%endif

%if %{?model_build_feature_bt_disable_audio_media}
export CFLAGS="$CFLAGS -DTIZEN_AVRCP_DISABLE -DTIZEN_A2DP_DISABLE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_AVRCP_DISABLE -DTIZEN_A2DP_DISABLE"
export FFLAGS="$FFLAGS -DTIZEN_AVRCP_DISABLE -DTIZEN_A2DP_DISABLE"
%endif

%if %{?model_build_feature_bt_disable_health}
export CFLAGS="$CFLAGS -DTIZEN_HDP_DISABLE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_HDP_DISABLE"
export FFLAGS="$FFLAGS -DTIZEN_HDP_DISABLE"
%endif
%endif

%cmake \
%if "%{?tizen_profile_name}" == "wearable"
	-DTIZEN_WEARABLE=YES \
%else
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
install -D -m 0644 test_scripts/bt_on_test_while_infinite.service %{buildroot}%{_libdir}/systemd/system/bt_on_test_while_infinite.service
install -D -m 0644 test/bt_infinite_spp_test_server.service %{buildroot}%{_libdir}/systemd/system/bt_infinite_spp_test_server.service
install -D -m 0644 test/bt_infinite_spp_test_client.service %{buildroot}%{_libdir}/systemd/system/bt_infinite_spp_test_client.service

install -D -m 0755 test_scripts/check_bt_during_infinite_boot.sh %{buildroot}/%{_prefix}/etc/bluetooth/check_bt_during_infinite_boot.sh

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
/etc/smack/accesses2.d/capi.network.bluetooth.test.rule

%files gatt-test
%manifest bluetooth-gatt-test.manifest
%{_bindir}/bt_unit_test_gatt
/etc/smack/accesses2.d/capi.network.bluetooth.test.gatt.rule

%files infinite-boot-test
%{_libdir}/systemd/system/bt_on_test_while_infinite.service
%attr(755,-,-) %{_prefix}/etc/bluetooth/check_bt_during_infinite_boot.sh

%post infinite-boot-test
/usr/bin/systemctl daemon-reload
/usr/bin/systemctl enable bt_on_test_while_infinite.service

%preun infinite-boot-test
/usr/bin/systemctl disable bt_on_test_while_infinite.service

%files infinite-spp-test-server
%manifest bluetooth-infinite-spp-test-server.manifest
%{_bindir}/bt_infinite_spp_test
%{_libdir}/systemd/system/bt_infinite_spp_test_server.service
/etc/smack/accesses2.d/capi.network.bluetooth.infinite.spp.test.rule

%post infinite-spp-test-server
/usr/bin/systemctl daemon-reload
/usr/bin/systemctl enable bt_infinite_spp_test_server.service

%preun infinite-spp-test-server
/usr/bin/systemctl disable bt_infinite_spp_test_server.service

%files infinite-spp-test-client
%manifest bluetooth-infinite-spp-test-client.manifest
%{_bindir}/bt_infinite_spp_test
%{_libdir}/systemd/system/bt_infinite_spp_test_client.service
/etc/smack/accesses2.d/capi.network.bluetooth.infinite.spp.test.rule

%post infinite-spp-test-client
/usr/bin/systemctl daemon-reload
/usr/bin/systemctl enable bt_infinite_spp_test_client.service

%preun infinite-spp-test-client
/usr/bin/systemctl disable bt_infinite_spp_test_client.service

%files devel
%{_includedir}/network/bluetooth.h
%{_includedir}/network/bluetooth_type.h
%{_includedir}/network/bluetooth_internal.h
%{_includedir}/network/bluetooth_type_internal.h
%{_libdir}/pkgconfig/capi-network-bluetooth.pc
%{_libdir}/libcapi-network-bluetooth.so
%{_datadir}/license/capi-network-bluetooth-devel

