package database

import (
	"fmt"

	"gopkg.in/ini.v1"
)

var config struct {
	MySQL struct {
		IP       string
		Port     int
		User     string
		Password string
		Database string
	}
	Service struct {
		Listen string
	}
	Log struct {
		Level string
		File  string
	}
}

func LoadConfig(filePath string) error {
	cfg, err := ini.Load(filePath)
	if err != nil {
		return fmt.Errorf("failed to load config file: %v", err)
	}

	// Map [mysql] section
	config.MySQL.IP = cfg.Section("mysql").Key("ip").String()
	config.MySQL.Port, _ = cfg.Section("mysql").Key("port").Int()
	config.MySQL.User = cfg.Section("mysql").Key("user").String()
	config.MySQL.Password = cfg.Section("mysql").Key("password").String()
	config.MySQL.Database = cfg.Section("mysql").Key("database").String()

	// Map [service] section
	config.Service.Listen = cfg.Section("service").Key("listen").String()

	// Map [log] section
	config.Log.Level = cfg.Section("log").Key("level").String()
	config.Log.File = cfg.Section("log").Key("file").String()

	return nil
}
