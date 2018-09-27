#!/usr/bin/env python
#! -*- coding:utf-8 -*-

from xmax import utils

TRANSFER_FROM_NAME = '@{from}'
TRANSFER_TO_NAME = '@{to}'
TRANSFER_AMOUNT_NAME = '@{amount}'
TRANSFER_MEMO_NAME = '@{memo}'

SIMPLE_TRANSFER_JSON = '\
{\
	"code": "xmax",\
	"type": "transfer",\
	"authorization": [{\
		"account": @{from},\
		"permission": "active"\
	}],\
	"data": {\
		"from":@{from},\
		"to": @{to},\
		"amount": @{amount},\
		"memo": @{memo}\
	}\
}'\