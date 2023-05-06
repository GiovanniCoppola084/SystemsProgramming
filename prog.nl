

2023-05-05 20:35                                                          Page 1


00010000 T begtext	   00012d2d T _km_dump	      000166c0 T inode_delete_da
00010000 T _start	   00012d94 T _km_page_alloc  00016729 T delete_inode_po
0001006c T __isr_restore   00012e8c T _km_page_free   00016800 T delete_data_blo
00010bd3 T __inb	   0001303f T _km_slice_alloc 000168e4 T delete_pointer_
00010bdf T __inw	   00013103 T _km_slice_free  0001698b T move_in_directo
00010bec T __inl	   00013173 T _pcb_init	      00016a21 T move_out_direct
00010bf8 T __outb	   0001320f T _pcb_alloc      00016a4d T print_directory
00010c05 T __outw	   00013263 T _pcb_free	      00016bb1 T print_file_syst
00010c13 T __outl	   000132ad T _pcb_find	      00016c98 T read_line
00010c20 T __get_flags	   00013302 T _pcb_cleanup    00016d16 T is_fs_initializ
00010c23 T __pause	   000133df T _performExit    00016d38 T is_inode
00010c2b T __get_ra	   00013572 T _pcount	      00016d5d T print_menu
00010d1f T __cio_setscroll 000135e9 T _pcb_dump	      00016e46 T file_shell
00010dc1 T __cio_moveto	   000136f2 T _ctx_dump	      0001735e T init
00010e21 T __cio_putchar_a 000137e4 T _ctx_dump_all   00017584 T idle
00010e99 T __cio_putchar   00013887 T _ptable_dump    000175b2 T wait
00010f97 T __cio_puts_at   00013b99 T _q_init	      000175ca T cwritech
00010fd9 T __cio_puts	   00013c6e T _q_free	      000175eb T cwrites
0001100a T __cio_write	   00013ca2 T _q_alloc	      00017618 T cwrite
00011042 T __cio_clearscro 00013d64 T _q_length	      00017633 T swritech
000110bb T __cio_clearscre 00013db2 T _q_insert	      00017654 T swrites
0001112b T __cio_scroll	   00013f28 T _q_find	      00017681 T swrite
00011633 T __cio_printf_at 00013f99 T _q_remove	      0001769c T str2int
00011653 T __cio_printf	   0001406c T _q_removeBy     0001770e T strlen
000117d5 T __cio_getchar   00014119 T _q_forEach      00017730 T strcpy
0001186a T __cio_gets	   00014195 T _q_dump	      0001775a T strcat
000118bb T __cio_input_que 000142a7 T _sched_init     00017790 T strcmp
000118e6 T __cio_init	   0001434a T _schedule	      000177c6 T pad
00011972 T __put_char_or_c 00014438 T _dispatch	      000177e8 T padstr
000119ab T __bound	   00014872 T _sio_init	      0001787c T sprint
000119cf T __memset	   000149ef T _sio_enable     00017aaa T cvt_dec0
000119f3 T __memclr	   00014a66 T _sio_disable    00017b33 T cvt_dec
00011a14 T __memcpy	   00014add T _sio_inq_length 00017b7a T cvt_hex
00011a41 T __strlen	   00014ae7 T _sio_readc      00017c09 T cvt_oct
00011a63 T __strcmp	   00014b48 T _sio_reads      00017c93 T parseArgString
00011a99 T __strcpy	   00014be3 T _sio_writec     00017d36 T exit
00011ac9 T __strcat	   00014c53 T _sio_write      00017d3e T spawn
00011aff T __pad	   00014cfa T _sio_puts	      00017d46 T read
00011b21 T __padstr	   00014d37 T _sio_dump	      00017d4e T write
00011bb5 T __sprint	   00014eb4 T _stk_init	      00017d56 T sleep
00011e27 T __cvtdec0	   00014f3a T _stk_alloc      00017d5e T kill
00011eb0 T __cvtuns0	   00014f86 T _stk_free	      00017d66 T waitpid
00011f16 T __cvtdec	   00014fac T _stk_setup      00017d6e T ushell
00011f5d T __cvtuns	   00015079 T _stk_dump	      00017d76 T bogus
00011f8f T __cvthex	   00015cba T _sys_init	      00017d7e T fallbackExit
0001200e T __cvtoct	   00015d88 T _vec_clear      0001c000 D __isr_stub_tabl
0001228a T __panic	   00015df9 T _vec_add	      0001c50c D __hexdigits
000122a6 T __init_interrup 00015efa T _vec_remove     0001c520 D _stateStr
000122b9 T __install_isr   0001603a T _vec_dump	      0001c568 D _prioStr
000122de T __delay	   0001612d T init_inodes     0001c580 D spawnTable
0001251c T _clk_init	   000161e5 T init_data_block 0001c5b0 B __bss_start
000125a6 T _kreport	   0001628c T file_system_ini 0001c5b0 D _edata
00012667 T _kinit	   000162ec T create_inode    0001e380 B __isr_table
00012937 T _grow_list	   00016456 T create_data_blo 0001e780 B _systemTime
00012990 T _kpanic	   000165d9 T inode_read      0001e7a0 B b512
00012af1 T _km_init	   00016642 T inode_write     0001e9a0 B b256







2023-05-05 20:35                                                          Page 2


0001eaa0 B _systemStack	   0001eae0 B _activeProcs    0001eb64 B _sioReadQueue
0001eaa4 B _systemESP	   0001eb44 B _nextPid	      0001eb68 B _sleepQueue
0001eac0 B _nProcs	   0001eb48 B _readyLength    0001eb6c B fs
0001eac4 B _initPCB	   0001eb54 B _current	      0001eb70 B _end
0001eac8 B _userShell	   0001eb58 B _readyQueue
























































