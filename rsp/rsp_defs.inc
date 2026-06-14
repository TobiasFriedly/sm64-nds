
.definelabel OSTask_addr, 0xFC0

OSTask_flags           equ 0x04
OSTask_ucode           equ 0x10
OSTask_ucode_size      equ 0x14
OSTask_ucode_data      equ 0x18
OSTask_ucode_data_size equ 0x1C

OS_TASK_DP_WAIT equ 0x0002

DPC_STATUS_XBUS_DMA    equ 0x0001
DPC_STATUS_DMA_BUSY    equ 0x0100
DPC_STATUS_START_VALID equ 0x0400

DPC_STATUS_CLR_XBUS    equ 0x0001
