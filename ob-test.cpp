#include "DBWrapper.h"

int main(int argc, char* argv[])
{
	DBWrapper db("chainstate");
//	db.outputAllKeyVals();
	std::string txid = "ce70420880f1e3e5fa290cf20306ba2552b0c645d74d1e15213830c297063b1d";
	std::string val;
	db.fetchRecord(txid, val);
	return EXIT_SUCCESS;
}
