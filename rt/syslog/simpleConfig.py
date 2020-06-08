# capture executables on all nodes
hostname_patterns = [
  ['KEEP', '.*']
]
path_patterns = [
  ['PKGS', r'.*\/test_record_pkg'],
  ['PKGS', r'.*\/python[0-9][^/][^/]*'],
  ['SKIP', r'.*\/lua'],
  ['SKIP', r'.*\/expr'],
  ['SKIP', r'.*\/cc1'],
  ['SKIP', r'.*\/bash'],
  ['SKIP', r'.*\/collect2'],
  ['SKIP', r'.*\/mpich/.*'],
  ['SKIP', r'.*\/x86_64-linux-gnu.*'],
]

python_pkg_patterns = [
  { 'k_s' : 'SKIP', 'kind' : 'path', 'patt' : r"^[^/]"      },  # SKIP all built-in packages
  { 'k_s' : 'SKIP', 'kind' : 'name', 'patt' : r"^_"         },  # SKIP names that start with a underscore
  { 'k_s' : 'SKIP', 'kind' : 'name', 'patt' : r".*\."       },  # SKIP all names that are divided with periods: a.b.c
  { 'k_s' : 'KEEP', 'kind' : 'path', 'patt' : r".*/.local/" },  # KEEP all packages installed by users
  { 'k_s' : 'SKIP', 'kind' : 'path', 'patt' : r"/home"      },  # SKIP all other packages in user locations
]
