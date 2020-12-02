#include <iomanip>
#include <string>
#include <unistd.h>
#include "dbwrapper.h"
#include "varint.h"

static void showUsage(std::string name);

void setOutpointData(std::string& txidHexString, std::optional<uint32_t>& vout);

enum class Mode { single, dump_all };

int main(int argc, char* argv[])
{
	if (argc < 2) {
		showUsage(argv[0]);
	}

	std::string sourceDB, txidHexString;
	std::optional<uint32_t> vout;
	auto mode = Mode::single;
	std::string dbPath;
	bool processOptions = true;

	if (argc == 2) {
		dbPath = std::string(argv[1]);
		setOutpointData(txidHexString, vout);
		processOptions = false;
	}

	int opt;
	while (processOptions && (opt = getopt(argc, argv, "m:t:o:")) != -1) {
		switch (opt) {
			case 'm':
				if (strcmp("single", optarg)) { mode = Mode::dump_all; }
				break;
			case 't':
				txidHexString = std::string(optarg);
				break;
			case 'o':
				if (strcmp("0", optarg)) {
					vout = static_cast<uint32_t>(std::stoul(std::string(optarg)));
					if (vout == 0) {
						std::cerr << "Unrecognized value for vout.\n";
						showUsage(argv[0]);
					}
				} else {
					vout = 0;
				}
				break;
			default:
				showUsage(argv[0]);
		}
	} 
	
	// dbPath is the first non-getopt argument
	dbPath = processOptions ? argv[optind] : dbPath;

	// @TODO - Sanity check - valid path supplied?
	DBWrapper db(dbPath);
	
	if (mode == Mode::dump_all) {
	       	db.printAllUTXOs();
		return EXIT_SUCCESS;
	}
	
	// DBWrapper::fetchRecord() is overloaded. Pass in a string or a std::vector<unsigned char>.
	std::vector<unsigned char> valueBytes;
	try {
		db.fetchRecord(txidHexString, vout.value(), valueBytes);
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

void setOutpointData(std::string& txidHexString, std::optional<uint32_t>& vout)
{
	std::cout << "Enter a txid: ";
	std::cin >> txidHexString;
	std::cout << "Enter an output index: ";
	uint32_t tmp;
	std::cin >> tmp;
	vout.emplace(tmp);
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

