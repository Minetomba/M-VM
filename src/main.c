#include <stdio.h>
#include <stdint.h>

typedef uint64_t standard;

standard in_ports[64 + 1];
standard out_ports[64 + 1];
standard registers[16 + 1];
standard memory[4096];
/*
ldi r1, i1 | 2,1,1
ldi r2, i2 | 2,2,2
ldi r3, i2 | 2,3,2
out p1, r1 | 1,1,1
out p2, r2 | 1,2,2
out p3, r3 | 1,3,3
in r4, p1  | 0,4,1
hlt        | 3,0,0
*/
standard program_read_only[] = {2,1,1,2,2,2,2,3,2,1,1,1,1,2,2,1,3,3,0,4,1,3,0,0};

// Auxiliary modules
void alu() { // Occupied: out port 1 (op), out port 2 (arg1), out port 3 (arg2), in port 1 (out)
	if (out_ports[1] == 1) { // Add
		in_ports[1] = out_ports[2] + out_ports[3];
	} else if (out_ports[1] == 2) { // Nand
		in_ports[1] = ~(out_ports[2] & out_ports[3]);
	} else if (out_ports[1] == 3) { // Right shift
		in_ports[1] = out_ports[2] >> out_ports[3];
	} else if (out_ports[1] == 4) { // Less than
		if (out_ports[2] < out_ports[3]) {
			in_ports[1] = 1;
		} else {
			in_ports[1] = 0;
		}
	} else if (out_ports[1] == 5) { // Greater than
		if (out_ports[2] > out_ports[3]) {
			in_ports[1] = 1;
		} else {
			in_ports[1] = 0;
		}
	} else if (out_ports[1] == 6) { // Equal to
		if (out_ports[2] == out_ports[3]) {
			in_ports[1] = 1;
		} else {
			in_ports[1] = 0;
		}
	} else if (out_ports[1] == 7) { // Mimick arg1
		in_ports[1] = out_ports[2];
	}
	return;
}
void ram() { // Occupied: out port 4 (addr), out port 5 (value for write), out port 6 (write opened?), in port 2 (read value)
	if (out_ports[6] == 1) {
		memory[out_ports[4]] = out_ports[5];
	}
	in_ports[2] = memory[out_ports[4]];
	return;
}
void jumpingifnotzero() { // Occupied: out port 7 (PC now), out port 8 (target PC), out port 9 (input), in port 3 (next PC jump address)
	if (out_ports[9] == 0) {
		in_ports[3] = out_ports[7];
	} else {
		in_ports[3] = out_ports[8];
	}
	return;
}

// Auxiliary modules calling
void update_ports() {
	alu();
	ram();
	jumpingifnotzero();
	return;
}

// Helper functions
void regdump() {
	standard i = 0;
	while (i <= 16) {
		printf("r%lu: i%lu\n", i, registers[i]);
		i++;
	}
	return;
}

// Core interpretation
int main() {
	registers[16] = -3;
	while (1) {
		registers[16] += 3;
		update_ports();
		standard opcode = program_read_only[registers[16]];
		standard arg1 = program_read_only[registers[16] + 1];
		standard arg2 = program_read_only[registers[16] + 2];
		if (opcode == 0) {
			registers[arg1] = in_ports[arg2];
		} else if (opcode == 1) {
			out_ports[arg1] = registers[arg2];
		} else if (opcode == 2) {
			registers[arg1] = arg2;
		} else if (opcode == 3) {
			regdump();
			return 0;
		}
	}
}