# Access Bitcoin UTXO Set With the Bitcoin Core chainstate Database 

The Unspent Transaction Output (UTXO) set is a subset of Bitcoin transaction outputs that are not yet spent.

For a new transaction to be valid, it must have access to UTXOs that can be used as inputs - the creator of the transaction must be able to meet the spending conditions of the transaction input UTXOs. Transactions consume UTXOs as inputs and create new UTXOs as outputs - with spending conditions locked such that the intended recipient can unlock the new UTXOs

The UTXO set contains all unspent outputs - it therefore contains all the data necessary to validate new transactions.

UTXOs consist of two parts:

1. The amount transferred to the output
2. The locking script `scriptPubKey` that specifies the spending conditions for the output

Local Database: chainstate
--------------------------
Bitcoin Core stores UTXO data in the `chainstate` LevelDB database. Data is stored in a per-output model - entries in the chainstate database represent single UTXOs.


References
----------
* [Project docs][1]

[1]: https://github.com/google/leveldb/blob/master/doc/index.md

