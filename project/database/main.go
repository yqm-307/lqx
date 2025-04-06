package database

import (
	database "database/src"
)

func main() {
	// 读取配置文件
	if err := database.LoadConfig("config.ini"); err != nil {
		panic(err)
	}
}
