#include <iomanip>
#include <string>
#include <unistd.h>
#include "dbwrapper.h"
#include "varint.h"

static void showUsage(std::string name);

enum class Mode { single, dump_all };

int main(int argc, char* argv[])
{
	if (argc < 2) {
		showUsage(argv[0]);
	}

	std::string sourceDB, txidHexString;
	auto mode = Mode::single;
	int opt;
	while ((opt = getopt(argc, argv, "m:t:")) != -1) {
		switch (opt) {
			case 'm':
				if (strcmp("single", optarg)) { mode = Mode::dump_all; }
				break;
			case 't':
				txidHexString = std::string(optarg);
				break;
			default:
				showUsage(argv[0]);
		}
	} 
	
	std::cout << "Mode is: " << (mode == Mode::single ? "single" : "dump_all") << "\n";
	std::cout << "txid is: " << (txidHexString == "" ? "Not set." : txidHexString) << "\n";
	std::cout << "There are " << argc - optind  << " remaining elements.\n";
	std::cout << "db is: " << argv[optind] << "\n";

	std::string dbPath(argv[optind]);
	std::cout << "dbPath is: " << dbPath << "\n";

	// Samity check - valid path supplied?
	DBWrapper db(dbPath);
	
	if (mode == Mode::dump_all) {
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

static void showUsage(std::string name)
{
    std::cerr << "Usage: " << name << " <option(s)> SOURCES\n"
              << "Options:\n"
              << "-h,--help\t\tShow this help message\n"
              << "-m,--mode <mode>\t\"dump_all\" to dump all UTXOs, \"single\" for a single txid. "
	      << "Default is \"single\".\n"
	      << "-t, --txid <txid>\tHexstring representation of UTXO to lookup. "
	      << "If mode is \"t\" and no txid is provided, user will be prompted to enter one."
              << std::endl;
    exit(EXIT_FAILURE);
}

