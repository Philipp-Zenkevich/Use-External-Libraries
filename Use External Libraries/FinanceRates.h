#include <iostream>
#include <iomanip>
#include <istream>
#include <ostream>
#include <vector>
#include <map>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <Windows.h>
#include <string>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

using boost::asio::ip::tcp;

class FinanceRates
{
private:
	static std::map<std::string, std::vector<float>> rateValues;
	static std::string GetData();
	static void ReadKeys();
	static void printTable(const boost::system::error_code& e, boost::asio::deadline_timer* pt, int* pcont);
public:
	void run();
};
