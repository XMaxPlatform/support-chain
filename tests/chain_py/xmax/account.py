#!/usr/bin/env python
#! -*- coding:utf-8 -*-

from xmax import utils

NEWACC_TRXACC_NAME = '@{trx_acc}'
NEWACC_CREATOR_NAME = '@{creator}'
NEWACC_ACCOUNT_NAME = '@{account}'
NEWACC_DEPOSIT_NAME = '@{deposit}'

NEWACC_OWNER_KEY = '@{owner_key}'
NEWACC_ACTIVE_KEY = '@{active_key}'
SIMPLE_NEW_ACCOUNT_JSON = '\
{\n\
	"code": "xmax",\n\
	"type": "addaccount",\n\
	"authorization": [{\n\
		"account": @{trx_acc},\n\
		"permission": "active"\n\
	}],\n\
	"data": {\n\
		"creator": @{creator},\n\
		"name": @{account},\n\
		"owner": {\n\
			"threshold": "1",\n\
			"keys": [{\n\
				"key": @{owner_key},\n\
				"weight": "1"\n\
			}],\n\
			"accounts":[]\n\
		},\n\
		"active": {\n\
			"threshold": "1",\n\
			"keys": [{\n\
				"key": @{active_key},\n\
				"weight": "1"\n\
			}],\n\
			"accounts":[]\n\
		},\n\
		"deposit": @{deposit}\n\
	}\n\
}'