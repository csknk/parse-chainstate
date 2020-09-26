#include "DBWrapper.h"
//#include "utilities.h"

int main(int argc, char* argv[])
{
	DBWrapper db("chainstate-2");
	if (argc == 2 && strcmp(argv[1], "true") == 0) {
		db.outputAllKeyVals();
		return EXIT_SUCCESS;
	}
//	db.outputAllKeyVals();
	std::cout << "Enter a txid:";
	std::string txid;
	std::cin >> txid;
	std::string val;
	db.fetchRecord(txid, val);
	std::cout << "value: " << val << "\n";

	std::vector<unsigned char> valueBytes;
	db.fetchRecord(txid, valueBytes);
//	utilities::printToHex(valueBytes);
	return EXIT_SUCCESS;
}
