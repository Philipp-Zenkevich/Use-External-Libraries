#include "FinanceRates.h"

namespace Fin
{
	class ValuteItem
	{
	public:
		std::string id;
		std::string numCode;
		std::string charCode;
		int nominal;
		std::string name;
		float value;
		float previos;
	};

	void from_json(const json& j, ValuteItem& v)
	{
		j.at("ID").get_to(v.id);
		j.at("NumCode").get_to(v.numCode);
		j.at("CharCode").get_to(v.charCode);
		j.at("Nominal").get_to(v.nominal);
		j.at("Name").get_to(v.name);
		j.at("Value").get_to(v.value);
		j.at("Previous").get_to(v.previos);
	}

	class ExchangeRates
	{
	public:
		std::string date;
		std::string previousDate;
		std::string previousURL;
		std::string timestamp;
		std::map<std::string, ValuteItem> valute;
	};

	void from_json(const json& j, ExchangeRates& er)
	{
		j.at("Date").get_to(er.date);
		j.at("PreviousDate").get_to(er.previousDate);
		j.at("PreviousURL").get_to(er.previousURL);
		j.at("Timestamp").get_to(er.timestamp);
		er.valute = j.at("Valute").get<std::map<std::string, ValuteItem>>();
	}
}

std::string FinanceRates::GetData()
{
	boost::asio::io_service io_service;
	tcp::resolver resolver(io_service);
	tcp::resolver::query query("www.cbr-xml-daily.ru", "http");
	tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
	tcp::socket socket(io_service);
	boost::asio::connect(socket, endpoint_iterator);
	boost::asio::streambuf request;
	std::ostream request_stream(&request);
	request_stream << "GET " << "/daily_json.js" << " HTTP/1.1\r\n";
	request_stream << "Host: " << "www.cbr-xml-daily.ru" << "\r\n";
	request_stream << "Accept: */*\r\n";
	request_stream << "Connection: close\r\n\r\n";
	boost::asio::write(socket, request);
	boost::asio::streambuf response;
	boost::asio::read_until(socket, response, "\r\n");
	std::istream response_stream(&response);
	std::string http_version;
	response_stream >> http_version;
	unsigned int status_code;
	response_stream >> status_code;
	std::string status_message;
	std::getline(response_stream, status_message);

	if (!response_stream || http_version.substr(0, 5) != "HTTP/")
	{
		std::cout << "Invalid response\n";
	}
	if (status_code != 200)
	{
		std::cout << "Response returned with status code " << status_code << "\n";
	}

	boost::asio::read_until(socket, response, "\r\n\r\n");
	std::string header;
	while (std::getline(response_stream, header) && header != "\r");
	std::getline(response_stream, header);
	boost::system::error_code error;

	if (response.size() > 0)
	{
		boost::asio::read(socket, response, boost::asio::transfer_all(), error);
		std::stringstream ss;
		ss << &response;
		std::string res = ss.str();
		return res;
	}

	if (error != boost::asio::error::eof)
		throw boost::system::system_error(error);
}

void FinanceRates::ReadKeys()
{
	while (!GetAsyncKeyState(27));
	exit(0);
}

void FinanceRates::printTable(const boost::system::error_code& e, boost::asio::deadline_timer* pt, int* pcont)
{
	system("cls");

	Fin::ExchangeRates rates;
	std::string str = FinanceRates::GetData();
	json j;
	std::stringstream ss(str);
	ss >> j;

	rates = j.get<Fin::ExchangeRates>();

	std::cout << rates.timestamp << std::endl;
	for (const auto& v : rates.valute)
	{
		std::cout << v.second.name << std::endl;
		std::cout << std::left << std::setw(45) << "(" + v.second.charCode + ")";
		std::cout << std::right << v.second.value << std::endl;
		/*if (FinanceRates::rateValues.find(v.second.charCode) == FinanceRates::rateValues.end())
			FinanceRates::rateValues.insert(std::pair<std::string, std::vector<float>>(v.second.charCode, std::vector<float>()));
		else
			FinanceRates::rateValues.at(v.second.charCode).push_back(v.second.value);*/

	}
	++(*pcont);
	pt->expires_at(pt->expires_at() + boost::posix_time::seconds(10));
	pt->async_wait(boost::bind(printTable, boost::asio::placeholders::error, pt, pcont));
	std::cout << "Press ESC to stop\n";
}

void FinanceRates::run()
{
	std::thread escThread(FinanceRates::ReadKeys);
	int count = 0;
	boost::asio::io_service io;
	boost::asio::deadline_timer t(io, boost::posix_time::seconds(0));
	t.async_wait(boost::bind(FinanceRates::printTable, boost::asio::placeholders::error, &t, &count));
	io.run();
	escThread.join();

	system("cls");
	/*for (const auto& v : rateValues)
	{
		double sum = 0;
		for (double c : v.second)
		{
			sum += c;
		}
		std::cout << std::left << std::setw(45) << v.first;
		std::cout << std::right << sum << std::endl;
	}*/
}
