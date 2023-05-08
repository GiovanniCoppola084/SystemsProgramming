

2023-05-08 19:25                                                          Page 1


00010000 T begtext	   00012d42 T _km_dump	      000166a9 T inode_delete_da
00010000 T _start	   00012da9 T _km_page_alloc  00016712 T delete_inode_po
0001006c T __isr_restore   00012ea1 T _km_page_free   0001680c T delete_data_blo
00010bd3 T __inb	   00013054 T _km_slice_alloc 00016910 T delete_pointer_
00010bdf T __inw	   00013118 T _km_slice_free  000169c6 T move_in_directo
00010bec T __inl	   00013188 T _pcb_init	      00016a2e T move_out_direct
00010bf8 T __outb	   00013224 T _pcb_alloc      00016a5a T print_directory
00010c05 T __outw	   00013278 T _pcb_free	      00016b92 T print_file_syst
00010c13 T __outl	   000132c2 T _pcb_find	      00016c97 T write_pci_confi
00010c20 T __get_flags	   00013317 T _pcb_cleanup    00016d09 T read_pci_config
00010c23 T __pause	   000133f4 T _performExit    00016d73 T _ahci_isr
00010c2b T __get_ra	   00013587 T _pcount	      00016d8c T _ahci_init
00010d1f T __cio_setscroll 000135fe T _pcb_dump	      00016dbe T find_ahci
00010dc1 T __cio_moveto	   00013707 T _ctx_dump	      00016ebe T start_cmd
00010e21 T __cio_putchar_a 000137f9 T _ctx_dump_all   00016ef6 T stop_cmd
00010e99 T __cio_putchar   0001389c T _ptable_dump    00016f42 T port_rebase
00010f97 T __cio_puts_at   00013bae T _q_init	      00017052 T find_cmdslot
00010fd9 T __cio_puts	   00013c83 T _q_free	      000170a7 T check_WD
0001100a T __cio_write	   00013cb7 T _q_alloc	      00017325 T probe_port
00011042 T __cio_clearscro 00013d79 T _q_length	      00017451 T read_line
000110bb T __cio_clearscre 00013dc7 T _q_insert	      000174dd T enter_index
0001112b T __cio_scroll	   00013f3d T _q_find	      0001752c T is_fs_initializ
00011633 T __cio_printf_at 00013fae T _q_remove	      0001754e T is_inode
00011653 T __cio_printf	   00014081 T _q_removeBy     00017573 T print_menu
000117d5 T __cio_getchar   0001412e T _q_forEach      0001765c T file_shell
0001186a T __cio_gets	   000141aa T _q_dump	      000179d7 T init
000118bb T __cio_input_que 000142bc T _sched_init     00017bfd T idle
000118e6 T __cio_init	   0001435f T _schedule	      00017c2b T wait
00011972 T __put_char_or_c 0001444d T _dispatch	      00017c43 T cwritech
000119ab T __bound	   00014887 T _sio_init	      00017c64 T cwrites
000119cf T __memset	   00014a04 T _sio_enable     00017c91 T cwrite
000119f3 T __memclr	   00014a7b T _sio_disable    00017cac T swritech
00011a14 T __memcpy	   00014af2 T _sio_inq_length 00017ccd T swrites
00011a41 T __strlen	   00014afc T _sio_readc      00017cfa T swrite
00011a63 T __strcmp	   00014b5d T _sio_reads      00017d15 T str2int
00011a99 T __strcpy	   00014bf8 T _sio_writec     00017d87 T strlen
00011ac9 T __strcat	   00014c68 T _sio_write      00017da9 T strcpy
00011aff T __pad	   00014d0f T _sio_puts	      00017dd3 T strcat
00011b21 T __padstr	   00014d4c T _sio_dump	      00017e09 T strcmp
00011bb5 T __sprint	   00014ec9 T _stk_init	      00017e3f T pad
00011e27 T __cvtdec0	   00014f4f T _stk_alloc      00017e61 T padstr
00011eb0 T __cvtuns0	   00014f9b T _stk_free	      00017ef5 T sprint
00011f16 T __cvtdec	   00014fc1 T _stk_setup      00018123 T cvt_dec0
00011f5d T __cvtuns	   0001508e T _stk_dump	      000181ac T cvt_dec
00011f8f T __cvthex	   00015ccf T _sys_init	      000181f3 T cvt_hex
0001200e T __cvtoct	   00015d9d T _vec_clear      00018282 T cvt_oct
0001228a T __panic	   00015e0e T _vec_add	      0001830c T parseArgString
000122a6 T __init_interrup 00015f0f T _vec_remove     000183af T exit
000122b9 T __install_isr   0001604f T _vec_dump	      000183b7 T spawn
000122de T __delay	   00016142 T init_inodes     000183bf T read
0001251c T _clk_init	   000161fa T init_data_block 000183c7 T write
000125a6 T _kreport	   000162a1 T file_system_ini 000183cf T sleep
00012667 T _kinit	   00016301 T create_inode    000183d7 T kill
0001294c T _grow_list	   00016455 T create_data_blo 000183df T waitpid
000129a5 T _kpanic	   000165c2 T inode_read      000183e7 T ushell
00012b06 T _km_init	   0001662b T inode_write     000183ef T bogus







2023-05-08 19:25                                                          Page 2


000183f7 T fallbackExit	   0001f780 B _systemTime     0001fee0 B _activeProcs
0001d000 D __isr_stub_tabl 0001f7a0 B b512	      0001ff44 B _nextPid
0001d50c D __hexdigits	   0001f9a0 B fs_buf	      0001ff48 B _readyLength
0001d520 D _stateStr	   0001fda0 B b256	      0001ff54 B _current
0001d568 D _prioStr	   0001fea0 B _systemStack    0001ff58 B _readyQueue
0001d580 D spawnTable	   0001fea4 B _systemESP      0001ff64 B _sioReadQueue
0001d5b0 B __bss_start	   0001fec0 B _nProcs	      0001ff68 B _sleepQueue
0001d5b0 D _edata	   0001fec4 B _initPCB	      0001ff6c B fs
0001f380 B __isr_table	   0001fec8 B _userShell      0001ff70 B _end




















































