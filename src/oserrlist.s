;
; 2002-07-18, Ullrich von Bassewitz
; 2022, ChaN
; 2023, Rumbledethumps
;
; Defines the platform-specific error list.
;
; The table is built as a list of entries:
;
;       .byte   entrylen
;       .byte   errorcode
;       .asciiz errormsg
;
; and, terminated by an entry with length zero that is returned if the
; error code could not be found.
;

        .export         __sys_oserrlist

;----------------------------------------------------------------------------
; Macros used to generate the list (may get moved to an include file?)

; Regular entry
.macro  sys_oserr_entry         code, msg
        .local  Start, End
Start:  .byte   End - Start
        .byte   code
        .asciiz msg
End:
.endmacro

; Sentinel entry
.macro  sys_oserr_sentinel      msg
        .byte   0                       ; Length is always zero
        .byte   0                       ; Code is unused
        .asciiz msg
.endmacro

;----------------------------------------------------------------------------
; The error message table

.rodata

__sys_oserrlist:

        sys_oserr_entry          1, "A hard error occurred in the low level disk I/O layer"
        sys_oserr_entry          2, "Assertion failed"
        sys_oserr_entry          3, "The physical drive cannot work"
        sys_oserr_entry          4, "Could not find the file"
        sys_oserr_entry          5, "Could not find the path"
        sys_oserr_entry          6, "The path name format is invalid"
        sys_oserr_entry          7, "Access denied due to prohibited access or directory full"
        sys_oserr_entry          8, "Access denied due to prohibited access"
        sys_oserr_entry          9, "The file/directory object is invalid"
        sys_oserr_entry         10, "The physical drive is write protected"
        sys_oserr_entry         11, "The logical drive number is invalid"
        sys_oserr_entry         12, "The volume has no work area"
        sys_oserr_entry         13, "There is no valid FAT volume"
        sys_oserr_entry         14, "The f_mkfs() aborted due to any problem"
        sys_oserr_entry         15, "Could not get a grant to access the volume within defined period"
        sys_oserr_entry         16, "The operation is rejected according to the file sharing policy"
        sys_oserr_entry         17, "LFN working buffer could not be allocated"
        sys_oserr_entry         18, "Number of open files > FF_FS_LOCK"
        sys_oserr_entry         19, "Given parameter is invalid"
        sys_oserr_sentinel      "Unknown error"
