#define MYSQLPP_MYSQL_HEADERS_BURIED
#include <mysql++/mysql++.h>
#include <bbt/core/errcode/Errcode.hpp>
#include <database/Define.hpp>

namespace service::database
{

class MySqlClient:
    mysqlpp::ConnectionPool
{
public:
    MySqlClient();
    ~MySqlClient();

    bbt::core::errcode::ErrOpt ConnectTo(const std::string& host, const std::string& user,
              const std::string& password, const std::string& dbname,
              unsigned int port = 3306);

    bbt::core::errcode::ErrOpt Query(const std::string& sql);

private:

};

} // namespace database