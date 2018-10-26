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
		"authority": "active"\
	}],\
	"data": {\
		"from":@{from},\
		"to": @{to},\
		"amount": @{amount},\
		"memo": @{memo}\
	}\
}'\

def transferJson(fromName, toName, ammount, memo):

	stream = SIMPLE_TRANSFER_JSON.replace(TRANSFER_FROM_NAME, utils.quoteValue(fromName))

	stream = stream.replace(TRANSFER_TO_NAME, utils.quoteValue(toName))

	stream = stream.replace(TRANSFER_AMOUNT_NAME, utils.quoteValue(str(ammount)))

	stream = stream.replace(TRANSFER_MEMO_NAME, utils.quoteValue(memo))

	return stream