from getent.constants import * # NOQA


class AliasStruct(Structure):
    _fields_ = [
        ('name', ctypes_c_char_p),
        ('members_len', size_t),
        ('members', POINTER(ctypes_c_char_p)),
        ('local', c_int),
    ]


class InAddrStruct(Structure):
    # Taken from <netinet/in.h>
    _fields_ = [
        ('s_addr', uint32_t),
    ]


class InAddr6Union(Union):
    _fields_ = [
        ('u6_addr8', ARRAY(uint8_t, 16)),
        ('u6_addr16', ARRAY(uint16_t, 8)),
        ('u6_addr32', ARRAY(uint32_t, 4)),
    ]


class InAddr6Struct(Structure):
    # Taken from <netinet/in.h>
    _anonymous_ = ('in6_u',)
    _fields_ = [
        ('in6_u', InAddr6Union),
    ]


class HostStruct(Structure):
    _fields_ = [
        ('name', ctypes_c_char_p),
        ('aliases', POINTER(ctypes_c_char_p)),
        ('addrtype', c_int),
        ('addr_list_len', c_int),
        ('addr_list', POINTER(c_void_p)),
    ]


class NetworkStruct(Structure):
    # Taken from <netdb.h>
    _fields_ = [
        ('name', ctypes_c_char_p),              # official network name
        ('aliases', POINTER(ctypes_c_char_p)),  # alias list
        ('addrtype', c_int),            # net address type
        ('net', uint32_t),              # network number
    ]


class ProtoStruct(Structure):
    # Taken from <netdb.h>
    _fields_ = [
        ('name', ctypes_c_char_p),              # official protocol name
        ('aliases', POINTER(ctypes_c_char_p)),  # alias list
        ('proto', c_int),               # protocol number
    ]


class RPCStruct(Structure):
    # Taken from <rpc/netdb.h>
    _fields_ = [
        ('name', ctypes_c_char_p),              # name of server for RPC program
        ('aliases', POINTER(ctypes_c_char_p)),  # alias list
        ('number', c_long),             # RPC program number
    ]


class ServiceStruct(Structure):
    # Taken from <serv/netdb.h>
    _fields_ = [
        ('name', ctypes_c_char_p),              # official service name
        ('aliases', POINTER(ctypes_c_char_p)),  # alias list
        ('port', c_int),                # port number
        ('proto', ctypes_c_char_p),            # protocol to use
    ]


class GroupStruct(Structure):
    # Taken from <grp.h>
    _fields_ = [
        ("name", ctypes_c_char_p),
        ("password", ctypes_c_char_p),
        ("gid", c_int),
        ("members", POINTER(ctypes_c_char_p)),
    ]


class PasswdStruct(Structure):
    _fields_ = [
        ('name', ctypes_c_char_p),
        ('password', ctypes_c_char_p),
        ('uid', c_int),
        ('gid', c_int),
        ('gecos', ctypes_c_char_p),
        ('dir', ctypes_c_char_p),
        ('shell', ctypes_c_char_p),
    ]


class ShadowStruct(Structure):
    _fields_ = [
        ('name', ctypes_c_char_p),
        ('password', ctypes_c_char_p),
        ('change', c_long),
        ('min', c_long),
        ('max', c_long),
        ('warn', c_long),
        ('inact', c_long),
        ('expire', c_long),
        ('flag', c_ulong),
    ]
