#!/usr/bin/env python
#! -*- coding:utf-8 -*-

def quoteValue(val):
    return r'"' + val + r'"'

def jsonArray(strs):
    buff = ''
    count = len(strs)
    if count > 1:
        for idx in range(0, count - 1):
            buff += strs[idx]
            buff += ','
        buff += strs[count - 1]
    else:
        buff = strs[0]

    return buff

def jsonValArray(strs):
    buff = ''
    count = len(strs)
    if count > 1:
        for idx in range(0, count - 1):
            buff += (quoteValue(strs[idx]) + ',')

        buff += quoteValue(strs[count - 1])
    else:
        buff = quoteValue(strs[0])

    return buff
