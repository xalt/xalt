# $Id$

Summary: The XALT software usage tracking tool
Name: xalt
Version: 3.0.1
Release: 1%{?dist}
License: LGPLv2
Group: System Environment/Base
Source0:  %{name}-%{version}.tar.gz
Packager: mclay@tacc.utexas.edu

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

#------------------------------------------------
# Set this obscure variable so that the rpm program
# will byte-compile python3 code correctly.
#------------------------------------------------
%global _python_bytecompile_errors_terminate_build 0

#------------------------------------------------
# BASIC DEFINITIONS
#------------------------------------------------
%define debug_package %{nil}
%include rpm-dir.inc

%define PNAME xalt
%define APPS /opt/apps
%define MODULES modulefiles

%define PKG_BASE      %{APPS}/%{name}
%define INSTALL_DIR   %{APPS}/%{PNAME}/%{version}
%define GENERIC_IDIR  %{PKG_BASE}/%{name}
%define MODULE_DIR    %{APPS}/%{MODULES}/%{PNAME}
%define MODULE_VAR    TACC_XALT

%description
XALT is a tool to allow a site to track user executables and library
usage on a cluster. When installed it can tell a site what are the top
executables by Node-Hours or by the number of users or the number of times
it is run. XALT also tracks library usage as well. XALT can also track
package use by R, MATLAB or Python. It tracks both MPI and non-MPI
programs.


%prep

%setup -n %{PNAME}-%{PNAME}-%{version}

%build
%install


if [ -f "$BASH_ENV" ]; then
  . $BASH_ENV
  module purge
  clearMT
  export MODULEPATH=/opt/apps/xsede/modulefiles:/opt/apps/modulefiles:/opt/modulefiles
#  module load cuda
fi


myhost=$(hostname -f)
myhost=${myhost%.tacc.utexas.edu}
HOST=$myhost
first=${myhost%%.*}
SYSHOST=${myhost#*.}

TRANSMISSION=syslog
SYSHOST_CONF=nth_name:2
ETC_DIR=/tmp/moduleData

#if [[ $SYSHOST = "s3" ]]; then
#  TRANSMISSION=file
#  CONF_OPTS="--with-xaltFilePrefix=/scratch/projects/XALT"
#  SYSHOST_CONF=hardcode:s3
#  ETC_DIR=/home1/moduleData
#fi

if [[ $SYSHOST = "ls5" ]]; then
  TRANSMISSION=file
  CONF_OPTS="--with-xaltFilePrefix=/scratch/projects/XALT"
  SYSHOST_CONF=hardcode:ls5
  ETC_DIR=/home1/moduleData
fi

if [[ $SYSHOST = "longhorn" ]]; then
  TRANSMISSION=file
  MAKE_OPTS="EXTRA_FLAGS=-I$TACC_CUDA_INC"
  CONF_OPTS="--with-xaltFilePrefix=/scratch/projects/XALT --with-trackGPU=nvml CPPFLAGS=-I$TACC_CUDA_INC"
  ETC_DIR=/home/moduleData
fi

if [[ $SYSHOST = "frontera" ]]; then
  MAKE_OPTS="EXTRA_FLAGS=-I$TACC_CUDA_INC"
  CONF_OPTS="--with-trackGPU=nvml CPPFLAGS=-I$TACC_CUDA_INC"
fi

CXX=/usr/bin/g++ CC=/usr/bin/gcc ./configure CXX=/usr/bin/g++ CC=/usr/bin/gcc --prefix=%{APPS} --with-syshostConfig=$SYSHOST_CONF \
   --with-config=Config/TACC_config.py --with-transmission=$TRANSMISSION --with-MySQL=no                                          \
   --with-etcDir=$ETC_DIR $CONF_OPTS

touch Makefile.in
touch makefile
make CXX=/usr/bin/g++ CC=/usr/bin/gcc DESTDIR=$RPM_BUILD_ROOT $MAKE_OPTS install Inst_TACC
rm -f $RPM_BUILD_ROOT/%{INSTALL_DIR}/sbin/xalt_db.conf
rm $RPM_BUILD_ROOT/%{INSTALL_DIR}/../%{name}

#-----------------
# Modules Section
#-----------------

rm -rf $RPM_BUILD_ROOT/%{MODULE_DIR}
mkdir -p $RPM_BUILD_ROOT//%{MODULE_DIR}
cat   >  $RPM_BUILD_ROOT/%{MODULE_DIR}/%{version}.lua << 'EOF'
help(
[[
The xalt module collects system usage data.

Version %{version}
]]
)

whatis("Name: XALT")
whatis("Version: %{version}")
whatis("Category: tools")
whatis("Keywords: System, TOOLS")
whatis("URL: http://xalt.sf.net")
whatis("Description: Collects system usage data")

prepend_path{"PATH",                      "%{GENERIC_IDIR}/bin", priority = 100}
prepend_path("COMPILER_PATH",             "%{GENERIC_IDIR}/bin")
prepend_path("LD_PRELOAD",                "%{GENERIC_IDIR}/lib64/libxalt_init.so")
setenv (     "%{MODULE_VAR}_DIR",         "%{GENERIC_IDIR}/")
setenv (     "%{MODULE_VAR}_BIN",         "%{GENERIC_IDIR}/bin")
setenv (     "XALT_EXECUTABLE_TRACKING",  "yes")
setenv (     "XALT_SAMPLING",             "yes")

-- Uncomment these two lines to use XALT inside Singularity containers
-- setenv("SINGULARITYENV_LD_PRELOAD", pathJoin(base,"$LIB/libxalt_init.so"))
-- prepend_path("SINGULARITY_BINDPATH", base)

-- Uncomment this line to have XALT track python packages.
-- prepend_path("PYTHONPATH",           "%{GENERIC_IDIR}/site_packages")
EOF

#--------------
#  Version file.
#--------------

cat > $RPM_BUILD_ROOT/%{MODULE_DIR}/.version.%{version} << 'EOF'
#%Module3.1.1#################################################
##
## version file for %{PNAME}-%{version}
##

set     ModulesVersion      "%{version}"
EOF

%{SPEC_DIR}/checkModuleSyntax $RPM_BUILD_ROOT/%{MODULE_DIR}/%{version}.lua

%files
%defattr(-,root,install)
%{INSTALL_DIR}
%{MODULE_DIR}

%post

cd %{PKG_BASE}

if [ -d %{name} ]; then
  rm -f %{name}
fi
ln -s %{version} %{name}

%postun

cd %{PKG_BASE}

if [ -h %{name} ]; then
  lv=`readlink %{name}`
  if [ ! -d $lv ]; then
    rm %{name}
  fi
fi

%clean
rm -rf $RPM_BUILD_ROOT

