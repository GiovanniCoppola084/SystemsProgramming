

2023-05-02 20:14                                                          Page 1


00010000 T begtext	   00012d2d T _km_dump	      00016795 T inode_delete_da
00010000 T _start	   00012d94 T _km_page_alloc  00016820 T delete_inode_po
0001006c T __isr_restore   00012e8c T _km_page_free   000168e8 T delete_data_blo
00010bd3 T __inb	   0001303f T _km_slice_alloc 000169ae T delete_pointer_
00010bdf T __inw	   00013103 T _km_slice_free  00016a37 T move_in_directo
00010bec T __inl	   00013173 T _pcb_init	      00016abd T print_directory
00010bf8 T __outb	   0001320f T _pcb_alloc      00016cab T main1
00010c05 T __outw	   00013263 T _pcb_free	      00016e9f T main2
00010c13 T __outl	   000132ad T _pcb_find	      00017025 T main3
00010c20 T __get_flags	   00013302 T _pcb_cleanup    0001719a T userH
00010c23 T __pause	   000133df T _performExit    0001732a T userZ
00010c2b T __get_ra	   00013572 T _pcount	      00017451 T userI
00010d1f T __cio_setscroll 000135e9 T _pcb_dump	      0001773c T userW
00010dc1 T __cio_moveto	   000136f2 T _ctx_dump	      00017875 T userJ
00010e21 T __cio_putchar_a 000137e4 T _ctx_dump_all   0001798f T userY
00010e99 T __cio_putchar   00013887 T _ptable_dump    00017aa1 T main4
00010f97 T __cio_puts_at   00013b99 T _q_init	      00017c36 T userX
00010fd9 T __cio_puts	   00013c6e T _q_free	      00017d86 T main5
0001100a T __cio_write	   00013ca2 T _q_alloc	      00017f4d T userP
00011042 T __cio_clearscro 00013d64 T _q_length	      0001804d T userQ
000110bb T __cio_clearscre 00013db2 T _q_insert	      00018152 T userR
0001112b T __cio_scroll	   00013f28 T _q_find	      000182b6 T userS
00011633 T __cio_printf_at 00013f99 T _q_remove	      000183b9 T main6
00011653 T __cio_printf	   0001406c T _q_removeBy     0001869a T init
000117d5 T __cio_getchar   00014119 T _q_forEach      000188c0 T idle
0001186a T __cio_gets	   00014195 T _q_dump	      00018bd4 T wait
000118bb T __cio_input_que 000142a7 T _sched_init     00018bec T cwritech
000118e6 T __cio_init	   0001434a T _schedule	      00018c0d T cwrites
00011972 T __put_char_or_c 00014438 T _dispatch	      00018c3a T cwrite
000119ab T __bound	   00014872 T _sio_init	      00018c55 T swritech
000119cf T __memset	   000149ef T _sio_enable     00018c76 T swrites
000119f3 T __memclr	   00014a66 T _sio_disable    00018ca3 T swrite
00011a14 T __memcpy	   00014add T _sio_inq_length 00018cbe T str2int
00011a41 T __strlen	   00014ae7 T _sio_readc      00018d30 T strlen
00011a63 T __strcmp	   00014b48 T _sio_reads      00018d52 T strcpy
00011a99 T __strcpy	   00014be3 T _sio_writec     00018d7c T strcat
00011ac9 T __strcat	   00014c53 T _sio_write      00018db2 T strcmp
00011aff T __pad	   00014cfa T _sio_puts	      00018de8 T pad
00011b21 T __padstr	   00014d37 T _sio_dump	      00018e0a T padstr
00011bb5 T __sprint	   00014eb4 T _stk_init	      00018e9e T sprint
00011e27 T __cvtdec0	   00014f3a T _stk_alloc      000190cc T cvt_dec0
00011eb0 T __cvtuns0	   00014f86 T _stk_free	      00019155 T cvt_dec
00011f16 T __cvtdec	   00014fac T _stk_setup      0001919c T cvt_hex
00011f5d T __cvtuns	   00015079 T _stk_dump	      0001922b T cvt_oct
00011f8f T __cvthex	   00015cba T _sys_init	      000192b5 T parseArgString
0001200e T __cvtoct	   00015d88 T _vec_clear      00019358 T exit
0001228a T __panic	   00015df9 T _vec_add	      00019360 T spawn
000122a6 T __init_interrup 00015efa T _vec_remove     00019368 T read
000122b9 T __install_isr   0001603a T _vec_dump	      00019370 T write
000122de T __delay	   0001612d T init_inodes     00019378 T sleep
0001251c T _clk_init	   000161d5 T init_data_block 00019380 T kill
000125a6 T _kreport	   0001627c T file_system_ini 00019388 T waitpid
00012667 T _kinit	   000162dc T create_inode    00019390 T ushell
00012937 T _grow_list	   000164da T create_data_blo 00019398 T bogus
00012990 T _kpanic	   000166b5 T inode_read      000193a0 T fallbackExit
00012af1 T _km_init	   0001671f T inode_write     0001c000 D __isr_stub_tabl







2023-05-02 20:14                                                          Page 2


0001c50c D __hexdigits	   0001e8e0 B b512	      0001ec84 B _nextPid
0001c520 D _stateStr	   0001eae0 B b256	      0001ec88 B _readyLength
0001c568 D _prioStr	   0001ebe0 B _systemStack    0001ec94 B _current
0001c580 D spawnTable	   0001ebe4 B _systemESP      0001ec98 B _readyQueue
0001c6f0 B __bss_start	   0001ec00 B _nProcs	      0001eca4 B _sioReadQueue
0001c6f0 D _edata	   0001ec04 B _initPCB	      0001eca8 B _sleepQueue
0001e4c0 B __isr_table	   0001ec08 B _userShell      0001ecac B _end
0001e8c0 B _systemTime	   0001ec20 B _activeProcs





















































