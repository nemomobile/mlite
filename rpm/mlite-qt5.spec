# 
# Do NOT Edit the Auto-generated Part!
# Generated by: spectacle version 0.27
# 

Name:       mlite-qt5

# >> macros
# << macros

Summary:    Useful classes originating from MeeGo Touch
Version:    0.2.2
Release:    1
Group:      System/Libraries
License:    LGPL v2.1
URL:        http://www.meego.com
Source0:    %{name}-%{version}.tar.bz2
Source100:  mlite-qt5.yaml
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
BuildRequires:  pkgconfig(Qt5DBus)
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Test)
BuildRequires:  pkgconfig(gconf-2.0)

%description
Select set of useful classes from meegotouch that do not require
taking in all of meegotouch.


%package devel
Summary:    mlite development package
Group:      System/Libraries
Requires:   %{name} = %{version}-%{release}

%description devel
Development files needed for using the mlite library


%package tests
Summary:    Test suite for mlite-qt5
Group:      System/Libraries
Requires:   %{name} = %{version}-%{release}

%description tests
Test suite for mlite-qt5.


%prep
%setup -q -n %{name}-%{version}

# >> setup
# << setup

%build
# >> build pre
export QT_SELECT=5
# << build pre

%qmake5  \
    VERSION=%{version}

make %{?jobs:-j%jobs}

# >> build post
# << build post

%install
rm -rf %{buildroot}
# >> install pre
# << install pre
%qmake5_install

# >> install post
# << install post

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%{_libdir}/libmlite5.so.*
# >> files
# << files

%files devel
%defattr(-,root,root,-)
%{_includedir}/mlite5
%{_libdir}/pkgconfig/*.pc
%{_libdir}/libmlite5.so
# >> files devel
# << files devel

%files tests
%defattr(-,root,root,-)
/opt/tests/mlite-qt5/*
# >> files tests
# << files tests
