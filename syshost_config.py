#
#  This is how a site defines syshost, that is the system hostname.  
#
# Method (1): hardcode
#   Specify the syshost name at configuration time.
#
# Method (2) second_name:
#    So if login nodes are name (like they are on stampede)  login1.stampede.tacc.utexas.edu.
#    Then the "syshost" is the second name in the hostname.  
#
# Method (3) mapping:
#    Another way to specify syshost is the "mapping".  This is a dictionary where the keys are
#    basic regular expressions and the value is the mapping name
#
# Method (4) read_file:
#    Here a site defines a file to be read that specifies the host name
#  


syshost_config = {
  # specify the way you wish to specify the syshost (a.k.a system host name).  

  'hardcode' : { 'name' : 'ls5' },

  'second_name' : {},

  'mapping'  : {
    'aprun[0-9]-darter' : 'darter',
  }
}
  

  
