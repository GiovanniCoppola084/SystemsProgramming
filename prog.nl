

2023-03-23 14:29                                                          Page 1


00010000 T begtext	   00012ce9 T _km_dump	      00016cb3 T userJ
00010000 T _start	   00012d50 T _km_page_alloc  00016dcd T userY
0001006c T __isr_restore   00012e48 T _km_page_free   00016edf T main4
00010bd3 T __inb	   00012ffb T _km_slice_alloc 00017074 T userX
00010bdf T __inw	   000130bf T _km_slice_free  000171c4 T main5
00010bec T __inl	   0001312f T _pcb_init	      0001738b T userP
00010bf8 T __outb	   000131cb T _pcb_alloc      0001748b T userQ
00010c05 T __outw	   0001321f T _pcb_free	      00017590 T userR
00010c13 T __outl	   00013269 T _pcb_find	      000176f4 T userS
00010c20 T __get_flags	   000132be T _pcb_cleanup    000177f7 T main6
00010c23 T __pause	   0001339b T _performExit    00017ad8 T init
00010c2b T __get_ra	   0001352e T _pcount	      00017cfe T idle
00010d1f T __cio_setscroll 000135a5 T _pcb_dump	      00017d56 T wait
00010dc1 T __cio_moveto	   000136ae T _ctx_dump	      00017d6e T cwritech
00010e21 T __cio_putchar_a 000137a0 T _ctx_dump_all   00017d8f T cwrites
00010e99 T __cio_putchar   00013843 T _ptable_dump    00017dbc T cwrite
00010f97 T __cio_puts_at   00013b55 T _q_init	      00017dd7 T swritech
00010fd9 T __cio_puts	   00013c2a T _q_free	      00017df8 T swrites
0001100a T __cio_write	   00013c5e T _q_alloc	      00017e25 T swrite
00011042 T __cio_clearscro 00013d20 T _q_length	      00017e40 T str2int
000110bb T __cio_clearscre 00013d6e T _q_insert	      00017eb2 T strlen
0001112b T __cio_scroll	   00013ee4 T _q_find	      00017ed4 T strcpy
00011633 T __cio_printf_at 00013f55 T _q_remove	      00017efe T strcat
00011653 T __cio_printf	   00014028 T _q_removeBy     00017f34 T strcmp
000117d5 T __cio_getchar   000140d5 T _q_forEach      00017f6a T pad
0001186a T __cio_gets	   00014151 T _q_dump	      00017f8c T padstr
000118bb T __cio_input_que 00014263 T _sched_init     00018020 T sprint
000118e6 T __cio_init	   00014306 T _schedule	      0001824e T cvt_dec0
00011972 T __put_char_or_c 000143f4 T _dispatch	      000182d7 T cvt_dec
000119ab T __bound	   0001482e T _sio_init	      0001831e T cvt_hex
000119cf T __memset	   000149ab T _sio_enable     000183ad T cvt_oct
000119f3 T __memclr	   00014a22 T _sio_disable    00018437 T parseArgString
00011a14 T __memcpy	   00014a99 T _sio_inq_length 000184da T exit
00011a41 T __strlen	   00014aa3 T _sio_readc      000184e2 T spawn
00011a63 T __strcmp	   00014b04 T _sio_reads      000184ea T read
00011a99 T __strcpy	   00014b9f T _sio_writec     000184f2 T write
00011ac9 T __strcat	   00014c0f T _sio_write      000184fa T sleep
00011aff T __pad	   00014cb6 T _sio_puts	      00018502 T kill
00011b21 T __padstr	   00014cf3 T _sio_dump	      0001850a T waitpid
00011bb5 T __sprint	   00014e70 T _stk_init	      00018512 T ushell
00011e27 T __cvtdec0	   00014ef6 T _stk_alloc      0001851a T bogus
00011eb0 T __cvtuns0	   00014f42 T _stk_free	      00018522 T fallbackExit
00011f16 T __cvtdec	   00014f68 T _stk_setup      0001c000 D __isr_stub_tabl
00011f5d T __cvtuns	   00015035 T _stk_dump	      0001c50c D __hexdigits
00011f8f T __cvthex	   00015c76 T _sys_init	      0001c520 D _stateStr
0001200e T __cvtoct	   00015d44 T _vec_clear      0001c568 D _prioStr
0001228a T __panic	   00015db5 T _vec_add	      0001c580 D spawnTable
000122a6 T __init_interrup 00015eb6 T _vec_remove     0001c6f0 B __bss_start
000122b9 T __install_isr   00015ff6 T _vec_dump	      0001c6f0 D _edata
000122de T __delay	   000160e9 T main1	      0001e4c0 B __isr_table
0001251c T _clk_init	   000162dd T main2	      0001e8c0 B _systemTime
000125a6 T _kreport	   00016463 T main3	      0001e8e0 B b512
00012667 T _kinit	   000165d8 T userH	      0001eae0 B b256
00012937 T _grow_list	   00016768 T userZ	      0001ebe0 B _systemStack
00012990 T _kpanic	   0001688f T userI	      0001ebe4 B _systemESP
00012af1 T _km_init	   00016b7a T userW	      0001ec00 B _nProcs







2023-03-23 14:29                                                          Page 2


0001ec04 B _initPCB	   0001ec88 B _readyLength    0001eca4 B _sioReadQueue
0001ec08 B _userShell	   0001ec94 B _current	      0001eca8 B _sleepQueue
0001ec20 B _activeProcs	   0001ec98 B _readyQueue     0001ecac B _end
0001ec84 B _nextPid

























































