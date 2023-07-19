New Features in XALT
====================

**Controlling installed permission**:
    (xalt-2.10.43+): XALT now uses your umask to set permissions unless
    your user id (id -u) is less than UID_MIN (from /etc/login.defs)
    or UID_MIN=1000 otherwise.  In that case, it uses a
    umask of 022.  This translates to 755 for executable files and 644
    for all others.  Sites can override this at configure time with
    **--with-mode=MODE**.

**Controlling subdirectories to store XALT data**:
   (xalt-2.10.43+): XALT create a prime number of directories named 000 to 996 
   where the json result files are written. A user installing  XALT can turn off
   the creation of prime number subdirectories by setting  --with-primeNumber=0 
   to create a flat file hierarchy during configure command. Note that this is 
   not an optimal way to keep thousands of files in a single directory.