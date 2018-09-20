#!/usr/bin/env python
#! -*- coding:utf-8 -*-

import sys

from xmax import account
from urllib import request, parse

RPC_SERVER_POINT = 'http://127.0.0.1:18801'

PUSH_TRX_RPC = RPC_SERVER_POINT + '/v0/contractutil_plugin/fix_push_trx'
GET_ACC_RPC = RPC_SERVER_POINT + '/v0/xmaxchain/get_account'


def rpcCall(rpc, trxData, bprint):

    req = request.Request(rpc)
    #request.urlopen(req, data=trxData.encode('utf-8'))
    with request.urlopen(req, data=trxData.encode('utf-8')) as f:
        if bprint:
            # print('Status:', f.status, f.reason)
            # for k, v in f.getheaders():
            #   print('%s: %s' % (k, v))
            print('Data:', f.read().decode('utf-8'))

    return

# read account name.
accname = sys.argv[1] 

getjson = account.getAccountJson(accname)
rpcCall(GET_ACC_RPC, getjson, True)