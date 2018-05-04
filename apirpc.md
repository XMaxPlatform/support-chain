# XMAX RPC Interface description

Describe RPC interfaces of XMax chain. Use HTTP protocal.



# Table of contents
- [Blockchain API](#blockchainapi)
    - [get_account](#getaccount)
    - [get_table_rows](#gettablerows)
    - [get_info](#getinfo)
    - [get_block](#getblock)
- [ContractUtil API](#contractutilapi)
    - [create_account](#createaccount)
    - [push_transaction](#pushtransaction)



<a name="blockchainapi"></a>
## Blockchain API 

<a name="getaccount"></a>
### get_account

Get an account's information in blockchain.

#### Example:

```bash
$ curl http://127.0.0.1:18888/v0/xmaxchain/get_account -X POST -d '{"account_name":"testera"}'
```

<a name="gettablerows"></a>
### get_table_rows
Retrive contract data from an account.

#### Example:
```bash
$ curl http://127.0.0.1:18888/v0/xmaxchain/get_table_rows -X POST -d '{"scope":"testera", "code":"currency", "table":"account", "json":"true"}'
$ curl http://127.0.0.1:18888/v0/xmaxchain/get_table_rows -X POST -d '{"scope":"testera", "code":"currency", "table":"account", "json":"true", "lower_bound":0, "upper_bound":-1, "limit":10}'
```

<a name="getinfo"></a>
### get_info
Get blockchain infomation

#### Example:
```bash
$ curl http://127.0.0.1:18888/v0/xmaxchain/get_info
```

<a name="getblock"></a>
### get_block
Get specific block info use id or block num.

#### Example
```bash
$ curl http://127.0.0.1:18888/v0/xmaxchain/get_block -X POST -d '{"block_num_or_id":3}'
$ curl http://127.0.0.1:18888/v0/xmaxchain/get_block -X POST -d '{"block_num_or_id":000000034e41cef56934029d3e1be3ea812169320934c42386adec9f8bb94710}'
```


<a name="contractutilapi"></a>
## ContractUtil API

<a name="createaccount"></a>
### create_account
Create an new account

#### Example:

```bash
$ curl --data-binary '["testera", "newacc01", "5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3", "XMX7bmwBdhswDgwMNwVi7qXukb9F2qLBdbnvhHjhLWRKMXxVxAYSp", "XMX7bmwBdhswDgwMNwVi7qXukb9F2qLBdbnvhHjhLWRKMXxVxAYSp"]' http://127.0.0.1:18888/v0/contractutil_plugin/create_account
```

<a name="pushtransaction"></a>
### push_transaction
Commit a transaction for execution.

#### Example:
```bash
$ curl --data-binary '["5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3", "testera", ["testera", "testerb"], {"code":"currency", "type":"transfer", "authorization":[{"account":"testerb","permission":"active"}], "data":{"from":"testera", "to":"testerb", "quantity":1000}}]' http://127.0.0.1:18888/v0/contractutil_plugin/push_transaction
```

