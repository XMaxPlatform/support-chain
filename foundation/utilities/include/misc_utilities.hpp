/**
 *  @file
 *  @copyright defined in xmax/LICENSE
 */
#pragma once


#define HAS_BIT_FLAG(_flag, _key) ((_flag & _key) != 0)

#define NO_BIT_FLAG(_flag, _key) ((_flag & _key) == 0)
