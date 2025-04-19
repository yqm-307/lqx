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

run_service() {
    local service_name="$1"
    local config_file="$2"

    if pgrep -f "$service_name" > /dev/null; then
        echo "$service_name is already running."
        exit -1
    else
        nohup "$service_name" "$config_file" > /dev/null 2>&1 &
        echo $! >> "$pid_file"
        echo "$service_name started."
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
    check_exe "../build/bin/scene/scene"

    ## 清空 pidFile.tmp 文件
    > "$pid_file"

    ## 运行所有可执行程序
    echo "Starting all servers..."

    run_service "../build/bin/database/database" "$config_dir/database/config.ini"
    run_service "../build/bin/monitor/monitor" "$config_dir/monitor/config.ini"
    run_service "../build/bin/gateway/gateway" "$config_dir/gateway/config.ini"
    run_service "../build/bin/scene/scene" "$config_dir/scene/config.ini"

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
