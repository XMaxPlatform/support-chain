#!/usr/bin/env python
#! -*- coding:utf-8 -*-

from urllib import request, parse

PUSH_TRX_URL_SUFFIX = '/v0/contractutil_plugin/fix_push_trx'
GET_ACC_URL_SUFFIX = '/v0/xmaxchain/get_account'

# bprint: if true print post data.
def rpcCall(rpcUrl, trxData, bprint):

    req = request.Request(rpcUrl)
    #request.urlopen(req, data=trxData.encode('utf-8'))
    with request.urlopen(req, data=trxData.encode('utf-8')) as f:
        if bprint:
            # print('Status:', f.status, f.reason)
            # for k, v in f.getheaders():
            #   print('%s: %s' % (k, v))
            print('Data:', f.read().decode('utf-8'))

    return


def pushTrxRpc(addr, trxData, bprint):
    url = addr + PUSH_TRX_URL_SUFFIX
    rpcCall(url, trxData, bprint)
    return

def getAccountRpc(addr, data, bprint):
    url = addr + GET_ACC_URL_SUFFIX
    rpcCall(url, data, bprint)
    return