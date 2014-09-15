#-----------------------------------------------------------------------
# XALT: A tool to track the programs on a cluster.
# Copyright (C) 2013-2014 University of Texas at Austin
# Copyright (C) 2013-2014 University of Tennessee
# 
# This library is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation; either version 2.1 of 
# the License, or (at your option) any later version. 
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# Lesser  General Public License for more details. 
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free
# Software Foundation, Inc., 59 Temple Place, Suite 330,
# Boston, MA 02111-1307 USA
#-----------------------------------------------------------------------

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
