# capture executables on all nodes
hostname_patterns = [
  ['KEEP', '.*']
]
path_patterns = [
  ['PKGS',  r'.*\/test_record_pkg'],
  ['SKIP', r'.*\/lua'],
  ['SKIP', r'.*\/expr'],
  ['SKIP', r'.*\/cc1'],
  ['SKIP', r'.*\/bash'],
  ['SKIP', r'.*\/collect2'],
  ['SKIP', r'.*\/mpich/.*'],
  ['SKIP', r'.*\/x86_64-linux-gnu.*'],
]
