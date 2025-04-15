#!/bin/bash

## 默认配置
pid_file="pidFile.tmp"
config_dir="../project"

## 参数解析
usage() {
    echo "Usage: $0 [-p pid_file] [-c config_dir] [-r] [-s]"
    echo "  -p pid_file   Specify the PID file (default: pidFile.tmp)"
    echo "  -c config_dir Specify the configuration directory (default: ../project)"
    echo "  -restart      Restart all programs"
    echo "  -stop         Stop all programs listed in the PID file"
    exit 1
}

action=""

while getopts "p:c:restartstoph" opt; do
    case $opt in
        p) pid_file="$OPTARG" ;;
        c) config_dir="$OPTARG" ;;
        r) action="restart" ;;
        s) action="stop" ;;
        h) usage ;;
        *) usage ;;
    esac
done

## 检查所有服务的exe文件是否存在
check_exe() {
    if [ ! -f "$1" ]; then
        echo "Error: $1 not found!"
        exit 1
    fi
}


## 重启所有服务
restart_all() {
    stop_all

    sleep 0.5

    run_all
}

## 启动所有服务
run_all() {
    check_exe "../build/bin/database/database"
    check_exe "../build/bin/monitor/monitor"
    check_exe "../build/bin/gateway/gateway"

    ## 清空 pidFile.tmp 文件
    > "$pid_file"

    ## 运行所有可执行程序
    echo "Starting all servers..."

    ## 检查并启动 database 服务
    if pgrep -f "../build/bin/database/database" > /dev/null; then
        echo "Database service is already running."
        exit -1
    else
        nohup ./../build/bin/database/database -c "$config_dir/database/config.ini" > /dev/null 2>&1 &
        echo $! >> "$pid_file"
        echo "Database service started."
    fi

    sleep 0.2

    ## 检查并启动 monitor 服务
    if pgrep -f "../build/bin/monitor/monitor" > /dev/null; then
        echo "Monitor service is already running."
        exit -1
    else
        nohup ./../build/bin/monitor/monitor -c "$config_dir/monitor/config.ini" > /dev/null 2>&1 &
        echo $! >> "$pid_file"
        echo "Monitor service started."
    fi

    sleep 0.2

    ## 检查并启动 gateway 服务
    if pgrep -f "../build/bin/gateway/gateway" > /dev/null; then
        echo "Gateway service is already running."
        exit -1
    else
        nohup ./../build/bin/gateway/gateway -c "$config_dir/gateway/config.ini" > /dev/null 2>&1 &
        echo $! >> "$pid_file"
        echo "Gateway service started."
    fi

    echo "All servers started. PIDs written to $pid_file."
}

## 停止所有服务
stop_all() {
    if [ ! -f "$pid_file" ]; then
        echo "Error: PID file $pid_file not found!"
        exit 1
    fi

    echo "Stopping all servers..."
    while read -r pid; do
        if kill -0 "$pid" 2>/dev/null; then
            kill "$pid"
            echo "Stopped process with PID: $pid"
        else
            echo "Process with PID $pid not found or already stopped."
        fi
    done < "$pid_file"

    ## 清空 PID 文件
    > "$pid_file"
    echo "All servers stopped."
}

## 根据选项执行操作
case $action in
    restart) restart_all ;;
    stop) stop_all ;;
    *) usage ;;
esac
