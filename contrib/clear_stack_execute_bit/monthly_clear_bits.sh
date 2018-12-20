#!/bin/bash
# -*- shell-script -*-

module list

for month_range in 12:31 11:30 10:31 09:30 08:31 07:31 06:30; do
    left=${month_range%:*}
    rght=${month_range#*:}

    strt="2018-${left}-01"
    fini="2018-${left}-${rght}"

    echo $(date) python2 ./xalt_db_clear_bit_run.py --confFn ./xalt_stampede2_db.conf --start "$strt" --end "$fini" --clear_bit
                 python2 ./xalt_db_clear_bit_run.py --confFn ./xalt_stampede2_db.conf --start "$strt" --end "$fini" --clear_bit
done
date


