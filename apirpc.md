# XMAX RPC Interface description

Describe RPC interfaces of XMax chain. Use HTTP protocal.



# Table of contents
- [Blockchain API](#blockchainapi)
    - [get_account](#getaccount)
    - [get_table_rows](#gettablerows)
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

