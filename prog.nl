

2023-04-30 19:23                                                          Page 1


00010000 T begtext	   00012d2d T _km_dump	      00016783 T inode_delete_da
00010000 T _start	   00012d94 T _km_page_alloc  0001680e T delete_inode_po
0001006c T __isr_restore   00012e8c T _km_page_free   000168d6 T delete_data_blo
00010bd3 T __inb	   0001303f T _km_slice_alloc 0001699c T delete_pointer_
00010bdf T __inw	   00013103 T _km_slice_free  00016a25 T move_in_directo
00010bec T __inl	   00013173 T _pcb_init	      00016aab T print_directory
00010bf8 T __outb	   0001320f T _pcb_alloc      00016c82 T main1
00010c05 T __outw	   00013263 T _pcb_free	      00016e76 T main2
00010c13 T __outl	   000132ad T _pcb_find	      00016ffc T main3
00010c20 T __get_flags	   00013302 T _pcb_cleanup    00017171 T userH
00010c23 T __pause	   000133df T _performExit    00017301 T userZ
00010c2b T __get_ra	   00013572 T _pcount	      00017428 T userI
00010d1f T __cio_setscroll 000135e9 T _pcb_dump	      00017713 T userW
00010dc1 T __cio_moveto	   000136f2 T _ctx_dump	      0001784c T userJ
00010e21 T __cio_putchar_a 000137e4 T _ctx_dump_all   00017966 T userY
00010e99 T __cio_putchar   00013887 T _ptable_dump    00017a78 T main4
00010f97 T __cio_puts_at   00013b99 T _q_init	      00017c0d T userX
00010fd9 T __cio_puts	   00013c6e T _q_free	      00017d5d T main5
0001100a T __cio_write	   00013ca2 T _q_alloc	      00017f24 T userP
00011042 T __cio_clearscro 00013d64 T _q_length	      00018024 T userQ
000110bb T __cio_clearscre 00013db2 T _q_insert	      00018129 T userR
0001112b T __cio_scroll	   00013f28 T _q_find	      0001828d T userS
00011633 T __cio_printf_at 00013f99 T _q_remove	      00018390 T main6
00011653 T __cio_printf	   0001406c T _q_removeBy     00018671 T init
000117d5 T __cio_getchar   00014119 T _q_forEach      00018897 T idle
0001186a T __cio_gets	   00014195 T _q_dump	      00018b3a T wait
000118bb T __cio_input_que 000142a7 T _sched_init     00018b52 T cwritech
000118e6 T __cio_init	   0001434a T _schedule	      00018b73 T cwrites
00011972 T __put_char_or_c 00014438 T _dispatch	      00018ba0 T cwrite
000119ab T __bound	   00014872 T _sio_init	      00018bbb T swritech
000119cf T __memset	   000149ef T _sio_enable     00018bdc T swrites
000119f3 T __memclr	   00014a66 T _sio_disable    00018c09 T swrite
00011a14 T __memcpy	   00014add T _sio_inq_length 00018c24 T str2int
00011a41 T __strlen	   00014ae7 T _sio_readc      00018c96 T strlen
00011a63 T __strcmp	   00014b48 T _sio_reads      00018cb8 T strcpy
00011a99 T __strcpy	   00014be3 T _sio_writec     00018ce2 T strcat
00011ac9 T __strcat	   00014c53 T _sio_write      00018d18 T strcmp
00011aff T __pad	   00014cfa T _sio_puts	      00018d4e T pad
00011b21 T __padstr	   00014d37 T _sio_dump	      00018d70 T padstr
00011bb5 T __sprint	   00014eb4 T _stk_init	      00018e04 T sprint
00011e27 T __cvtdec0	   00014f3a T _stk_alloc      00019032 T cvt_dec0
00011eb0 T __cvtuns0	   00014f86 T _stk_free	      000190bb T cvt_dec
00011f16 T __cvtdec	   00014fac T _stk_setup      00019102 T cvt_hex
00011f5d T __cvtuns	   00015079 T _stk_dump	      00019191 T cvt_oct
00011f8f T __cvthex	   00015cba T _sys_init	      0001921b T parseArgString
0001200e T __cvtoct	   00015d88 T _vec_clear      000192be T exit
0001228a T __panic	   00015df9 T _vec_add	      000192c6 T spawn
000122a6 T __init_interrup 00015efa T _vec_remove     000192ce T read
000122b9 T __install_isr   0001603a T _vec_dump	      000192d6 T write
000122de T __delay	   0001612d T init_inodes     000192de T sleep
0001251c T _clk_init	   000161d5 T init_data_block 000192e6 T kill
000125a6 T _kreport	   0001627c T file_system_ini 000192ee T waitpid
00012667 T _kinit	   000162dc T create_inode    000192f6 T ushell
00012937 T _grow_list	   000164c8 T create_data_blo 000192fe T bogus
00012990 T _kpanic	   000166a3 T inode_read      00019306 T fallbackExit
00012af1 T _km_init	   0001670d T inode_write     0001d000 D __isr_stub_tabl







2023-04-30 19:23                                                          Page 2


0001d50c D __hexdigits	   0001f8e0 B b512	      0001fc84 B _nextPid
0001d520 D _stateStr	   0001fae0 B b256	      0001fc88 B _readyLength
0001d568 D _prioStr	   0001fbe0 B _systemStack    0001fc94 B _current
0001d580 D spawnTable	   0001fbe4 B _systemESP      0001fc98 B _readyQueue
0001d6f0 B __bss_start	   0001fc00 B _nProcs	      0001fca4 B _sioReadQueue
0001d6f0 D _edata	   0001fc04 B _initPCB	      0001fca8 B _sleepQueue
0001f4c0 B __isr_table	   0001fc08 B _userShell      0001fcac B _end
0001f8c0 B _systemTime	   0001fc20 B _activeProcs





















































