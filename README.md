# Access Bitcoin UTXO Set With the Bitcoin Core chainstate Database 
C++ project to access Bitcoin UTXO data by parsing the `chainstate` database.

UTXOs
-----
The Unspent Transaction Output (UTXO) set is a subset of Bitcoin transaction outputs that are not yet spent.

For a new transaction to be valid, it must have access to UTXOs that can be used as inputs - the creator of the transaction must be able to meet the spending conditions of the transaction input UTXOs. Transactions consume UTXOs as inputs and create new UTXOs as outputs - with spending conditions locked such that the intended recipient can unlock the new UTXOs.

The UTXO set contains all unspent outputs - it therefore contains all the data necessary to validate new transactions.

UTXOs consist of two parts:

1. The amount transferred to the output
2. The locking script `scriptPubKey` that specifies the spending conditions for the output

Outpoints
---------
Each transaction (with the exception of coinbase transactions) spends a UTXO from a previous transaction. In the context of a transaction, UTXOs are referenced by "outpoints".

A single transaction can have multiple outputs - so each UTXO outpoint consists of their transaction ID (`txid`) and output index number (often referred to as `vout`).

Together, the `txid` and output index are known as the UTXO outpoint.

Local Database: chainstate
--------------------------
Bitcoin Core full nodes store UTXO data in the `chainstate` LevelDB database. Data is stored in a per-output model - each entry in the chainstate database represents a single UTXO.

UTXO data is stored in this way so that transactions can be validated and new transactions created without the necessity of checking the entire blockchain.

At the time of writing, the `chainstate` database is approximately 4GB in size.

Data Storage Format
-------------------
[LevelDB][8] is a simple on-disk key-value store. Keys and values are stored as strings in arbitrary byte arrays, sorted by key. Indexing is not supported. Though LevelDB keys & values are strings, they are not C-style null-terminated strings - this is because LevelDB keys & values may contain null bytes.

Chainstate Keys
---------------
Keys in the `chainstate` database consist of a little-endian representation of the txid prepended with the single byte `0x43` ('C') and appended with a Varint representation of the vout:

`0x43<txid, little endian><vout>`

Chainstate Values
-----------------
Values in the `chainstate` database contain the following data:

* Block height
* Whether or not the UTXO is a coinbase transaction
* Amount (in Satoshis)
* nSize - an indication of the type/size of locking script
* Locking script - hash 160 for P2PKH & P2SH, public key for P2PK, otherwise full script

Value Obfuscation
-----------------
Values in the `chainstate` database are obfuscated - this was a [change][11] added to the database in order to prevent false positives triggered in Windows anti-virus software. 

Obfuscation is a simple XOR operation against a repeated 8-byte obfuscation key. The obfuscation key is a random value unique to each node, with the obfuscation key stored in the `chainstate` database itself, under the key `0x0e00+"obfuscate_key"` ([see here][12]). 

This means that values must be XORed against the obfuscation key after they have been retrieved.

**TODO**: If this tool is expanded to provide analytic data on UTXOs - possibly by building a relational database from UTXO data - consider building a custom version of Bitcoin Core without obfuscation and a rebuild `chainstate` database. If dumping all UTXOs, this will likely provide a performance boost. This might be achieved by removing the following code and re-building the `chainstate` database - [see this note][13]:

```c++
    // The base-case obfuscation key, which is a noop.
    obfuscate_key = std::vector<unsigned char>(OBFUSCATE_KEY_NUM_BYTES, '\000');

    bool key_exists = Read(OBFUSCATE_KEY_KEY, obfuscate_key);

    if (!key_exists && obfuscate && IsEmpty()) {
        // Initialize non-degenerate obfuscation if it won't upset
        // existing, non-obfuscated data.
        std::vector<unsigned char> new_key = CreateObfuscateKey();

        // Write `new_key` so we don't obfuscate the key with itself
        Write(OBFUSCATE_KEY_KEY, new_key);
        obfuscate_key = new_key;

        LogPrintf("Wrote new obfuscate key for %s: %s\n", path.string(), HexStr(obfuscate_key));
    }

    LogPrintf("Using obfuscation key for %s: %s\n", path.string(), HexStr(obfuscate_key));
``` 
See [here][10].

Once the value has been de-obfuscated, data is stored in the following format:

`<Varint block height><Varint amount><nSize><locking script>`

Varints in the context of the `chainstate` database are described below.

Variable Length Integers: Varints
---------------------------------
Bitcoin uses Varints to transmit and store values where the minimum number of bytes required to store a value is not known.

For example, a block height that is less than or equal to 255 could be stored in a single byte whereas the block height 649392 would require three (unsigned) bytes:

| Value | Minimum Byte Representation |
|-|-|
| 649392 | 09 E8 B0 |
||(9 * 256²) + (232 * 256) + 176 = 649392 |

To efficiently allow for such variability, Bitcoin uses a system of variable-length integers such that a minimal amount of space is used to store integers, whilst allowing for integers to be as large or as small as necessary.

**Varints serialize integers into one or more bytes, with smaller numbers requiring fewer bytes to be encoded.**

### Varints vs compactSize Integers
Bitcoin has multiple methods for encoding variable length integers, with different methods used in different parts of the codebase.

The raw transaction format and peer-to-peer network messages within Bitcoin use a type of variable length integer encoding known as "compactSize". This involves prepending integers with a byte that indicates integer length for numbers greater than 252.

Used in the transaction format, compactSize integers format part of the [Bitcoin consensus rules][9].

This document and repo is concerned with Varints as this is the method which Bitcoin core uses to serialize data to disk in the LevelDB database.

Varints in the LevelDB chainstate Database
-------------------------------------------
In the context of storing data in the levelDB `chainstate` database (which stores UTXO data), integers are stored as base 128 encoded numbers. The last 7 bits in each byte are used to represent a digit, and the position of the byte represents the power of 128 to be multiplied.

This system leaves the most significant bit (MSB) of each byte available to carry information regarding whether or not the integer is complete.

If the MSB of a byte is set, the next digit (byte) should be read as part of the integer. If the leading digit is not set, the byte represents the final digit in the encoded base 128 integer.   

To ensure that each integer has a unique representation in the encoding system, 1 is subtracted from all bytes except for the byte representing the last digit.

| Decimal Number | Hexadecimal Representation | Binary |
|-|-|-|
| 128 | 0x80 0x00 | 1000 0000 0000 0000 |
| 256 | 0x81 0x00 | 1000 0001 0000 0000 |
| 65535 | 0x82 0xFE 0x7F | 1000 0010 1111 1110 0111 1111 |

This system is compact:
* Integers 0-127 are represented by 1 byte
* 128-16511 require 2 bytes
* 16512-2113663 require 3 bytes.

Each integer has a unique encoding, and the encoding is infinite in capacity - integers of any size can be represented.

Worked Manual Example
---------------------
This example takes a value from the `chainstate` database of UTXOs from Bitcoin Core and decodes the value to provide:

* Block height (First Varint, excluding least significant bit)
* Coinbase status (Last bit of first Varint)
* Amount (Second Varint)
* Script type (Third Varint)
* Unique script value (Remainder of the value)

This worked example is drawn from the README [this GitHub repo][3] for a Bitcoin chainstate parser in Ruby. 

Start value: `c0842680ed5900a38f35518de4487c108e3810e6794fb68b189d8b`

### First Varint: Block Height
|					| Byte₀		| Byte₁		| Byte₂		|
|-|-|-|-|
| Start, hexadecimal			|0xC0		|0x84		|0x26		|
| Start, binary				|1100 0000	|1000 0100	|0010 0110	|
| Last 7 bits of each byte		| 100 0000	| 000 0100	| 010 0110	|
| Add 1 to each byte except last	| 100 0001	| 000 0101	| 010 0110	|

tmp array:
||||
|-|-|-|
|0x41		|0x05		|0x26		|
|0100 0001	|0000 0101	|0010 0110	|

Remove last zero - flag showing coinbase status
|						| Byte₀		| Byte₁		| Byte₂		|
|-|-|-|-|
| Concatenate consecutive bits to get value	| 0000 1000	| 0010 0001	| 0101 0011	|
| Result, hexadecimal				| 0x08		| 0x21		| 0x53		|	
| Result, decimal				| 8		| 33		| 83		|	

In decimal: (8 * 256²) + (33 * 256) + 83 = 532819

### Second Varint: Amount
|					| Byte₀		| Byte₁		| Byte₂		|
|-|-:|-:|-:|
|					|0x80		|0xED		|0x59		|
|					|1000 0000	|1110 1101	|0101 1001	|
| Last 7 bits				| 000 0000	| 110 1101	| 101 1001	|
| Add 1 to each byte except last	| 000 0001	| 110 1110	| 101 1001	|
| Concatenate				|0000 0000	|0111 0111	|0101 1001	|
| Result, hexadecimal			|0x00		| 0x77		|0x59		|
| Result, decimal			|0		|119		|89		|

In decimal: (0 . 256²) + (119 * 256) + 89 = 30553

Usage
-----
@TODO

References
----------
* [LevelDB Project docs][8] - not very useful
* [Compact Integers in Bitcoin][9] - Bitcoin Developer on bitcoin.org
* [Variable length quantity][7], Wikipedia
* [Bitcoin SE answer on CVarint format in chainstate DB][2]
* [Bitcoin chainstate parser by in3rsha][3] - very useful README
* [https://jonnydee.wordpress.com/2011/05/01/convert-a-block-of-digits-from-base-x-to-base-y/][4]
* [Comment relating to Variable-length integers][5], Bitcoin Core `/src/serialize.h#L339`
* [SO Answer on Varint encoding in chainstate DB][6]
* [Remove lines for non-obfuscated values][10]
* [Obfuscate chainstate, PR #6650][11]

[1]: https://github.com/bitcoin/bitcoin/blob/v0.13.2/src/serialize.h#L307L372
[2]: https://bitcoin.stackexchange.com/a/51639/56514
[3]: https://github.com/in3rsha/bitcoin-chainstate-parser
[4]: https://jonnydee.wordpress.com/2011/05/01/convert-a-block-of-digits-from-base-x-to-base-y/
[5]: https://github.com/bitcoin/bitcoin/blob/master/src/serialize.h#L339
[6]: https://bitcoin.stackexchange.com/a/51639/56514
[7]: https://en.wikipedia.org/wiki/Variable-length_quantity
[8]: https://github.com/google/leveldb/blob/master/doc/index.md
[9]: https://developer.bitcoin.org/reference/transactions.html#compactsize-unsigned-integers
[10]: https://github.com/bitcoin/bitcoin/blob/80aa83aa406447d9b0932301b37966a30d0e1b6e/src/dbwrapper.cpp#L149-L166
[11]: https://github.com/bitcoin/bitcoin/pull/6650
[12]: https://github.com/csknk/parse-chainstate/blob/51434fbf8cfde3f19e2a3ac0ff8a5ee35259b6e0/DBWrapper.cpp#L24-L25
[13]: https://bitcoin.stackexchange.com/a/62700/56514

Working Notes
--------------
https://bitcoin.stackexchange.com/a/83423/56514
