#include <iostream>
#include <cassert>
#include <string>
#include <iomanip>

#include "leveldb/db.h"

template <typename T> void printToHex(T s);
void stringToHexBytes(const std::string& s, std::vector<unsigned char>& hexBytes); 

int main(int argc, char* argv[])
{
	std::string dbName = "testdb";
	if (argc == 2) {
		dbName = argv[1];
	}

	std::cout << "Opening " << dbName << "\n";

	leveldb::DB* db;
	leveldb::Options options;
	options.create_if_missing = false;



//	leveldb::Status status = leveldb::DB::Open(options, "testdb", &db);
	leveldb::Status status = leveldb::DB::Open(options, dbName, &db);
	if (!status.ok()) std::cerr << status.ToString() << std::endl;
	assert(status.ok());

	// Read from db
	leveldb::ReadOptions readoptions;
	readoptions.verify_checksums = true;
	std::string keyName = {0x0e, 0x00};
	std::string obKey;
	keyName += "obfuscate_key";
	status = db->Get(readoptions, keyName, &obKey);
	assert(status.ok());

	printToHex(keyName);
	std::vector<unsigned char> xorKey;
	stringToHexBytes(obKey, xorKey);

	std::cout << "obfuscate_key: ";
	printToHex(obKey);
	/*
	leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
	for (it->SeekToFirst(); it->Valid(); it->Next()) {
		std::cout << it->key().ToString() << ": " << it->value().ToString() << "\n";
	}
	assert(it->status().ok());
	delete it;
	*/
	std::cout << "XOR key: ";	
	printToHex(xorKey);
	return 0;
}

void stringToHexBytes(const std::string& s, std::vector<unsigned char>& hexBytes)
{
	for (size_t i = 0; i < s.size(); i++) {
		if ((unsigned char)s[i] == 0xff) continue;
		hexBytes.push_back(s[i]);
	}
}
/**
 * Print a string as hexadecimal values.
 *
 * */
template <typename T>
void printToHex(T s)
{
	for(size_t i = 0; i < s.size(); i++) {
		std::cout << std::hex << std::setfill('0') << std::setw(2) << (int)s[i];
	}
	std::cout << '\n';
}

