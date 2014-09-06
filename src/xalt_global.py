import os
from xalt_stack  import Stack

#------------------------------------------------------------------------
# XALT_Stack:  Instance of a simple stack class that contains useful
#              data when there is an exception
#------------------------------------------------------------------------

XALT_Stack              = Stack()

#------------------------------------------------------------------------
# XALT_TRANSMISSION_STYLE:  Controls where the json data is sent:
#                           "file"     : writes a file ~/.xalt.d
#                           "syslog"   : writes data to syslog
#                           "directdb" : Calls db directly
#------------------------------------------------------------------------

XALT_TRANSMISSION_STYLE = os.environ.get("XALT_TRANSMISSION_STYLE","@transmission@")

#------------------------------------------------------------------------
# XALT_ETC_DIR:  Controls where the xalt_db.conf and the Reverse Map
#                Directory can be found.
#------------------------------------------------------------------------

XALT_ETC_DIR            = os.environ.get("XALT_ETC_DIR","@etc_dir@")
