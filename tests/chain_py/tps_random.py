#!/usr/bin/env python
#! -*- coding:utf-8 -*-

import random
import time

from xmax import account, trx, rpc, transfer

from urllib import request, parse

RPC_SERVER_POINT = 'http://127.0.0.1:18801'

GET_INFO_RPC = RPC_SERVER_POINT + '/v0/xmaxchain/get_info'

CREATOR_PRI_KEY = '5KDVLHu4YDA6bBnu9GQbr25saJoNZrHRb4mq1WQwDouhGizqQvU'
CREATOR_NAME = 'testerb'

OWNER_KEY = 'XMX5Wgr3AkX9k1hLjymep9snY2AwvXDAVJBTEQw38t49A8RCR2H3j'
ACTIVE_KEY = 'XMX7zXBwWFHgk9ovzZtUf5y3FK6Sk85biSbZgFTWTcnZewHaXUSCT'
ACTIVE_KEY_PRIVATE = '5KDVLHu4YDA6bBnu9GQbr25saJoNZrHRb4mq1WQwDouhGizqQvU'

RANCHARS =['a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','1','2','3','4']

NEWACC_PREFIX = 'tn'

def checkConnect():
    try:
        rpc.rpcCall(GET_INFO_RPC, "", False)
    except Exception:
        print('no connection, sleep...')
        return False
    else:
        return True