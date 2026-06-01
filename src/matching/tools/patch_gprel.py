#!/usr/bin/env python3
import struct, sys, re

R_MIPS_GPREL16 = 7   # R_MIPS_GPREL16 is type 7 in MIPS ELF ABI
SHT_REL    = 9
SHT_SYMTAB = 2

def patch_gprel16(obj_file, gp_base):
    with open(obj_file, "rb") as f:
        data = bytearray(f.read())
    if data[:4] != b"\x7fELF":
        return
    endian = "<" if data[5] == 1 else ">"
    # ELF32 header (indices into unpacked tuple starting at offset 16):
    # [0]=type [1]=machine [2]=version [3]=entry [4]=phoff [5]=shoff
    # [6]=flags [7]=ehsize [8]=phentsize [9]=phnum [10]=shentsize [11]=shnum [12]=shstrndx
    vals = struct.unpack_from(endian + "HHIIIIIHHHHHH", data, 16)
    e_shoff     = vals[5]
    e_shentsize = vals[10]
    e_shnum     = vals[11]
    e_shstrndx  = vals[12]

    def get_shdr(idx):
        off = e_shoff + idx * e_shentsize
        v = struct.unpack_from(endian + "IIIIIIIIII", data, off)
        k = ("sh_name","sh_type","sh_flags","sh_addr","sh_offset",
             "sh_size","sh_link","sh_info","sh_addralign","sh_entsize")
        return dict(zip(k, v))

    shstrtab   = get_shdr(e_shstrndx)
    shstr_base = shstrtab["sh_offset"]

    def cstr(base, idx):
        s = base + idx
        e = data.index(0, s)
        return data[s:e].decode("utf-8", errors="replace")

    text_shdr = rel_text_shdr = rel_text_idx = symtab_shdr = strtab_shdr = None
    for i in range(e_shnum):
        sh   = get_shdr(i)
        name = cstr(shstr_base, sh["sh_name"])
        if   name == ".text":                                  text_shdr     = sh
        elif name == ".rel.text" and sh["sh_type"] == SHT_REL: rel_text_shdr = sh; rel_text_idx = i
        elif sh["sh_type"] == SHT_SYMTAB:                     symtab_shdr   = sh
        elif name == ".strtab":                                strtab_shdr   = sh

    if not all([text_shdr, rel_text_shdr, symtab_shdr, strtab_shdr]):
        return

    strtab_base = strtab_shdr["sh_offset"]
    sym_size    = symtab_shdr["sh_entsize"]

    def get_sym_name(idx):
        off = symtab_shdr["sh_offset"] + idx * sym_size
        (st_name,) = struct.unpack_from(endian + "I", data, off)
        return cstr(strtab_base, st_name)

    num_rels  = rel_text_shdr["sh_size"] // 8
    rel_base  = rel_text_shdr["sh_offset"]
    text_base = text_shdr["sh_offset"]
    resolved  = []  # indices of entries to remove

    for i in range(num_rels):
        entry_off        = rel_base + i * 8
        r_offset, r_info = struct.unpack_from(endian + "II", data, entry_off)
        r_sym  = r_info >> 8
        r_type = r_info & 0xFF
        if r_type != R_MIPS_GPREL16:
            continue
        sym_name = get_sym_name(r_sym)
        m = re.match(r"^D_([0-9A-Fa-f]{8})$", sym_name)
        if not m:
            continue
        sym_addr = int(m.group(1), 16)
        gprel    = sym_addr - gp_base
        if not (-32768 <= gprel <= 32767):
            continue
        gprel16   = gprel & 0xFFFF
        # Patch instruction bytes in .text
        instr_off = text_base + r_offset
        instr     = struct.unpack_from(endian + "I", data, instr_off)[0]
        instr     = (instr & 0xFFFF0000) | gprel16
        struct.pack_into(endian + "I", data, instr_off, instr)
        resolved.append(i)

    if not resolved:
        return

    # Compact .rel.text: rebuild it without the resolved entries
    keep       = [i for i in range(num_rels) if i not in set(resolved)]
    new_relocs = bytearray()
    for i in keep:
        off = rel_base + i * 8
        new_relocs += data[off:off + 8]

    # Write compact table back to the same file offset (shrinking in-place is safe
    # because ELF section sizes are tracked independently of file layout)
    new_size = len(new_relocs)
    old_size = rel_text_shdr["sh_size"]
    data[rel_base:rel_base + new_size] = new_relocs
    # Zero out the freed space so stale bytes don't confuse tools
    data[rel_base + new_size:rel_base + old_size] = bytes(old_size - new_size)

    # Update sh_size in the section header
    sh_hdr_off = e_shoff + rel_text_idx * e_shentsize
    struct.pack_into(endian + "I", data, sh_hdr_off + 20, new_size)  # sh_size at offset 20

    with open(obj_file, "wb") as f:
        f.write(data)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        sys.exit(1)
    patch_gprel16(obj_file=sys.argv[2], gp_base=int(sys.argv[1], 16))
