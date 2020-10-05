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

Local Database: chainstate
--------------------------
Bitcoin Core stores UTXO data in the `chainstate` LevelDB database. Data is stored in a per-output model - entries in the chainstate database represent single UTXOs.


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

Varints in the LevelDB chainstate Database
-------------------------------------------
In the context of storing data in the levelDB `chainstate` database (which stores UTXO data), integers are stored as MSB base 128 encoded numbers. The last 7 bits in each byte is used to represent a digit, and the position of the byte represents the power of 128 to be multiplied.

This system leaves the most significant bit (MSB) of each byte available to carry information regarding whether or not the integer is complete.

If the MSB of a byte is set, the next digit (byte) should be read as part of the integer. If the leading digit is not set, the byte represents the final digit in the encoded integer.   

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
| | Byte₀| Byte₁ | Byte₂ |
|-|-|-|-|
| Start, hexadecimal | 0xC0 | 0x84 | 0x26 |
| Start, binary | 1100 0000 | 1000 0100 | 0010 0110 |
| Last 7 bits of each byte | 100 0000 |	000 0100 | 010 0110 |
| Add 1 to each byte except last | 100 0001 | 000 0101 | 010 0110 |

tmp array:
||||
|-|-|-|
| 0x41		| 0x05		| 0x26		|
| 100 0001	| 000 0101	| 010 0110	|

Remove last zero - flag showing coinbase status
| | Byte₀| Byte₁ | Byte₂ |
|-|-|-|-|
| Concatenate consecutive bits to get value	| 0000 1000	| 0010 0001	| 0101 0011	|
| Result, hexadecimal				| 0x08		| 0x21		| 0x53		|	
| Result, decimal				| 8		| 33		| 83		|	

In decimal: (8 * 256²) + (33 * 256) + 83 = 532819

### Second Varint: Amount
| | Byte₀| Byte₁ | Byte₂ |
|-|-:|-:|-:|
|					|0x80		|0xED		|0x59		|
|					|1000 0000	|1110 1101	|0101 1001	|
| Last 7 bits				| 000 0000	| 110 1101	| 101 1001	|
| Add 1 to each byte except last	| 000 0001	| 110 1110	| 101 1001	|
| Concatenate				|0000 0000	|0111 0111	|0101 1001	|
| Result, hexadecimal			|0x00		| 0x77		|0x59		|
| Result, decimal			|00		|119		|89		|

In decimal: (119 * 256) + 89 = 30553

Usage
-----
@TODO

References
----------
* [LevelDB Project docs][8] - not very useful
* [Variable length quantity][7], Wikipedia
* [Bitcoin SE answer on CVarint format in chainstate DB][2]
* [Bitcoin chainstate parser by in3rsha][3] - very useful README
* [https://jonnydee.wordpress.com/2011/05/01/convert-a-block-of-digits-from-base-x-to-base-y/][4]
* [Comment relating to Variable-length integers][5], Bitcoin Core `/src/serialize.h#L339`
* [SO Answer on Varint encoding in chainstate DB][6]

[1]: https://github.com/bitcoin/bitcoin/blob/v0.13.2/src/serialize.h#L307L372
[2]: https://bitcoin.stackexchange.com/a/51639/56514
[3]: https://github.com/in3rsha/bitcoin-chainstate-parser
[4]: https://jonnydee.wordpress.com/2011/05/01/convert-a-block-of-digits-from-base-x-to-base-y/
[5]: https://github.com/bitcoin/bitcoin/blob/master/src/serialize.h#L339
[6]: https://bitcoin.stackexchange.com/a/51639/56514
[7]: https://en.wikipedia.org/wiki/Variable-length_quantity
[8]: https://github.com/google/leveldb/blob/master/doc/index.md

