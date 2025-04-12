#include <database/dbhelper/MySqlClient.hpp>

namespace service::database
{

MySqlClient::MySqlClient()
{

}

MySqlClient::~MySqlClient()
{
    mysqlpp::ConnectionPool::clear();
}

bbt::core::errcode::ErrOpt MySqlClient::Query(const std::string& sql)
{
    auto conn = mysqlpp::ConnectionPool::grab();

    if (conn == nullptr)
        return bbt::core::errcode::Errcode("Failed to grab connection", ERR_UNKNOWN);
    
    mysqlpp::Query query_obj = conn->query(sql);

    auto result = query_obj.execute();

    result.info();

    return std::nullopt;
}

} // namespace service::database