
.byte  0x80, 0x37, 0x12, 0x40
.word  0x0000000F
.word  entry_point

#ifdef VERSION_SH
    .word  0x00001448
#elif defined(VERSION_CN)
    .word  0x0000144C
#elif defined(VERSION_EU)
    .word  0x00001446
#else
    .word  0x00001444
#endif

#ifdef VERSION_CN
    .fill 0x30
#else

.word  0x4EAA3D0E
.word  0x74757C24
.word  0x00000000
.word  0x00000000
#ifdef VERSION_SH
.ascii "SUPERMARIO64        "
#else
.ascii "SUPER MARIO 64      "
#endif
.word  0x00000000
.word  0x0000004E
.ascii "SM"

#ifdef VERSION_EU
    .ascii "P"
#elif defined(VERSION_US)
    .ascii "E"
#else
    .ascii "J"
#endif

#ifdef VERSION_SH
    .byte  0x03
#else
    .byte  0x00
#endif

#endif
