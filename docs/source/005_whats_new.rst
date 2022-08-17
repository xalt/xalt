New Features in XALT
====================

**Controlling installed permission**:
    (xalt-2.10.41+): XALT now uses your umask to set permissions unless
    your user id (id -u) is less than 500.  In that case, it uses a
    umask of 022.  This translates to 755 for executable files and 644
    for all others.  Sites can override this at configure time with
    **--with-mode=MODE**.
