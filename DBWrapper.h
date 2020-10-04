#ifndef DBWRAPPER_H
#define DBWRAPPER_H 

#include <iostream>
#include <vector>
#include <stdexcept>
#include <memory>
#include "leveldb/db.h"
#include "varint.h"

typedef std::vector<unsigned char> BytesVec;

class DBWrapper {
	private:
		std::string obfuscationKeyKey;
		BytesVec obfuscationKey;
		leveldb::Options options;
		std::string dbName;
		leveldb::ReadOptions readoptions;
		leveldb::DB* db;
		leveldb::Status status;
		void checkStatus(std::string msg);
		void setObfuscationKey();
		void openDB();
		template <typename T> void deObfuscate(T bytes, BytesVec& plaintext);

	public:
		DBWrapper();
		~DBWrapper();
		DBWrapper(std::string dbName);
		void setDBName(const std::string& s);
		void read(std::string const& key, std::string& val);
		void outputAllKeyVals();
		void getAllUTXOs(std::vector<UTXO>& utxos);
		void fetchRecord(const std::string& key, const uint32_t vout, std::string& value);
		void fetchRecord(const std::string& key, const uint32_t vout, BytesVec& value);
};

/**
 * XOR `bytes` against the corresponding byte in the obfuscation key.
 * The obfuscation repeats as necessary.
 *
 * */
template <typename T>
void DBWrapper::deObfuscate(T bytes, BytesVec& plaintext)
{
	for (size_t i = 0, j = 0; i < bytes.size(); i++) {
		plaintext.push_back(obfuscationKey[j++] ^ bytes[i]);
		if (j == obfuscationKey.size()) j = 0;	
	}
}	

#endif /* DBWRAPPER_H */
