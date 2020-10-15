#include <iomanip>
#include "dbwrapper.h"
#include "varint.h"

int main(int argc, char* argv[])
{
//	DBWrapper db("chainstate-2");
	DBWrapper db("mainnet-chainstate");
	if (argc == 2 && strcmp(argv[1], "true") == 0) {
	       	db.printAllUTXOs();
		return EXIT_SUCCESS;
	}
	std::cout << "Enter a txid: ";
	std::string txid;
	std::cin >> txid;

	std::cout << "Enter an output index: ";
	uint32_t vout;
	std::cin >> vout;

	// DBWrapper::fetchRecord() is overloaded.
	// Pass in a string or a std::vector<unsigned char>.
	std::vector<unsigned char> valueBytes;
	try {
		db.fetchRecord(txid, vout, valueBytes);
	}
	catch (const std::invalid_argument& e) {
		std::cerr << "Error. " << e.what() << "\n";
		return	EXIT_FAILURE;
	}
	// Initialise UTXO with a Varint constructed from a vector of bytes retrieved from the chainstate db. 
	Varint v(valueBytes);
	UTXO u(v);
	std::cout << u << "\n";
	
	return EXIT_SUCCESS;
}
