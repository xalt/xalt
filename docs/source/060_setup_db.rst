Setup the database
------------------

It is expected that a site which uses the database system setup for
XALT will install the database on a VM.  The necessary steps are given
below.

Install XALT on the VM
^^^^^^^^^^^^^^^^^^^^^^

You'll need to install the MySQL packages on the VM and the
python MySQL package as well::

    $ git clone https://github.com/xalt/xalt.git
    $ cd xalt
    $ ./configure --prefix=$XALT_DIR                               \
      --with-syshostConfig=hardcode:$SYSHOST                       \
      --with-config=Config/SITE_config.py 
    $ make install

where XALT_DIR is the location like /opt/apps or where ever you wish
to install XALT on the VM; SYSHOST is the name of your cluster.  You
can one MySQL DB server be shared by multiple clusters.  Just pick one
name for SYSHOST here, it won't be used on the VM so it doesn't matter
which name you chose. Also **SITE_config.py** is the configuration
file that you used before.


Install MySQL on the VM
^^^^^^^^^^^^^^^^^^^^^^^

Here you will want to install MySQL where the database itself has room
to grow.  You should consider installing it where the databases have
at least a couple of TB of space.  If you have more than one cluster
where you are collecting XALT data, you'll have a separate database
for each cluster.

Create a database account
^^^^^^^^^^^^^^^^^^^^^^^^^

I recommend that you name your database xalt_<clusterName>.  So TACC's db for
stampede2 is called xalt_stampede2. Then create an account to write to
that database::

    mysql> GRANT ALL PRIVILEGES ON xalt_stampede2.* To 'xaltUser'@'hostname' IDENTIFIED BY 'password';
    mysql> GRANT ALL PRIVILEGES ON xalt_stampede2.* To 'xaltUser'@'*' IDENTIFIED BY 'password';
    
Where you have correctly set the **db name**, **userName** and **hostname** and
**password** to match your setup.

Setup database credentials
^^^^^^^^^^^^^^^^^^^^^^^^^^
Create a file to hold the database credentials with
conf_create.py for each cluster::

    $ cd $XALT_ETC_DIR; python $XALT_DIR/xalt/xalt/sbin/conf_create.py

And answer the questions asked to match what you did in step above.
