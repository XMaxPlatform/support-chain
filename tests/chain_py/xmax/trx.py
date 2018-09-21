#!/usr/bin/env python
#! -*- coding:utf-8 -*-

from xmax import utils

TRX_SCOPE_KEY = '${scope}'
TRX_MSG_KEY = '${messages}'

TRX_SIGN_KEY = '${sign_keys}'
TRX_BODY_KEY = '${trx_body}'

TRX_POST_JSON = '[\n{ "sign_keys": [${sign_keys}]}\n, ${trx_body}\n]'

TRX_JSON = '\
{\n\
	"ref_block_num": "0",\n\
	"ref_block_prefix": "0",\n\
	"expiration": "0",\n\
	"scope": [${scope}],\n\
	"read_scope": [],\n\
	"messages": [${messages}],\n\
	"signatures": []\n\
}'
