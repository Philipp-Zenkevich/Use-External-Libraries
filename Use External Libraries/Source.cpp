#include "FinanceRates.h"
#include <locale.h>

int main(int argc, char* argv[])
{
	setlocale(0, "RU.UTF-8");
	SetConsoleOutputCP(1251);
	SetConsoleCP(1251);
	FinanceRates fr;
	fr.run();
	return 0;
}