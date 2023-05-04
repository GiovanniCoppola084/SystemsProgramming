

2023-05-04 19:01                                                          Page 1


00010000 T begtext	   00012d2d T _km_dump	      0001666e T inode_delete_da
00010000 T _start	   00012d94 T _km_page_alloc  000166f9 T delete_inode_po
0001006c T __isr_restore   00012e8c T _km_page_free   000167c7 T delete_data_blo
00010bd3 T __inb	   0001303f T _km_slice_alloc 0001691c T delete_pointer_
00010bdf T __inw	   00013103 T _km_slice_free  000169c3 T move_in_directo
00010bec T __inl	   00013173 T _pcb_init	      00016a49 T move_out_direct
00010bf8 T __outb	   0001320f T _pcb_alloc      00016a75 T print_directory
00010c05 T __outw	   00013263 T _pcb_free	      00016c09 T print_file_syst
00010c13 T __outl	   000132ad T _pcb_find	      00016cf0 T main1
00010c20 T __get_flags	   00013302 T _pcb_cleanup    00016ee4 T main2
00010c23 T __pause	   000133df T _performExit    0001706a T main3
00010c2b T __get_ra	   00013572 T _pcount	      000171df T userH
00010d1f T __cio_setscroll 000135e9 T _pcb_dump	      0001736f T userZ
00010dc1 T __cio_moveto	   000136f2 T _ctx_dump	      00017496 T userI
00010e21 T __cio_putchar_a 000137e4 T _ctx_dump_all   00017781 T userW
00010e99 T __cio_putchar   00013887 T _ptable_dump    000178ba T userJ
00010f97 T __cio_puts_at   00013b99 T _q_init	      000179d4 T userY
00010fd9 T __cio_puts	   00013c6e T _q_free	      00017ae6 T main4
0001100a T __cio_write	   00013ca2 T _q_alloc	      00017c7b T userX
00011042 T __cio_clearscro 00013d64 T _q_length	      00017dcb T main5
000110bb T __cio_clearscre 00013db2 T _q_insert	      00017f92 T userP
0001112b T __cio_scroll	   00013f28 T _q_find	      00018092 T userQ
00011633 T __cio_printf_at 00013f99 T _q_remove	      00018197 T userR
00011653 T __cio_printf	   0001406c T _q_removeBy     000182fb T userS
000117d5 T __cio_getchar   00014119 T _q_forEach      000183fe T main6
0001186a T __cio_gets	   00014195 T _q_dump	      000186df T init
000118bb T __cio_input_que 000142a7 T _sched_init     00018905 T idle
000118e6 T __cio_init	   0001434a T _schedule	      00018d45 T wait
00011972 T __put_char_or_c 00014438 T _dispatch	      00018d5d T cwritech
000119ab T __bound	   00014872 T _sio_init	      00018d7e T cwrites
000119cf T __memset	   000149ef T _sio_enable     00018dab T cwrite
000119f3 T __memclr	   00014a66 T _sio_disable    00018dc6 T swritech
00011a14 T __memcpy	   00014add T _sio_inq_length 00018de7 T swrites
00011a41 T __strlen	   00014ae7 T _sio_readc      00018e14 T swrite
00011a63 T __strcmp	   00014b48 T _sio_reads      00018e2f T str2int
00011a99 T __strcpy	   00014be3 T _sio_writec     00018ea1 T strlen
00011ac9 T __strcat	   00014c53 T _sio_write      00018ec3 T strcpy
00011aff T __pad	   00014cfa T _sio_puts	      00018eed T strcat
00011b21 T __padstr	   00014d37 T _sio_dump	      00018f23 T strcmp
00011bb5 T __sprint	   00014eb4 T _stk_init	      00018f59 T pad
00011e27 T __cvtdec0	   00014f3a T _stk_alloc      00018f7b T padstr
00011eb0 T __cvtuns0	   00014f86 T _stk_free	      0001900f T sprint
00011f16 T __cvtdec	   00014fac T _stk_setup      0001923d T cvt_dec0
00011f5d T __cvtuns	   00015079 T _stk_dump	      000192c6 T cvt_dec
00011f8f T __cvthex	   00015cba T _sys_init	      0001930d T cvt_hex
0001200e T __cvtoct	   00015d88 T _vec_clear      0001939c T cvt_oct
0001228a T __panic	   00015df9 T _vec_add	      00019426 T parseArgString
000122a6 T __init_interrup 00015efa T _vec_remove     000194c9 T exit
000122b9 T __install_isr   0001603a T _vec_dump	      000194d1 T spawn
000122de T __delay	   0001612d T init_inodes     000194d9 T read
0001251c T _clk_init	   000161d5 T init_data_block 000194e1 T write
000125a6 T _kreport	   0001627c T file_system_ini 000194e9 T sleep
00012667 T _kinit	   000162dc T create_inode    000194f1 T kill
00012937 T _grow_list	   00016418 T create_data_blo 000194f9 T waitpid
00012990 T _kpanic	   0001658e T inode_read      00019501 T ushell
00012af1 T _km_init	   000165f8 T inode_write     00019509 T bogus







2023-05-04 19:01                                                          Page 2


00019511 T fallbackExit	   000208c0 B _systemTime     00020c20 B _activeProcs
0001e000 D __isr_stub_tabl 000208e0 B b512	      00020c84 B _nextPid
0001e50c D __hexdigits	   00020ae0 B b256	      00020c88 B _readyLength
0001e520 D _stateStr	   00020be0 B _systemStack    00020c94 B _current
0001e568 D _prioStr	   00020be4 B _systemESP      00020c98 B _readyQueue
0001e580 D spawnTable	   00020c00 B _nProcs	      00020ca4 B _sioReadQueue
0001e6f0 B __bss_start	   00020c04 B _initPCB	      00020ca8 B _sleepQueue
0001e6f0 D _edata	   00020c08 B _userShell      00020cac B _end
000204c0 B __isr_table




















































