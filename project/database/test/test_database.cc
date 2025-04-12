#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define MYSQLPP_MYSQL_HEADERS_BURIED
#include <boost/test/unit_test.hpp>
#include <mysql++/mysql++.h>

using namespace std;

BOOST_AUTO_TEST_SUITE(DatabaseTest)

BOOST_AUTO_TEST_CASE(xxxx)
{
    mysqlpp::Connection conn;

    try
    {
        conn.connect("test_db", "127.0.0.1", "root", "200101", 3306);
        auto query = conn.query("SELECT user, host FROM mysql.user");
        auto result = query.store();
        if (result)
        {
            for (size_t i = 0; i < result.num_rows(); ++i) {
                BOOST_TEST_MESSAGE("user: " << result[i]["user"] << ", host: " << result[i]["host"]);
            }
        }
        else {
            BOOST_TEST_ERROR("query failed! >>" << query.error());
            return;
        }
    }
    catch(const std::exception& e)
    {
        BOOST_TEST_ERROR(e.what());
    }
    
}

BOOST_AUTO_TEST_CASE(t_mysqlpp_test)
{
    int i = 0;

    i = i + 1;

    i = i + 1;

    BOOST_TEST_MESSAGE("i = " << i);
}

BOOST_AUTO_TEST_SUITE_END()