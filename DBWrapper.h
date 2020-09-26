#ifndef DBWRAPPER_H
#define DBWRAPPER_H 

#include <iostream>
#include <vector>
#include <stdexcept>
#include "leveldb/db.h"
//#include "utilities.h"

//static const *char[] = {
//	"P2PKH",	// data is the hash160 public key
//	"P2SH",		// data is the hash160 of a script
//	"P2PK",		// data is a compressed public key (nsize makes up part of the public key) [y=even]
//	"P2PK",		// data is a compressed public key (nsize makes up part of the public key) [y=odd]
//	"P2PK",		// data is an uncompressed public key (but has been compressed for leveldb) [y=even]
//	"P2PK"		// data is an uncompressed public key (but has been compressed for leveldb) [y=odd]
//};

struct UTXO {
	std::vector<unsigned char> scriptPubKey;
	int vout;
	bool coinbase;
	int height;
	short scriptType;
};

class DBWrapper {
	private:
		std::string obfuscationKeyKey;
		std::vector<unsigned char> obfuscationKey;
		leveldb::Options options;
		std::string dbName;
		leveldb::ReadOptions readoptions;
		leveldb::DB* db;
		leveldb::Status status;
		void checkStatus(std::string msg);
		void setObfuscationKey();
		void openDB();
		template <typename T> void deObfuscate(T bytes, std::vector<unsigned char>& plaintext);

	public:
		DBWrapper();
		~DBWrapper();
		DBWrapper(std::string dbName);
		void setDBName(const std::string& s);
		void read(std::string const& key, std::string& val);
		void outputAllKeyVals();
		void fetchRecord(const std::string& key, std::string& value);
		void fetchRecord(const std::string& key, std::vector<unsigned char>& value);
};

/**
 * XOR `bytes` against the corresponding byte in the obfuscation key.
 * The obfuscation repeats as necessary.
 *
 * */
template <typename T>
void DBWrapper::deObfuscate(T bytes, std::vector<unsigned char>& plaintext)
{
	for (size_t i = 0, j = 0; i < bytes.size(); i++) {
		plaintext.push_back(obfuscationKey[j++] ^ bytes[i]);
		if (j == obfuscationKey.size()) j = 0;	
	}
}	

#endif /* DBWRAPPER_H */
