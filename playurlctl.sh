#!/bin/bash
# author:weiwei
CMD=$1
NOHUP=${NOHUP:=$(which nohup)}
PS=${PS:=$(which ps)}

PLAYURLCTL="$0"
# resolve links - $0 may be a softlink
while [ -h "$PLAYURLCTL" ]; do
    ls=$(ls -ld "$PLAYURLCTL")
    link=$(expr "$ls" : '.*-> \(.*\)$')
    if expr "$link" : '/.*' > /dev/null; then
        PLAYURLCTL="$link"
    else
        PLAYURLCTL=$(dirname "$PLAYURLCTL")/"$link"
    fi
done

PLAYURLCTL_DIR=${PLAYURLCTL_DIR:=$(dirname "$PLAYURLCTL")}
PLAYURLCTL_RUNDIR=${PLAYURLCTL_RUNDIR:=/home/xcv/pidfile}

NAME=http_server

die() {
    echo $*
    exit 1
}

get_pid() {
    cat $1 2> /dev/null
}

pid_running(){
    kill -0 $1 2> /dev/null
}

do_start()
{
    echo "Starting $NAME ..."
    cd $PLAYURLCTL_DIR
    $NOHUP ./$NAME $1 > /dev/null 2>&1 &
}

do_stop()
{
    if [ ! -n "$1" ];then
        die "Need to choose a instance to stop."
    fi
    PID_FILE=$PLAYURLCTL_RUNDIR/$1
    if [ ! -f  "${PID_FILE}" ]; then
        die "Not stopping. PID file not found: ${PID_FILE}"
    fi

    PID=$(get_pid $PID_FILE)
    echo "Stopping $NAME $PID ..."
    echo "Waiting for $NAME to halt."
    kill -SIGUSR1 $PID
    #kill -SIGUSR1 $PID
    while [ -f "$PID_FILE" ]; do sleep 1; done;
    echo "$1 stopped"
}

do_reload()
{
    if [ ! -n "$1" ];then
       die "Need to choose a instance to reload."
    fi
    PID_FILE=$PLAYURLCTL_RUNDIR/$1
    if [ ! -f  "${PID_FILE}" ]; then
      die "Not reloading. PID file not found: ${PID_FILE}"
    fi
    PID=$(get_pid $PID_FILE)
    echo "Reloading $NAME ..."
    kill -SIGUSR2 $PID
}

do_restart()
{
    if [ ! -n "$1" ];then
       die "Need to choose a instance to restart."
    fi
    if [ ! -n "$2" ];then
       die "Need to set conf_path to restart."
    fi
    echo "Restarting ${1} ..." 
    do_stop $1
    do_start $2
}

do_status()
{
    for f in ${PLAYURLCTL_RUNDIR}/*; do
        if test -f $f; then
            name=$(echo $(basename $f))
            PID=$(get_pid $f)
            if [ ! -z $PID ]; then
                if pid_running $PID; then
                   echo "${name} running with PID ${PID}"
                   continue
                else
                   rm "${f}"
                   echo "Removed ${name} PID file with ${PID}."
                 fi
             fi
         fi
     done
}

case "$1" in
    start)
        do_start $2
        ;;
    stop)
        do_stop $2
        ;;
    reload)
        do_reload $2
        ;;
    restart)
        do_restart $2 $3
        ;; 
    status)
       do_status
       ;;
    *)
        echo "Usage: $SCRIPTNAME {start|stop|restart|reload|status}"
esac

