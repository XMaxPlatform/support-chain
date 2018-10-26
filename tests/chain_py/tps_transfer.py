#!/usr/bin/env python
#! -*- coding:utf-8 -*-

import random
import time

from xmax import account, trx, rpc, transfer

from urllib import request, parse

RPC_SERVER_POINT = 'http://127.0.0.1:18801'

CREATOR_PRI_KEY = '5KDVLHu4YDA6bBnu9GQbr25saJoNZrHRb4mq1WQwDouhGizqQvU'
CREATOR_NAME = 'testerb'

OWNER_KEY = 'XMX5Wgr3AkX9k1hLjymep9snY2AwvXDAVJBTEQw38t49A8RCR2H3j'
ACTIVE_KEY = 'XMX7zXBwWFHgk9ovzZtUf5y3FK6Sk85biSbZgFTWTcnZewHaXUSCT'
ACTIVE_KEY_PRIVATE = '5KDVLHu4YDA6bBnu9GQbr25saJoNZrHRb4mq1WQwDouhGizqQvU'

RANCHARS =['a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','1','2','3','4']

NEWACC_PREFIX = 'tn'

def transferToken(fromAcc, toAcc, amount, memo):
    newjson = transfer.transferJson(fromAcc, toAcc, amount, memo)

    scopes = [fromAcc, toAcc]

    trxjson = trx.formatTrxJson([newjson], scopes)

    postjson = trx.formatPostJson([ACTIVE_KEY_PRIVATE], [trxjson])

    rpc.pushTrxRpc(RPC_SERVER_POINT, postjson, False)
    return

def transferTest(Acc1, Acc2, amount, count):
    newjson1 = transfer.transferJson(Acc1, Acc2, amount, "")
    newjson2 = transfer.transferJson(Acc2, Acc1, amount, "")
    scopes = [Acc1, Acc2]

    msgs = []

    for idx in range(0, count):
        if(idx % 2 == 0):
            msgs.append(newjson1)
        else:
            msgs.append(newjson2)

    trxjson = trx.formatTrxJson(msgs, scopes)

    postjson = trx.formatPostJson([ACTIVE_KEY_PRIVATE], [trxjson])

    rpc.pushTrxRpc(RPC_SERVER_POINT, postjson, False)
    return  

def newAccount(accname, amount):
    newaccjson = account.newAccountJson(CREATOR_NAME, accname, amount, OWNER_KEY, ACTIVE_KEY)

    scopes = [CREATOR_NAME]

    trxjson = trx.formatTrxJson([newaccjson], scopes)

    postjson = trx.formatPostJson([CREATOR_PRI_KEY], [trxjson])

    print('new account: ' + accname)

    rpc.pushTrxRpc(RPC_SERVER_POINT, postjson, False)
    return

def getAccount(accname):
    getjson = account.getAccountJson(accname)
    rpc.getAccountRpc(RPC_SERVER_POINT, getjson, True)
    return

TEST_TRANSFER_COUNT = 10
TEST_MEGS_IN_TRX = 300

SLEEP_TIME = 0.7


ranchars = random.sample(RANCHARS, 4)
rstr = ''
rstr = rstr.join(ranchars)

prefix = NEWACC_PREFIX + rstr + '.'

acca = prefix + 'a'
accb = prefix + 'b'

newAccount(acca, 100)
newAccount(accb, 100)

time.sleep(1)

getAccount(acca)
getAccount(accb)

print('transfer begin...')

for idx in range(0, TEST_TRANSFER_COUNT):

    print('batch num: ' + str(idx))

    transferTest(acca, accb, 1, TEST_MEGS_IN_TRX)
    time.sleep(SLEEP_TIME)
        
print('transfer end...')