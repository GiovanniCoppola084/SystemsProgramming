#include "ahci.h"

#define PCI_CONFIG_ADDRESS 0xCF8	// PCI configuration address register offset
#define PCI_CONFIG_DATA 0xCFC		// PCI configuration data register offset

#define	SATA_SIG_ATA	0x00000101	// SATA drive
#define	SATA_SIG_ATAPI	0xEB140101	// SATAPI drive
#define	SATA_SIG_SEMB	0xC33C0101	// Enclosure management bridge
#define	SATA_SIG_PM		0x96690101	// Port multiplier
 
#define AHCI_DEV_NULL		0
#define AHCI_DEV_SATA 		1
#define AHCI_DEV_SEMB 		2
#define AHCI_DEV_PM 		3
#define AHCI_DEV_SATAPI 	4
 
#define HBA_PORT_IPM_ACTIVE 	1
#define HBA_PORT_DET_PRESENT 	3

#define HBA_PxCMD_ST   	 	0x0001  // ST - Start (command processing)
#define HBA_PxCMD_FRE   	0x0010  // FRE - FIS Receive Enable
#define HBA_PxCMD_FR    	0x4000  // FR - FIS receive Running
#define HBA_PxCMD_CR    	0x8000	// CR - Command list Running

#define HBA_PxIS_PSS		0x0002	// 
#define HBA_PxIS_DPS		0x0020	// 
#define HBA_PxIS_TFES   0x40000000  // TFES - Task File Error Status

#define ATA_DEV_BUSY 	0x80
#define ATA_DEV_DRQ 	0x08

#define ATA_CMD_READ_DMA_EX		0x25	// Read Command
#define ATA_CMD_WRITE_DMA_EX	0x35	// Write Command
#define ATA_CMD_IDENTIFY        0xEC	// Identify Command

static inline void outl(uint16_t port, uint32_t val) {

    __asm__ volatile ( "out %0, %1" : : "a"(val), "Nd"(port) );
}

static inline uint32_t inl(uint16_t port) {
    uint32_t ret;
    __asm__ volatile ( "inl %w1, %0" : "=a"(ret) : "Nd"(port) );

    return ret;
}

void write_pci_config(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint32_t val) {
	uint32_t address = (uint32_t)((bus<<16) | (slot << 11) | (func << 8) | offset| ((uint32_t)0x80000000));
	outl(PCI_CONFIG_ADDRESS, address);
    outl(val, PCI_CONFIG_DATA);
}


uint32_t read_pci_config(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    
    uint32_t address = (uint32_t)((bus<<16) | (slot << 11) | (func << 8) | offset | ((uint32_t)0x80000000));
    outl(PCI_CONFIG_ADDRESS, address);

    return inl(PCI_CONFIG_DATA);
}

void _ahci_isr(int vector, int code) {
	__cio_printf("AHCI Interrupt\n");
}

void _ahci_init() {
	__install_isr(INT_VEC_AHCI, _ahci_isr);
    
	uint32_t abar = find_ahci();

    probe_port((HBA_MEM *)abar); 
}

uint64_t find_ahci() {
    uint8_t bus, slot;//, function;
    uint16_t vendor_id, device_id;
    for (bus = 0 ; bus < 256; bus++) {
        for (slot = 0; slot < 32; slot++) {
            uint32_t config = read_pci_config(bus, slot, 0, 0);
            vendor_id = (uint16_t)config;
            device_id = (uint16_t)(config>>16); 
            uint32_t config2 = read_pci_config(bus, slot, 0, 0x8);
            uint8_t class = (uint8_t)(config2 >> 24);
            uint8_t subclass = (uint8_t)(config2 >> 16);

            if (vendor_id == 0x8086 && class == 0x01 && subclass == 0x06) {
                // BAR5 is HBA register location
				__cio_printf("DEVICE[%x] from VENDOR[%x] with CLASS[%x] and SUBCLASS[%x]\n",
					device_id, vendor_id, class, subclass);
				__cio_printf("found on BUS[%d], SLOT[%d]\n", bus, slot);

                return read_pci_config(bus, slot, 0, 0x24);                 
            }          
        }
    }
}

// Start command engine
void start_cmd(HBA_PORT *port) {
	// Wait until CR (bit15) is cleared
	while (port->cmd & HBA_PxCMD_CR);
 
	// Set FRE (bit4) and ST (bit0)
	port->cmd |= HBA_PxCMD_FRE;
	port->cmd |= HBA_PxCMD_ST; 
}
 
// Stop command engine
void stop_cmd(HBA_PORT *port) {
	// Clear ST (bit0)
	port->cmd &= ~HBA_PxCMD_ST;
 
	// Clear FRE (bit4)
	port->cmd &= ~HBA_PxCMD_FRE;
 
	// Wait until FR (bit14), CR (bit15) are cleared
	while(1) {
		if (port->cmd & HBA_PxCMD_FR)
			continue;
		if (port->cmd & HBA_PxCMD_CR)
			continue;
		break;
	}
}

void port_rebase(HBA_PORT *port, int portno) {
	stop_cmd(port);	// Stop command engine
	
	// Command list offset: 1K*portno
	// Command list entry size = 32
	// Command list entry maxim count = 32
	// Command list maxim size = 32*32 = 1K per port
	void *clb_alloc = _km_slice_alloc();
	port->clb = (uint32_t)clb_alloc;
	port->clbu = 0;

	__memset((void*)(port->clb), 0, 1024);
 
	// FIS offset: 32K+256*portno
	// FIS entry size = 256 bytes per port
	void *fb_alloc = _km_slice_alloc();
	port->fb = (uint32_t)fb_alloc;
	port->fbu = 0;
	__memset((void*)(port->fb), 0, 256);
 
	// Command table offset: 40K + 8K*portno
	// Command table size = 256*32 = 8K per port
	void *cmd_alloc = _km_page_alloc(2);
	HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*)(port->clb);

	for (int i=0; i<32; i++) {
		cmdheader[i].prdtl = 8;	// 8 prdt entries per command table
					// 256 bytes per command table, 64+16+48+16*8
		// Command table offset: 40K + 8K*portno + cmdheader_index*256
		cmdheader[i].ctba = (uint32_t)cmd_alloc;
		cmdheader[i].ctbau = 0;
		__memset((void*)cmdheader[i].ctba, 0, 256);
	}
	start_cmd(port);	// Start command engine
}
 
//Find a free command list slot
int find_cmdslot(HBA_PORT *port, int num_of_slots) {
	// If not set in SACT and CI, the slot is free
	uint32_t slots = (port->sact | port->ci);
	for (int i=0; i < num_of_slots; i++)
	{
		if ((slots&1) == 0)
			return i;
		slots >>= 1;
	}
	__cio_printf("Cannot find free command list entry\n");
	return -1;
}

void check_WD(HBA_MEM *abar, int portno) {
    HBA_PORT *port = &abar->ports[portno];

    port->is = 0xffffffff;		// Clear pending interrupt bits
	port->ie = 0xffffffff;		// Enable all interrupts

	int spin = 0; // Spin lock timeout counter

    int num_slots = (abar->cap & 0x0f00)>>8 ; // Bit 8-12
	int slot = find_cmdslot(port, num_slots);
	if (slot == -1)
		return;
	
	HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*)port->clb;
	cmdheader += slot;
    
	cmdheader->cfl = sizeof(FIS_REG_H2D)/sizeof(uint32_t);		// Command FIS size (H2D is 4 dwords)
	cmdheader->prdtl = 1;	// PRDT entries count
 
	HBA_CMD_TBL *cmdtbl = (HBA_CMD_TBL*)(cmdheader->ctba);

	void * tmpptr = _km_slice_alloc();
	uint32_t addr = (uint32_t)tmpptr;

	cmdtbl->prdt_entry[0].dba = addr;
	cmdtbl->prdt_entry[0].dbc = 0x1FF;
	cmdtbl->prdt_entry[0].i = 1;

	// Setup command
	FIS_REG_H2D *cmdfis = (FIS_REG_H2D*)(&cmdtbl->cfis);

	cmdfis->fis_type = FIS_TYPE_REG_H2D;
    cmdfis->command = ATA_CMD_IDENTIFY;
	cmdfis->pmport = 0;
    cmdfis->featurel = 1;
	cmdfis->c = 1;	// Command
    

	while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000) {
		spin++;
	}

	if (spin == 1000000) {
		__cio_printf("Port is hung\n");
		return;
	}

	// Issue command
	port->ci = 1 << slot; 

	while (1) {
		// Command succeeds
		if ((port->is & HBA_PxIS_DPS) && (port->is & HBA_PxIS_PSS) && ((port->ci & (1<<slot)) == 0)) {
			break;
		}
		// Task file error
		if (port->is & HBA_PxIS_TFES) {	
			__cio_printf("Read disk error\n");
			return;
		}
	}

    uint32_t response_address = cmdtbl->prdt_entry[0].dba;
    uint16_t *ptr2 = (uint16_t *) response_address;
    uint16_t model_num = ptr2[27];
	char lo = model_num & 0xFF;
	char hi = model_num >> 8;

	__cio_printf("Start of model number for drive on port %d: %c%c\n", portno, hi, lo);
}

// Check device type
static int check_type(HBA_PORT *port) {
	uint32_t ssts = port->ssts;
 
	uint8_t ipm = (ssts >> 8) & 0x0F;
	uint8_t det = ssts & 0x0F;
    
	if (det != HBA_PORT_DET_PRESENT)	// Check drive status
		return AHCI_DEV_NULL;
	if (ipm != HBA_PORT_IPM_ACTIVE)
		return AHCI_DEV_NULL;
 
	switch (port->sig) {
	case SATA_SIG_ATAPI:
		return AHCI_DEV_SATAPI;
	case SATA_SIG_SEMB:
		return AHCI_DEV_SEMB;
	case SATA_SIG_PM:
		return AHCI_DEV_PM;
	default:
		return AHCI_DEV_SATA;
	}
}

void probe_port(HBA_MEM *abar) {
	abar->ghc |= 1<<31;	//AHCI Enable
	abar->ghc |= 1<<1;	//Interrupts Enable

	// Search disk in implemented ports
	uint32_t pi = abar->pi;
	int i = 0;

	while (i<32) {
		if (pi & 1) {
			int dt = check_type(&abar->ports[i]);

			if (dt == AHCI_DEV_SATA) {
				__cio_printf("\nSATA drive found at port %d\n", i);
				
                port_rebase(&abar->ports[i], i);
                check_WD(abar, i);
			} else if (dt == AHCI_DEV_SATAPI) {
				__cio_printf("SATAPI drive found at port %d\n", i);
			} else if (dt == AHCI_DEV_SEMB)	{
				__cio_printf("SEMB drive found at port %d\n", i);
			} else if (dt == AHCI_DEV_PM) {
				__cio_printf("PM drive found at port %d\n", i);
			} else {
				__cio_printf("No drive found at port %d\n", i);
			}
		}
		pi >>= 1;
		i ++;
	}
}