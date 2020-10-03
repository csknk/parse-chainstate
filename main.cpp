#include <iomanip>
#include "DBWrapper.h"
#include "bitcoin-varint/varint.h"

int main(int argc, char* argv[])
{
	DBWrapper db("chainstate-2");
	if (argc == 2 && strcmp(argv[1], "true") == 0) {
//		db.outputAllKeyVals();
//		std::vector<Coin> utxos;
//		db.outputAllKeyVals();
		return EXIT_SUCCESS;
	}
	std::cout << "Enter a txid:";
	std::string txid;
	std::cin >> txid;
	std::string val;

	// DBWrapper::fetchRecord() is overloaded.
	// Pass in a string or a std::vector<unsigned char>.
	std::vector<unsigned char> valueBytes;
	db.fetchRecord(txid, 0x00, valueBytes);

	// API design
	Varint v(valueBytes);
	UTXO u(v);
	std::cout << u << "\n";
	
	return EXIT_SUCCESS;
}
