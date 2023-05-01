;
; 2000-05-17, Ullrich von Bassewitz
; 2022, ChaN
; 2023, Rumbledethumps
;
; int __fastcall__ _osmaperrno (unsigned char oserror);
; /* Map a system-specific error into a system-independent code. */
;

        .include        "errno.inc"

.code

__osmaperrno:
        ldx     #ErrTabSize
@L1:    cmp     ErrTab-2,x      ; Search for the error code
        beq     @L2             ; Jump if found
        dex
        dex
        bne     @L1             ; Next entry

; Code not found, return EUNKNOWN

        lda     #<EUNKNOWN
        ldx     #>EUNKNOWN
        rts

; Found the code

@L2:    lda     ErrTab-1,x
        ldx     #$00            ; High byte always zero
        rts

.rodata

ErrTab:

        .byte 1, EIO        ; FR_DISK_ERR,           (1) A hard error occurred in the low level disk I/O layer
        .byte 2, EUNKNOWN   ; FR_INT_ERR,            (2) Assertion failed
        .byte 3, EBUSY      ; FR_NOT_READY,          (3) The physical drive cannot work
        .byte 4, ENOENT     ; FR_NO_FILE,            (4) Could not find the file
        .byte 5, ENOENT     ; FR_NO_PATH,            (5) Could not find the path
        .byte 6, EINVAL     ; FR_INVALID_NAME,       (6) The path name format is invalid
        .byte 7, EACCES     ; FR_DENIED,             (7) Access denied due to prohibited access or directory full
        .byte 8, EEXIST     ; FR_EXIST,              (8) Access denied due to prohibited access
        .byte 9, EINVAL     ; FR_INVALID_OBJECT,     (9) The file/directory object is invalid
        .byte 10, EACCES    ; FR_WRITE_PROTECTED,    (10) The physical drive is write protected
        .byte 11, ENODEV    ; FR_INVALID_DRIVE,      (11) The logical drive number is invalid
        .byte 12, EUNKNOWN  ; FR_NOT_ENABLED,        (12) The volume has no work area
        .byte 13, EUNKNOWN  ; FR_NO_FILESYSTEM,      (13) There is no valid FAT volume
        .byte 14, EUNKNOWN  ; FR_MKFS_ABORTED,       (14) The f_mkfs() aborted due to any problem
        .byte 15, EUNKNOWN  ; FR_TIMEOUT,            (15) Could not get a grant to access the volume within defined period
        .byte 16, EBUSY     ; FR_LOCKED,             (16) The operation is rejected according to the file sharing policy
        .byte 17, ENOMEM    ; FR_NOT_ENOUGH_CORE,    (17) LFN working buffer could not be allocated
        .byte 18, EMFILE    ; FR_TOO_MANY_OPEN_FILES (18) Number of open files > FF_FS_LOCK
        .byte 19, EINVAL    ; FR_INVALID_PARAMETER   (19) Given parameter is invalid

ErrTabSize = (* - ErrTab)
