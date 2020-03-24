    ; paging info
    PAGE_DIR_TABLE_POS  equ     0x100000     ; 1mb

    PAGE_P              equ     1b          ; present
    PAGE_RW_R           equ     00b         ; can read
    PAGE_RW_W           equ     10b         ; can read and write
    PAGE_US_S           equ     000b        ; super privilege
    PAGE_US_U           equ     100b        ; user privilege