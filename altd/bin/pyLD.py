#! /usr/bin/python -E

'''
Copyright (c) 2010 The University of Tennessee
Copyright (c) 2010 UT-Batelle LLC

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
'''

import sys

sys.path = ['', '/nics/e/sw/tools/bin', '/usr/lib/python24.zip', '/usr/lib64/python2.4', '/usr/lib64/python2.4/plat-linux2', '/usr/lib64/python2.4/lib-tk', '/usr/lib64/python2.4/lib-dynload', '/usr/lib64/python2.4/site-packages', '/usr/lib64/python2.4/site-packages/Numeric', '/usr/lib64/python2.4/site-packages/dbus', '/usr/lib64/python2.4/site-packages/gtk-2.0']

import socket
import os
import time
import logging,logging.config 
import ConfigParser 
import base64

# ALTD path
ALTD_PATH=os.getenv("ALTD_PATH")
CURVER="0.7"
VERBOSE=False
SELECT_ON=False

if(os.getenv("ALTD_SELECT_ON") == "1" ):
    SELECT_ON=True
    USERS=os.getenv("ALTD_SELECT_USERS").split(",")
if(os.getenv("ALTD_VERBOSE") == "1" or (len(sys.argv) > 2 and sys.argv[1] == "--altd_verbose")):
    VERBOSE=True
try:
    # Initialize logger
    logging.config.fileConfig(ALTD_PATH+"/etc/altd_log.conf")
    if(VERBOSE):
        log=logging.getLogger("altd_ld2")
    else:
        log=logging.getLogger("altd_ld")
    log.debug("Verbose:"+str(VERBOSE))    
except IOError, err:
    sys.stderr.write("Cannot load log config file! Program aborted...")
    sys.exit(3)
try:
    config=ConfigParser.ConfigParser()
    cFile=open(ALTD_PATH+'/etc/altd_db.conf','r')
    config.readfp(cFile)
    # Load MySQL db info from config file
    MYSQL_HOST=config.get("MYSQL","HOST")
    MYSQL_USER=config.get("MYSQL","USER")
    MYSQL_PASSWD=base64.b64decode(config.get("MYSQL","PASSWD"))
   
    MYSQL_DB=config.get("MYSQL","DB")
    cFile.close()
except ConfigParser.NoOptionError, err:    
    log.exception(err)
except IOError, err:
    log.critical("Cannot load db config file! Program aborted...")
    sys.exit(3)
try:
    import MySQLdb
except ImportError, err:
    log.critical("No python MySQL module found")
    log.critical("Need to install python MySQLdb module...")
    sys.exit(3)

class ALTD_LD(object):
    def __init__(self,name=socket.getfqdn().lower()):
        """
        Return:
            None
        Function:
            Initialize variables.
        Description:
            Categorize hostname, because there can be krakenpf10, krakenpf9, 
            etc.  If none of the hostname matches the list, use unknown.
        """
        self.conn=None
        log.debug("Hostname:"+name)
        if(name.find("kraken") != -1):
            self.machine="kraken"
        elif(name.find("athena") != -1):
            self.machine="athena"
        elif(name.find("verne") != -1):
            self.machine="verne"
        else:
            self.machine="unknown"
        log.debug("Machine name:"+self.machine)
        self.username=os.getenv("USER")
        self.isConnected=False
        
    def mysql_db_connect(self):
        """
        Return:
            Connection status
        Function:
            Initialize MySQL connection.
        Description:
            Initialize MySQL connection with login info from config file.
            If MySQL is connected successfully, set isConnected to True.
        """
        try:
            self.conn = MySQLdb.connect(MYSQL_HOST,MYSQL_USER,MYSQL_PASSWD,MYSQL_DB)
            self.isConnected = True
            log.info("MySQLdb is connected")
            return True
        except MySQLdb.Error, err:
            log.exception(err)
            self.isConnected = False
            return False
    
    def table_lookup(self):
        """
        Return:
            Auto inc id of inserted record
        Function:
            Insert a record to link_tags table.
        Description:
            Store user info in the built machine table with default linkline_id=0,
            exit_code=-1 and current time. After the operation is completed, 
            return the tag_id.
        """
        inc_id=0
        if(not self.isConnected):
            inc_id=0
        else:
            query = "INSERT INTO altd_%s_link_tags VALUES (NULL, '%d', '%s', '%d', NOW())" %
                                                       (self.machine,0,self.username,-1)
            try:
                self.conn.query(query)
                inc_id = self.conn.insert_id()
                log.info("username=%s hostname=%s pos=%ld LD_SUC_INFO" %
                         (self.username, self.machine, inc_id))
            except MySQLdb.Error, err:
                log.exception(err)
        # Write to stderr for ld script to read in
        sys.stderr.write(str(inc_id))
        return inc_id
    
    def print_assembly(self,inc_id, path):
        """
        Return:
            None    
        Function:
            Generate ALTD assembly code.
        Description:
            The assembly file contains four fields, altd version, build  
            machine name, tag_id and the year. This information is sandwiched
            between ALTD_Link_Info and ATLD_Link_Info_End.
        """
        year = time.strftime("%Y")
        f = open(path,'w')

        f.writelines("\t.section .altd\n")
        f.writelines("\t.asciz \"ALTD_Link_Info\"\n") #this is how to find the section in the exec
        # Print cushion
        f.writelines("\n\t.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00\n")
        f.writelines("\t.asciz \"Version:"+CURVER+":\"\n")
        f.writelines("\t.asciz \"Machine:"+self.machine+":\"\n")
        f.writelines("\t.asciz \"Tag_id:"+str(inc_id)+":\"\n")
        f.writelines("\t.asciz \"Year:"+year+":\"\n")
        f.writelines("\t.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00\n")
        f.writelines("\t.asciz \"ALTD_Link_Info_End\"\n")

        log.debug("Tag id:"+str(inc_id))
    
        f.close()

    def update_tag_table(self,tag_id,ecode,linkline):
        """
        Return:
            None
        Function:
            Insert linkline and update record in link_tags table. 
        Description:
            Use the linkline as search key, if the linkline does not
            exist in the linkline table, insert a new record.
            Retrive linking_inc id and update to link_tags table 
            with ld exit status.
        """
        log.debug("isConnected?"+str(self.isConnected))
        if(not self.isConnected):
            log.debug("update_tag_table: is not connected to mysql")
            return -1
        else:
            query = "SELECT linking_inc FROM altd_%s_linkline WHERE linkline='%s'" % (self.machine,linkline)
            try:
                self.conn.query(query)
                result = self.conn.store_result()
                if(result.num_rows()):
                    # Same linkline found! 
                    row = result.fetch_row()
                    log.debug("ROW:"+str(row))
                    inc_id = int(row[0][0])
                    log.debug("MySQL inc_id:"+str(inc_id))
                    if(inc_id>0):
                        log.info("username=%s hostname=%s pos=%ld LD_SUC_INFO" % (self.username, self.machine, inc_id))
                else:
                    # It's unique, let's update the links_t table
                    query = "INSERT INTO altd_%s_linkline VALUES (NULL,'%s')" % (self.machine, linkline)
                    self.conn.query(query)
                    inc_id = self.conn.insert_id()
                    log.info("username=%s hostname=%s pos=%ld LD_SUC_INFO_NEW" % (self.username, self.machine, inc_id))
                query = "UPDATE altd_%s_link_tags SET linkline_id=%ld, exit_code=%d WHERE tag_id=%ld" % (self.machine,inc_id,ecode,tag_id)
                self.conn.query(query)
                
            except MySQLdb.Error, err:
                log.exception(err)

    def mysql_db_disconnect(self):
        """
        Return:
            None
        Function:
            Disconnect the MySQL connection.
        Description:
            If the MySQL is connected, disconnect the connection.
        """
        if(self.isConnected):
            try:
                self.conn.close()
                self.isConnected = False
            except MySQLdb.Error, err:
                log.exception(err)
                
    def main(self):
        """
        Return:
            None
        Function:
            generate assembly:
                - table_lookup()
                - print_assembly()
            store linkline:
                - update_tag_table()
        Description:
            If "--altd_verbose" exists, shift argc by one.
        """
        log.debug(sys.argv)
        if(len(sys.argv) > 2):
            if(sys.argv[1]=="--altd_verbose"):
                PRE=1
            else:
                PRE=0
            log.debug("PRE:%d"%PRE)
            if(SELECT_ON):
                if(self.username not in USERS):
                    log.error("user %s is not in selected list" % self.username)
                    sys.exit(3)
                else:
                    log.info("SELECT_ON mode user:%s" % self.username)
            if(sys.argv[PRE+1] == "--gen_assembly"):
                path = sys.argv[PRE+2]
                log.info("Process gen_assembly")
                if(self.mysql_db_connect()):
                    inc_id = self.table_lookup()
                    self.print_assembly(inc_id,path)
                if(self.isConnected):
                    self.mysql_db_disconnect()
                    return 0
            elif(sys.argv[PRE+1] == "--store_line"):
                log.debug("store_line")
                tag_id = int(sys.argv[PRE+2])
                exit_status = int(sys.argv[PRE+3])
    
                linkline = sys.argv[PRE+4]
                for line in sys.argv[PRE+5:]:
                    linkline += " "+line

                if(self.mysql_db_connect()):
                    self.update_tag_table(tag_id,exit_status,linkline)
                else:
                    return 0

                self.mysql_db_disconnect()
                return 0
            else:
                log.error("usage error")
                sys.exit(3)
        else:
            log.debug("No argument passed to pyLD")

if __name__ == "__main__":
   ALTD_LD().main()
    
