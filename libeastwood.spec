%define	major	0
%define	libname %mklibname eastwood %{major}
%define	libdev	%mklibname -d eastwood %{major}

Summary:	Game data library
Name:		libeastwood
Version:	0.1
Release:	1
License:	LGPLv2.1
Group:		System/Libraries
URL:		http://launchpad.net/doonlunacy
Source0:	%{name}-%{version}.tar.xz
BuildRequires:	SDL-devel SDL_mixer-devel libsamplerate-devel
BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}-buildroot

%description
This library provides support for handling wsa, shp, icn, cps, pal, adl, voc,
pak++ data files used in some games (mainly targetting Dune 2).

%package -n	%{libname}
Summary:	Game data library
Group:		System/Libraries
Provides:	%{name}-devel = %{version}-%{release}

%description -n	%{libname}
This library provides support for handling wsa, shp, icn, cps, pal, adl, voc,
pak++ data files used in some games (mainly targetting Dune 2).

%package -n	%{libdev}
Summary:	Development files and headers for %{name}
Group:		Development/C

%description -n	%{libdev}
This package contains the headers needed to build applications with %{name}.

%prep
%setup -q

%build
%cmake
%make VERBOSE=1

%install
rm -rf %{buildroot}
cd build
%makeinstall_std

%clean
rm -rf %{buildroot}

%files -n %{libname}
%defattr(-,root,root)
%{_libdir}/libeastwood.so.%{major}*

%files -n %{libdev}
%defattr(-,root,root)
%doc doc/*
%{_includedir}/eastwood
%{_libdir}/libeastwood.so
%{_libdir}/pkgconfig/libeastwood.pc
%{_datadir}/cmake/Modules/FindLibEastwood.cmake

%changelog
* Mon Jan 27 2009 Per Øyvind Karlsen <peroyvind@mandriva.org> 0.1-1
- initial package
