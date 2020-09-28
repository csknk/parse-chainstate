#include "DBWrapper.h"
#include <iomanip>

int main(int argc, char* argv[])
{
	DBWrapper db("chainstate-2");
	if (argc == 2 && strcmp(argv[1], "true") == 0) {
		db.outputAllKeyVals();
		return EXIT_SUCCESS;
	}
	std::cout << "Enter a txid:";
	std::string txid;
	std::cin >> txid;
	std::string val;

	// DBWrapper::fetchRecord() is overloaded.
	// Pass in a string or a std::vector<unsigned char>.
	db.fetchRecord(txid, 0x00, 0x00,0x00,  val);
	std::cout << "value: " << val << "\n";

	std::vector<unsigned char> valueBytes;
	db.fetchRecord(txid, 0x00, valueBytes);

	for (const auto& el : valueBytes) {
		std::cout << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << (int)el;
	}
	std::cout << std::dec << "\n";
	return EXIT_SUCCESS;
}
