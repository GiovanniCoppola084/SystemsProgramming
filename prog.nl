

2023-04-30 17:14                                                          Page 1


00010000 T begtext	   00012d2d T _km_dump	      000166ba T inode_delete_da
00010000 T _start	   00012d94 T _km_page_alloc  00016745 T delete_inode_po
0001006c T __isr_restore   00012e8c T _km_page_free   0001680d T delete_data_blo
00010bd3 T __inb	   0001303f T _km_slice_alloc 000168d3 T delete_pointer_
00010bdf T __inw	   00013103 T _km_slice_free  0001695c T move_in_directo
00010bec T __inl	   00013173 T _pcb_init	      000169e2 T print_directory
00010bf8 T __outb	   0001320f T _pcb_alloc      00016baf T main1
00010c05 T __outw	   00013263 T _pcb_free	      00016da3 T main2
00010c13 T __outl	   000132ad T _pcb_find	      00016f29 T main3
00010c20 T __get_flags	   00013302 T _pcb_cleanup    0001709e T userH
00010c23 T __pause	   000133df T _performExit    0001722e T userZ
00010c2b T __get_ra	   00013572 T _pcount	      00017355 T userI
00010d1f T __cio_setscroll 000135e9 T _pcb_dump	      00017640 T userW
00010dc1 T __cio_moveto	   000136f2 T _ctx_dump	      00017779 T userJ
00010e21 T __cio_putchar_a 000137e4 T _ctx_dump_all   00017893 T userY
00010e99 T __cio_putchar   00013887 T _ptable_dump    000179a5 T main4
00010f97 T __cio_puts_at   00013b99 T _q_init	      00017b3a T userX
00010fd9 T __cio_puts	   00013c6e T _q_free	      00017c8a T main5
0001100a T __cio_write	   00013ca2 T _q_alloc	      00017e51 T userP
00011042 T __cio_clearscro 00013d64 T _q_length	      00017f51 T userQ
000110bb T __cio_clearscre 00013db2 T _q_insert	      00018056 T userR
0001112b T __cio_scroll	   00013f28 T _q_find	      000181ba T userS
00011633 T __cio_printf_at 00013f99 T _q_remove	      000182bd T main6
00011653 T __cio_printf	   0001406c T _q_removeBy     0001859e T init
000117d5 T __cio_getchar   00014119 T _q_forEach      000187c4 T idle
0001186a T __cio_gets	   00014195 T _q_dump	      00018adc T wait
000118bb T __cio_input_que 000142a7 T _sched_init     00018af4 T cwritech
000118e6 T __cio_init	   0001434a T _schedule	      00018b15 T cwrites
00011972 T __put_char_or_c 00014438 T _dispatch	      00018b42 T cwrite
000119ab T __bound	   00014872 T _sio_init	      00018b5d T swritech
000119cf T __memset	   000149ef T _sio_enable     00018b7e T swrites
000119f3 T __memclr	   00014a66 T _sio_disable    00018bab T swrite
00011a14 T __memcpy	   00014add T _sio_inq_length 00018bc6 T str2int
00011a41 T __strlen	   00014ae7 T _sio_readc      00018c38 T strlen
00011a63 T __strcmp	   00014b48 T _sio_reads      00018c5a T strcpy
00011a99 T __strcpy	   00014be3 T _sio_writec     00018c84 T strcat
00011ac9 T __strcat	   00014c53 T _sio_write      00018cba T strcmp
00011aff T __pad	   00014cfa T _sio_puts	      00018cf0 T pad
00011b21 T __padstr	   00014d37 T _sio_dump	      00018d12 T padstr
00011bb5 T __sprint	   00014eb4 T _stk_init	      00018da6 T sprint
00011e27 T __cvtdec0	   00014f3a T _stk_alloc      00018fd4 T cvt_dec0
00011eb0 T __cvtuns0	   00014f86 T _stk_free	      0001905d T cvt_dec
00011f16 T __cvtdec	   00014fac T _stk_setup      000190a4 T cvt_hex
00011f5d T __cvtuns	   00015079 T _stk_dump	      00019133 T cvt_oct
00011f8f T __cvthex	   00015cba T _sys_init	      000191bd T parseArgString
0001200e T __cvtoct	   00015d88 T _vec_clear      00019260 T exit
0001228a T __panic	   00015df9 T _vec_add	      00019268 T spawn
000122a6 T __init_interrup 00015efa T _vec_remove     00019270 T read
000122b9 T __install_isr   0001603a T _vec_dump	      00019278 T write
000122de T __delay	   0001612d T init_inodes     00019280 T sleep
0001251c T _clk_init	   000161d5 T init_data_block 00019288 T kill
000125a6 T _kreport	   0001627c T file_system_ini 00019290 T waitpid
00012667 T _kinit	   000162dc T create_inode    00019298 T ushell
00012937 T _grow_list	   00016442 T create_data_blo 000192a0 T bogus
00012990 T _kpanic	   000165da T inode_read      000192a8 T fallbackExit
00012af1 T _km_init	   00016644 T inode_write     0001d000 D __isr_stub_tabl







2023-04-30 17:14                                                          Page 2


0001d50c D __hexdigits	   0001f8e0 B b512	      0001fc84 B _nextPid
0001d520 D _stateStr	   0001fae0 B b256	      0001fc88 B _readyLength
0001d568 D _prioStr	   0001fbe0 B _systemStack    0001fc94 B _current
0001d580 D spawnTable	   0001fbe4 B _systemESP      0001fc98 B _readyQueue
0001d6f0 B __bss_start	   0001fc00 B _nProcs	      0001fca4 B _sioReadQueue
0001d6f0 D _edata	   0001fc04 B _initPCB	      0001fca8 B _sleepQueue
0001f4c0 B __isr_table	   0001fc08 B _userShell      0001fcac B _end
0001f8c0 B _systemTime	   0001fc20 B _activeProcs





















































