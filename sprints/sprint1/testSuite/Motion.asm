;Animation Program.  (Can store ~32 frames of fullscreen animation.)
;-------------------
;Binary created using DAsm 2.12 running on an Amiga.

   PROCESSOR   6502

CHARCOUNT   EQU   #$60
MAXFRAMES   EQU   #$20

PPU2000hi   EQU   #$04
PPU2000lo   EQU   #$05
FRAMENUM    EQU   #$06
FRAMEODD    EQU   #$07


   ORG   $8000    ;32Kb PRG-ROM, 8Kb CHR-ROM

Reset_Routine  SUBROUTINE
   cld         ;Clear decimal flag
   sei         ;Disable interrupts
.WaitV   lda $2002
   bpl .WaitV     ;Wait for vertical blanking interval
   ldx #$00
   stx $2000
   stx $2001      ;Screen display off, amongst other things
   dex
   txs         ;Top of stack at $1FF

;Clear (most of) the NES' WRAM. This routine is ripped from "Duck Hunt" - I should probably clear all
;$800 bytes.
   ldy #$06    ;To clear 7 x $100 bytes, from $000 to $6FF?
   sty $01        ;Store count value in $01
   ldy #$00
   sty $00
   lda #$00

.Clear   sta ($00),y    ;Clear $100 bytes
   dey
   bne .Clear

   dec $01        ;Decrement "banks" left counter
   bpl .Clear     ;Do next if >= 0


   lda   #$24
   sta   $2006
   sta   PPU2000hi
   lda   #$00
   sta   $2006
   sta   PPU2000lo

   ldx   #$00
   ldy   #$10
.ClearPPU sta $2007
   dex
   bne   .ClearPPU
   dey
   bne   .ClearPPU

   lda   #$00
   sta   $2003
   tay
.ClearSpr sta   $2004
   dey
   bne   .ClearSpr

;********* Initialize Palette to specified colour ********

   ldx   #$3F
   stx   $2006
   ldx   #$00
   stx   $2006

   ldx   #$0D     ;Colour Value (White)
   ldy   #$20     ;Clear BG & Sprite palettes.
.InitPal stx $2007
   dey
   bne   .InitPal

   lda   #$3F
   sta   $2006
   lda   #$01
   sta   $2006
   lda   #$1A
   sta   $2007
   lda   #$2B
   sta   $2007
   lda   #$3C
   sta   $2007

;*********************************************************

;********* Set up Name & Attributes ******************

   lda   #<.CMCMap
   sta   $00
   sta   $02
   lda   #>.CMCMap
   sta   $01
   sta   $03

   lda   #$00
   sta   FRAMENUM
   sta   FRAMEODD

;************************************


;Enable vblank interrupts, etc.
   lda   #%10001000
   sta   $2000
   lda   #%00011110        ;Screen on, sprites on, show leftmost 8 pixels, colour
   sta   $2001
;  cli            ;Enable interrupts(?)

;Now just loop forever?
.Loop jmp   .Loop

.CMCMap
   INCLUDE MotionNAM.asm


IncAddrs SUBROUTINE
   lda   #CHARCOUNT
   clc
   adc   PPU2000lo
   sta   PPU2000lo
   lda   #$00
   adc   PPU2000hi
   sta   PPU2000hi

   lda   PPU2000hi
   and   #%11111011        ;Test for $23 or $27 as upper PPU address (??)
   cmp   #$23
   bne   .NotPPUOverflow
   lda   PPU2000lo
   cmp   #$C0
   bne   .NotPPUOverflow
   lda   #$20
   clc
   adc   FRAMEODD
   sta   PPU2000hi
   lda   #$00
   sta   PPU2000lo
   inc   FRAMENUM
   lda   #$04
   eor   FRAMEODD
   sta   FRAMEODD

.NotPPUOverflow

   lda   #CHARCOUNT
   clc
   adc   $00
   sta   $00
   lda   #$00
   adc   $01
   sta   $01

   lda   FRAMENUM
   cmp   #MAXFRAMES
   bne   .NotFrameOverflow
   lda   $02
   sta   $00
   lda   $03
   sta   $01
   lda   #$00
   sta   FRAMENUM

.NotFrameOverflow
   rts

NMI_Routine SUBROUTINE
   pha

   lda   PPU2000hi
   sta   $2006
   lda   PPU2000lo
   sta   $2006

   ldx   #CHARCOUNT
   ldy   #$00
.DrawLoop1
   lda   ($00),Y
   sta   $2007
   iny
   dex
   bne   .DrawLoop1

   jsr   IncAddrs

   lda   #$00
   sta   $2006
   sta   $2006

   lda   FRAMEODD
   lsr
   lsr
   ora   #$80
   sta   $2000
   lda   #$00
   sta   $2005
   sta   $2005

   pla
   rti

IRQ_Routine       ;Dummy label
   rti

;That's all the code. Now we just need to set the vector table approriately.

   ORG   $FFFA,0
   dc.w  NMI_Routine
   dc.w  Reset_Routine
   dc.w  IRQ_Routine    ;Not used, just points to RTI


;The end.
