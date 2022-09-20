#!/bin/sh
# this is different for linux vs mac
#alias mysqldump=mysqldump
# backup config
DT=$(date +%a.%H%M)
BACKUPFILE=/webbase/lime/dbdumps/${DT}.lime.mysql.gz
echo dumping mysql ${DBNAME} to ${BACKUPFILE}
# mysql config
DBHOST=stagedbinstance.czxetclya2il.ap-southeast-1.rds.amazonaws.com
DBNAME=limedb
DBUSER=limeu
DBPASS=limetest
mysqldump --opt -u${DBUSER} -p${DBPASS} -h${DBHOST} ${DBNAME} | gzip > ${BACKUPFILE}
