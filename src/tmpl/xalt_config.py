# This is the config file that XALT uses to set defaults for the
# tables. These patterns are used after the site config file.
#

#------------------------------------------------------------
# At the end of the  hostname patterns we mark all hosts
# as ignored. The Site file better accept some hostname or
# XALT won't track on any computers.
hostname_patterns = [
  ['SKIP', r'.*']                    # skip all hosts
  ]

#------------------------------------------------------------
# This "table" is use to filter executables by their path
# The value on the left is either KEEP or SKIP.  If the value
# is KEEP then if the path matches the regular expression then
# the executable is acceptable as far as the path test goes.
# If the value on the left is SKIP then if the path matches
# the regular expression then executable is not acceptable so
# no XALT tracking is done for that path.

# These executables are the XALT executables that must NEVER
# be tracked.

# Note that XALT_INSTALL_DIR is replaced with the XALT 
# installation directory

head_path_patterns = [
  ['SKIP',  r'.*\/bin\/my_uuidgen'],
  ['SKIP',  r'.*\/logger'],
  ['SKIP',  r'.*\/xalt_print_os'],
  ['SKIP',  r'.*\/xalt_syslog_to_db'],
  ['SKIP',  r'.*\/xalt_extract_record.x'],
  ['SKIP',  r'.*\/xalt_configuration_report.x'],
  ['SKIP',  r'.*\/xalt_syshost'],
  ['SKIP',  r'.*\/xalt_record_pkg'],
  ['SKIP',  r'.*\/sh'],
  ['SKIP',  r'.*\/dash'],
  ['SKIP',  r'.*\/bash'],
  ['SKIP',  r'.*\/zsh'],
  ['SKIP',  r'.*\/csh'],
  ['SKIP',  r'.*\/tcsh'],
  ['SKIP',  r'.*\/nu'],
  ['SKIP',  r'.*\/fish'],
  ['SKIP',  r'.*\/rc'],
  ['SKIP',  "XALT_INSTALL_DIR" ],
  ]


path_patterns = [
  ['KEEP',  r'.*'],
  ]
    
path_arg_patterns = [
  ['CONTINUE',  r'[^;]*;-.*'],
  ['CONTINUE',  r'.*'],
  ]
  


#------------------------------------------------------------
# XALT filter environment variables.  By default XALT will not
# record any env. vars in the xalt_env table.

env_patterns = [
  [ 'SKIP', r'.*' ],
  ]

#------------------------------------------------------------
# XALT filter python package filter.

python_pkg_patterns = [
  { 'k_s' : 'SKIP', 'kind' : 'name', 'patt' : r"^os$"         },  # SKIP os
  { 'k_s' : 'SKIP', 'kind' : 'name', 'patt' : r"^re$"         },  # SKIP re
  { 'k_s' : 'SKIP', 'kind' : 'name', 'patt' : r"^sys$"        },  # SKIP sys
  { 'k_s' : 'SKIP', 'kind' : 'name', 'patt' : r"^subprocess$" },  # SKIP subprocess
  { 'k_s' : 'KEEP', 'kind' : 'path', 'patt' : r".*"           },  # keep all other patterns
]  

#------------------------------------------------------------
# XALT pre-ingest filter
# 
pre_ingest_patterns = [
#   precent   path pattern 
    [1.0,     r'.*']                    # accept all remaining executables.
]

#------------------------------------------------------------
# XALT package filter
# 
pkg_patterns = [
#              pattern 
    ['KEEP',   r'.*']                    # accept all remaining executables.
]
