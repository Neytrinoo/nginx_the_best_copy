#!/usr/bin/bash
# Provides: mohican
# Required-View-Cmd: $SERVER_NAME $cmd $key
### END INIT INFO


export MOHICANS_HOME=.
export SERVER_NAME=mohican
SERVER_STATUS="off"
export PID_FILE=pid_file.txt
export DEFAULT_PATH_TO_CONFIG=.mohican.conf


get_pid() {
  # shellcheck disable=SC2002
  PID_MASTER_PROCESS=$(cat "$PID_FILE" | head -1)
}

start() {
  if [ $SERVER_STATUS = "on" ]; then
    echo "Server already started"
    exit 1
  else
    echo "Starting $SERVER_NAME Server..."
    "$MOHICANS_HOME"/mohican.out
    SERVER_STATUS="on"
    exit 0
  fi
}

stop_soft() {
	echo "Stopping Mohicans Server..."
	get_pid
	kill -1 "$PID_MASTER_PROCESS"
	echo "stop soft"
}

stop_hard() {
  echo "Stopping Mohicans Server..."
  get_pid
  kill -2 "$PID_MASTER_PROCESS"
  echo "stop hard"
  :> "$PID_FILE"
}

reload() {
  echo "Server reloading..."
  get_pid
  :> "$PID_FILE"
  kill -13 "$PID_MASTER_PROCESS"
}

status() {
# shellcheck disable=SC2046
  if [ $(head -n 1 "$PID_FILE") ]; then
    echo "$SERVER_NAME is running!!"
  else
    echo "$SERVER_NAME is down!!"
  fi
}

create_config() {
  cp "$DEFAULT_PATH_TO_CONFIG" settings/mohican.conf
}

  #read -n 1 -p "(нажмите любую клавишу для продолжения)"
  case $1 in
	  start)
	    start
	  ;;
    stop)
    shift
    case $1 in
      soft)
        stop_soft
        exit 0
      ;;
      hard)
        stop_hard
        exit 0
      ;;
    esac
    echo "Invalid key to cmd";
    ;;
  	status)
	    status
    ;;
    create)
      shift
      if [ "$1" =  config ]; then
        create_config
      fi
    ;;

	  *) echo "Usage : <start|stop|reload|status|create config>";
	  ;;
  esac
exit 0
