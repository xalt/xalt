from getent.constants import * # NOQA
from getent import headers
from ctypes.util import find_library

__all__ = ['GENERATE_MAP', 'libc', 'inet_pton',
           'gethostbyaddr', 'gethostbyname2', 'getnetbyname',
           'getgrnam', 'getgrgid', 'getpwnam', 'getpwuid',
           'getprotobyname', 'getprotobynumber',
           'getrpcbyname', 'getrpcbynumber',
           'getservbyname', 'getservbyport']

# Libc object
libc = CDLL(find_library("c"))

GENERATE_MAP = dict(
    alias=headers.AliasStruct,
    host=headers.HostStruct,
    net=headers.NetworkStruct,
    proto=headers.ProtoStruct,
    rpc=headers.RPCStruct,
    serv=headers.ServiceStruct,
    gr=headers.GroupStruct,
    pw=headers.PasswdStruct,
    sp=headers.ShadowStruct,
)

# Map libc function calls
for name, struct in GENERATE_MAP.items():
    base = '%sent' % (name,)
    globals()['set%s' % (base,)] = getattr(libc, 'set%s' % (base,), None)
    globals()['get%s' % (base,)] = getattr(libc, 'get%s' % (base,), None)
    if globals()['get%s' % (base,)] is not None:
        globals()['get%s' % (base,)].restype = POINTER(struct)
    globals()['end%s' % (base,)] = getattr(libc, 'end%s' % (base,), None)
    __all__.append('set%s' % (base,))
    __all__.append('get%s' % (base,))
    __all__.append('end%s' % (base,))


inet_pton = libc.inet_pton
inet_pton.argtypes = (c_int, ctypes_c_char_p, c_void_p)
inet_pton.restype = c_int
gethostbyaddr = libc.gethostbyaddr
gethostbyaddr.argtypes = (c_void_p, )
gethostbyaddr.restype = POINTER(headers.HostStruct)
gethostbyname2 = libc.gethostbyname2
gethostbyname2.argtypes = (ctypes_c_char_p, c_uint)
gethostbyname2.restype = POINTER(headers.HostStruct)
getnetbyname = libc.getnetbyname
getnetbyname.argtypes = (ctypes_c_char_p,)
getnetbyname.restype = POINTER(headers.NetworkStruct)
getgrnam = libc.getgrnam
getgrnam.argtypes = (ctypes_c_char_p,)
getgrnam.restype = POINTER(headers.GroupStruct)
getgrgid = libc.getgrgid
getgrgid.argtypes = (gid_t,)
getgrgid.restype = POINTER(headers.GroupStruct)
getpwnam = libc.getpwnam
getpwnam.argtypes = (ctypes_c_char_p,)
getpwnam.restype = POINTER(headers.PasswdStruct)
getpwuid = libc.getpwuid
getpwuid.argtypes = (uid_t,)
getpwuid.restype = POINTER(headers.PasswdStruct)
getprotobyname = libc.getprotobyname
getprotobyname.argtypes = (ctypes_c_char_p,)
getprotobyname.restype = POINTER(headers.ProtoStruct)
getprotobynumber = libc.getprotobynumber
getprotobynumber.argtypes = (c_int,)
getprotobynumber.restype = POINTER(headers.ProtoStruct)
getrpcbyname = libc.getrpcbyname
getrpcbyname.argtypes = (ctypes_c_char_p,)
getrpcbyname.restype = POINTER(headers.RPCStruct)
getrpcbynumber = libc.getrpcbynumber
getrpcbynumber.argtypes = (c_int,)
getrpcbynumber.restype = POINTER(headers.RPCStruct)
getservbyname = libc.getservbyname
getservbyname.argtypes = (ctypes_c_char_p, ctypes_c_char_p)
getservbyname.restype = POINTER(headers.ServiceStruct)
getservbyport = libc.getservbyport
getservbyport.argtypes = (c_int, ctypes_c_char_p)
getservbyport.restype = POINTER(headers.ServiceStruct)
# Not supported on all platforms
try:
    getspnam = libc.getspnam
    getspnam.argtypes = (ctypes_c_char_p,)
    getspnam.restype = POINTER(headers.ShadowStruct)
except AttributeError:
    getspnam = None
