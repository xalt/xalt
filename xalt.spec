# XALT RPM Spec file
#
# Download the XALT release tarball and build a RPM like:
#   $ rpmbuild -tb xalt-<version>.tar.gz --define 'config /path/to/myconfig.py' <options>
#
#   $ rpmbuild -tb xalt-<version>.tar.gz --define 'config /path/to/myconfig.py' --define 'syshost hardcode:mycluster' --with gpu
#
# Options:
#
#   --define 'config <value>':
#     Absolute path to the config file.  If the config file exists
#     in the tarball, then a relative path may be used.  This option
#     is required.
#
#   --define 'prefix <value>':
#     Absolute path prefix to install XALT.  The default value is "/opt".
#
#   --define 'syshost <value>':
#     System name.  The default value is "hardcode:cluster".
#
#   --define 'tranmission <value>':
#     Transmission style.  The default value is "syslog".
#
#   --with gpu / --without gpu
#     Enable GPU tracking.  The default is disabled.
#
#   --with mpi / --without mpi
#     Enable MPI tracking.  The default is enabled.
#
#   --with mysql / --without mysql
#     Enable MySQL support.  The default is disabled.
#
#   --with static / --without static
#     Link with static libraries.  The default is disabled.

%define _prefix %{?prefix}%{?!prefix:/opt}
%define _moduledir %{_prefix}/%{name}/%{version}/modulefiles
%define debug_package %{nil}

# XALT configuration options
%define _config %{?config}
%define _syshost %{?syshost}%{?!syshost:hardcode:cluster}
%define _transmission %{?transmission}%{?!transmission:syslog}
%define _build_id_links none

%bcond_with gpu
%bcond_without mpi
%bcond_with mysql
%bcond_with static

Summary: The XALT software usage tracking tool
Name: xalt
Version: 3.1.1
Release: 1%{?dist}
License: LGPLv2+
Group: System Environment/Base
URL: https://github.com/xalt/xalt
Source: %{name}-%{version}.tar.gz

AutoReq: no
BuildRequires: coreutils
BuildRequires: elfutils-libelf-devel
BuildRequires: flex
BuildRequires: gcc >= 4.8.5
BuildRequires: gcc-c++ >= 4.8.5
BuildRequires: glibc-headers
BuildRequires: util-linux
Requires: coreutils
Requires: glibc
Requires: libgcc
Requires: libstdc++
Requires: util-linux

%if %{with gpu}
BuildRequires: /usr/local/cuda/include/nvml.h
%endif

%if %{with static}
BuildRequires: openssl-static
%else
BuildRequires: openssl-libs
Requires: openssl-libs
%endif

%description
XALT is a tool to allow a site to track user executables and library
usage on a cluster. When installed it can tell a site what are the top
executables by Node-Hours or by the number of users or the number of times
it is run. XALT 2 also tracks library usage as well. XALT 2 can also track
package use by R, MATLAB or Python. It tracks both MPI and non-MPI
programs.

%prep
%setup -q

# The config file must be defined
if ! test -f "%{_config}"; then
  echo "valid config file is required: --define 'config <file>'"
  exit 1
fi

%build
./configure \
  --prefix=%{_prefix} \
  %{!?with_mysql:--with-MySQL=no} %{?with_mysql:--with-MySQL=yes} \
  %{!?with_static:--with-staticLibs=no} %{?with_static:--with-staticLibs=yes} \
  %{!?with_gpu:--with-trackGPU=no} %{?with_gpu:--with-trackGPU=yes} \
  %{!?with_mpi:--with-trackMPI=no} %{?with_mpi:--with-trackMPI=yes} \
  --with-config=%{_config} \
  --with-syshostConfig=%{_syshost} \
  --with-transmission=%{_transmission}

%install
%{__make} install DESTDIR=%{buildroot} %{?mflags_install}

# Generate Lua modulefile that can be used with Lmod
%{__mkdir_p} $RPM_BUILD_ROOT/%{_moduledir}/lua
cat <<EOF >$RPM_BUILD_ROOT/%{_moduledir}/lua/xalt-%{version}.lua
--NOTE: This is an automatically-generated file!

setenv("XALT_EXECUTABLE_TRACKING", "yes")

%if %{with gpu}
setenv("XALT_GPU_TRACKING", "yes")
%endif

local base  = "%{_prefix}/%{name}/%{version}"
local bin   = pathJoin(base, "bin")
local lib   = pathJoin(base, "%{_lib}")

prepend_path{"PATH",          bin, priority="100"}
prepend_path("LD_PRELOAD",    pathJoin(lib, "libxalt_init.so"))
prepend_path("COMPILER_PATH", bin)

-- Uncomment these two lines to use XALT inside Singularity containers
-- setenv("SINGULARITYENV_LD_PRELOAD", pathJoin(lib, "libxalt_init.so"))
-- prepend_path("SINGULARITY_BINDPATH", base)

------------------------------------------------------------
-- Only set this in production not for testing!!!
setenv("XALT_SCALAR_SAMPLING", "yes")
EOF

# Generate tcl modulefile
%{__mkdir_p} $RPM_BUILD_ROOT/%{_moduledir}/tcl
cat <<EOF >$RPM_BUILD_ROOT/%{_moduledir}/tcl/xalt-%{version}
setenv XALT_EXECUTABLE_TRACKING yes

%if %{with gpu}
setenv XALT_GPU_TRACKING yes
%endif

prepend-path  PATH            %{_prefix}/%{name}/%{version}/bin  100
prepend-path  LD_PRELOAD      %{_prefix}/%{name}/%{version}/%{_lib}/libxalt_init.so
prepend-path  COMPILER_PATH   %{_prefix}/%{name}/%{version}/bin

# Uncomment this to use XALT inside Singularity containers
#setenv SINGULARITYENV_LD_PRELOAD %{_prefix}/%{name}/%{version}/%{_lib}/libxalt_init.so
#prepend-path SINGULARITY_BINDPATH %{_prefix}/%{name}/%{version} 

############################################################
## Only set this in production not for testing!!!
setenv XALT_SCALAR_SAMPLING yes
EOF

%files
%defattr(-,root,root)
%{_prefix}/%{name}/*
%exclude %{_prefix}/%{name}/%{version}/libexec/*.pyc
%exclude %{_prefix}/%{name}/%{version}/libexec/*.pyo
%exclude %{_prefix}/%{name}/%{version}/libexec/getent/*.pyc
%exclude %{_prefix}/%{name}/%{version}/libexec/getent/*.pyo
%exclude %{_prefix}/%{name}/%{version}/%{_lib}/*.o

%changelog
* Tue Apr 9 2019 Scott McMillan <smcmillan@nvidia.com>
- Remove default config file, config option is now required

* Mon Apr 1 2019 Scott McMillan <smcmillan@nvidia.com>
- Initial spec file















