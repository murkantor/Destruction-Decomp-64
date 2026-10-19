def apply(config, args):
    config["baseimg"] = "baserom.z64"
    config["myimg"] = "build/dd64.z64"
    config["mapfile"] = "build/dd64.map"
    config["source_directories"] = ["src", "include", "asm"]
    config["objdump_executable"] = "mips-linux-gnu-objdump"
    config["make_command"] = ["make", "all"]
    config["arch"] = "mips"
