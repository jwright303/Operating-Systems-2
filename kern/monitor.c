// Simple command-line kernel monitor useful for
// controlling the kernel and exploring the system interactively.

#include <inc/stdio.h>
#include <inc/string.h>
#include <inc/memlayout.h>
#include <inc/assert.h>
#include <inc/x86.h>

#include <kern/console.h>
#include <kern/monitor.h>
#include <kern/kdebug.h>
#include <kern/trap.h>

#define CMDBUF_SIZE	80	// enough for one VGA text line


struct Command {
	const char *name;
	const char *desc;
	// return -1 to force monitor to exit
	int (*func)(int argc, char** argv, struct Trapframe* tf);
};

// LAB 1: add your command to here...
static struct Command commands[] = {
	{ "help", "Display this list of commands", mon_help },
	{ "kerninfo", "Display information about the kernel", mon_kerninfo },
    { "backtrace", "Display the backtrace of the stack", mon_backtrace },
    { "show", "Display sum fun ascii art", mon_show }
    //{ "backtrace", "Display the backtrace of the stack", mon_backtrace }
};

/***** Implementations of basic kernel monitor commands *****/

int
mon_help(int argc, char **argv, struct Trapframe *tf)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(commands); i++)
		cprintf("%s - %s\n", commands[i].name, commands[i].desc);
	return 0;
}

int
mon_kerninfo(int argc, char **argv, struct Trapframe *tf)
{
	extern char _start[], entry[], etext[], edata[], end[];

	cprintf("Special kernel symbols:\n");
	cprintf("  _start                  %08x (phys)\n", _start);
	cprintf("  entry  %08x (virt)  %08x (phys)\n", entry, entry - KERNBASE);
	cprintf("  etext  %08x (virt)  %08x (phys)\n", etext, etext - KERNBASE);
	cprintf("  edata  %08x (virt)  %08x (phys)\n", edata, edata - KERNBASE);
	cprintf("  end    %08x (virt)  %08x (phys)\n", end, end - KERNBASE);
	cprintf("Kernel executable memory footprint: %dKB\n",
		ROUNDUP(end - entry, 1024) / 1024);
	return 0;
}

int
mon_backtrace(int argc, char **argv, struct Trapframe *tf)
{
	// LAB 1: Your code here.
    // HINT 1: use read_ebp().
    // HINT 2: print the current ebp on the first line (not current_ebp[0])
    int *ebp_val = (int *) read_ebp();
    //int *next_ebp = (int *) ebp_val[0];

    cprintf("Stack backtrace:\n");
    for (int j = 0; j < 10; j++) {
        cprintf("  ebp %08x  eip %08x  args ", ebp_val, ebp_val[1]);
	    for (int i = 2; i <= 6; i++) {
            cprintf("%08x ", ebp_val[i]);
        }
        cprintf("\n");
        struct Eipdebuginfo info;
        debuginfo_eip(ebp_val[1], &info);
        cprintf("         %s:%d: ",
                info.eip_file, info.eip_line, info.eip_fn_name, ebp_val[1] - info.eip_fn_addr);
        cprintf("%.*s+%d\n", strlen(info.eip_fn_name)-8, info.eip_fn_name, ebp_val[1] - info.eip_fn_addr);


        ebp_val = (int *) ebp_val[0];

        if(ebp_val == 0) {
            break;
        }
    }

    return 0;
}

int
mon_show(int argc, char **argv, struct Trapframe *tf)
{
    cprintf("\x1b[36m\n");
    cprintf("                 000                  \n");
    cprintf("\x1b[m");
    cprintf("\x1b[35m");
    cprintf("               00   00                \n");
    cprintf("\x1b[m");
    cprintf("\x1b[34m");
    cprintf("              00 o o 00               \n");
    cprintf("\x1b[m");
    cprintf("\x1b[33m");
    cprintf("              00     00               \n");
    cprintf("\x1b[m");
    cprintf("\x1b[32m");
    cprintf("              00 |_| 00               \n");
    cprintf("\x1b[m");
    cprintf("\x1b[31m");
    cprintf("               00   00                \n");
    cprintf("\x1b[m");
    cprintf("\x1b[37m");
    cprintf("                 000 \x1b[m           \n");
    cprintf("                  0                   \n");
    cprintf("            \x1b[41m 00000000000 \x1b[m\n");
    cprintf("                 \x1b[41m 0 \x1b[m\n");
    cprintf("                 \x1b[41m 0 \x1b[m\n");
    cprintf("                 \x1b[47m 0 \x1b[m\n");
    cprintf("                 \x1b[41m 0 \x1b[m\n");
    cprintf("                 \x1b[41m 0 \x1b[m\n");
    cprintf("                 \x1b[41m 0 \x1b[m\n");
    cprintf("                \x1b[41m 0 0 \x1b[m\n");
    cprintf("               \x1b[41m 0 \x1b[m \x1b[41m 0 \x1b[m\n");
    cprintf("              \x1b[41m 0 \x1b[m   \x1b[41m 0 \x1b[m\n");
    cprintf("             \x1b[41m 0 \x1b[m     \x1b[41m 0 \x1b[m\n");

    return 0;
}


/***** Kernel monitor command interpreter *****/

#define WHITESPACE "\t\r\n "
#define MAXARGS 16

static int
runcmd(char *buf, struct Trapframe *tf)
{
	int argc;
	char *argv[MAXARGS];
	int i;

	// Parse the command buffer into whitespace-separated arguments
	argc = 0;
	argv[argc] = 0;
	while (1) {
		// gobble whitespace
		while (*buf && strchr(WHITESPACE, *buf))
			*buf++ = 0;
		if (*buf == 0)
			break;

		// save and scan past next arg
		if (argc == MAXARGS-1) {
			cprintf("Too many arguments (max %d)\n", MAXARGS);
			return 0;
		}
		argv[argc++] = buf;
		while (*buf && !strchr(WHITESPACE, *buf))
			buf++;
	}
	argv[argc] = 0;

	// Lookup and invoke the command
	if (argc == 0)
		return 0;
	for (i = 0; i < ARRAY_SIZE(commands); i++) {
		if (strcmp(argv[0], commands[i].name) == 0)
			return commands[i].func(argc, argv, tf);
	}
	cprintf("Unknown command '%s'\n", argv[0]);
	return 0;
}

void
monitor(struct Trapframe *tf)
{
	char *buf;

	cprintf("Welcome to the JOS kernel monitor!\n");
	cprintf("Type 'help' for a list of commands.\n");

	if (tf != NULL)
		print_trapframe(tf);

	while (1) {
		buf = readline("K> ");
		if (buf != NULL)
			if (runcmd(buf, tf) < 0)
				break;
	}
}
