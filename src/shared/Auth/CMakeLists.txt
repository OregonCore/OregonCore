
########### next target ###############

SET(oregonauth_STAT_SRCS
   AuthCrypt.cpp
   AuthCrypt.h
   BigNumber.cpp
   BigNumber.h
   Hmac.cpp
   Hmac.h
   Sha1.cpp
   Sha1.h
   md5.c
   md5.h
)

add_library(oregonauth STATIC ${oregonauth_STAT_SRCS})
