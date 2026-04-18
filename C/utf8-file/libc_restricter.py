import subprocess
import tempfile


def gen_libc_restricter(prohibited_fns):
    content = "#include <unistd.h>\n#include <signal.h>\n"
    for fn in prohibited_fns:
        content += "void {0}() {{ const char msg[] = \"call to libc function '{0}' is prohibited\\n\"; write(2, msg, sizeof(msg) - 1); raise(SIGABRT); }}\n".format(fn)

    with open("libc_restrict.c", "w") as f:
        f.write(content)

    subprocess.run(["gcc", "-shared", "libc_restrict.c", "-fno-builtin", "-o", "libc_restricter.so"], check=True)

    return

gen_libc_restricter(prohibited_fns=["fopen", "fread", "fwrite", "fclose"])
