import os, queue, re
import multiprocessing
import platform


from subprocess import Popen, STDOUT, PIPE, TimeoutExpired, call

def regx_get(regx, line, index):
    m = re.search(regx, line)
    if m != None and len(m.groups()) > index:
        return m.groups()[index]
    return None

def regx_match(regx, line):
    m = re.search(regx, line)
    if m != None and len(m.group()) != 0:
        return True
    return False

class BitcodeCompiler():
    def __init__(self, source_path, llvm_build_path):
        self.source_path = source_path
        self.llvm_build_path = llvm_build_path
        self.bc_ready = False

        self.cmd_queue = multiprocessing.Queue()

    def run(self):
        #self.adjust_kernel_for_clang()
        self.compile_bc_extra("src/nvidia", "nv-kernel.bc")
        self.compile_bc_extra("src/nvidia-modeset", "nv-modeset-kernel.bc")
        self.link_library()
        self.compile_bc_extra("kernel-open", "", rule_out_options=["-fconserve-stack", "-mrecord-mcount", "-Wno-maybe-uninitialized", "-Wno-alloc-size-larger-than", "-Wimplicit-fallthrough=5"])
        self.link_bc("nvidia.ko.bc", "kernel-open/nvidia")
        self.link_bc("nvidia-drm.ko.bc", "kernel-open/nvidia-drm")
        self.link_bc("nvidia-modeset.ko.bc", "kernel-open/nvidia-modeset")
        self.link_bc("nvidia-peermem.ko.bc", "kernel-open/nvidia-peermem")
        self.link_bc("nvidia-uvm.ko.bc", "kernel-open/nvidia-uvm")
    
    def adjust_kernel_for_clang(self):
        opts = ["-fno-inline-functions", "-fno-builtin-bcmp"]
        self._fix_asm_volatile_goto()
        self._add_extra_options(opts)
    
    def compile_bc_extra(self, sub_dir, bc_name, rule_out_options=[]):
        flag_kernel = False
        base = os.path.join(self.source_path, sub_dir)
        path = os.path.join(base, 'clang_log')
        my_env = os.environ.copy()
        if sub_dir == "kernel-open":
            regx = r'echo \'[ \t]*CC[ \t\[\]A-Z]*(\/([A-Za-z0-9_\-.]+\/)+([A-Za-z0-9_.\-]+))\';'
            self.build_modules(base, modules=True)
            if flag_kernel:
                my_env["NV_KERNEL_SOURCES"] = "/lib/modules/{}/build".format(platform.release())
                my_env["NV_KERNEL_OUTPUT"] = "/lib/modules/{}/build".format(platform.release())
                my_env["M"] = base
                my_env["ARCH"] = "x86_64"
                my_env["NV_KERNEL_MODULES"] = "nvidia nvidia-uvm nvidia-modeset nvidia-drm nvidia-peermem"
                my_env["INSTALL_MOD_DIR"] = "kernel/drivers/video"
                my_env["NV_SPECTRE_V2"] = 0
            flag_kernel = True
        else:
            os.makedirs(os.path.join(base, "_out/Linux_x86_64"), exist_ok=True)
            regx = r'printf " \[ %[-0-9\.s]+ \]  CC[ \t]*(([A-Za-z0-9_\-.]+\/)+([A-Za-z0-9_.\-]+))\\n"'
        
        self.generate_dry_run_log(base)

        procs = []
        for _ in range(0, 16):
            if flag_kernel:
                x = multiprocessing.Process(target=self.executor, args=(
                    '/lib/modules/{}/build/'.format(platform.release()), 
                    my_env, ))
            else:
                x = multiprocessing.Process(target=self.executor, args=(base, my_env, ))
            x.start()
            procs.append(x)
        with open(path, 'r') as f:
            lines = f.readlines()
            for line in lines:
                p2obj = regx_get(regx, line, 0)
                obj = regx_get(regx, line, 2)
                if p2obj == None or obj == None:
                    continue
                #print("CC {}".format(p2obj))
                new_cmd = []
                try:
                    clang_path = '{}/bin/clang'.format(self.llvm_build_path)
                    clangxx_path = '{}/bin/clang++'.format(self.llvm_build_path)
                    if line.find(clangxx_path) != -1:
                        idx1 = line.index(clangxx_path)
                    else:
                        idx1 = line.index(clang_path)
                    if sub_dir == "kernel-open":
                        idx2 = line[idx1:].index(';')
                    else:
                        idx2 = line[idx1:].index('&&')
                    cmd = line[idx1:idx1+idx2].split(' ')
                    if cmd[0] == clang_path:
                        new_cmd.append(cmd[0])
                        new_cmd.append('-emit-llvm')
                        new_cmd.append('-fshort-wchar')
                    for i in range(1, len(cmd)):
                        if cmd[i] == '' or cmd[i] in rule_out_options:
                            continue
                        new_cmd.append(cmd[i])
                except ValueError:
                    print('No \'wllvm\' or \';\' found in \'{}\''.format(line))
                    raise ValueError
                obj = obj[:len(obj)-1] + 'o'
                for i in range(len(new_cmd)):
                    st: str = new_cmd[i]
                    if st.endswith(obj):
                        new_cmd[i] = st[:len(st)-1] + 'bc'
                self.cmd_queue.put(new_cmd)
                """p = Popen(new_cmd, cwd=base, stdout=PIPE, stderr=PIPE)
                try:
                    p.wait(timeout=5)
                except TimeoutExpired:
                    if p.poll() == None:
                        p.kill()
                """

            self.bc_ready=True
            for p in procs:
                p.join()
            exitcode = 0
            if bc_name != None and bc_name != "":
                exitcode = self.link_bc(bc_name, base)
            if not flag_kernel:
                self.build_modules(base)
            return exitcode
    
    def link_bc(self, bc_name, base):
        if os.path.exists(os.path.join(self.source_path,bc_name)):
            os.remove(os.path.join(self.source_path, bc_name))
        link_cmd = '{0}/bin/llvm-link -o {1} `find ./ -name "*.bc" ! -name "{1}"`'.format(self.llvm_build_path, bc_name)
        print(link_cmd)
        p = Popen(['/bin/bash','-c', link_cmd], stdout=PIPE, stderr=STDOUT, cwd=base)
        with p.stdout:
            self._log_subprocess_output(p.stdout)
        exitcode = p.wait()
        if exitcode != 0:
            print("Fail to construct a monolithic bc")
                
    def executor(self, base, env):
        while not self.bc_ready or not self.cmd_queue.empty():
            try:
                cmd = self.cmd_queue.get(block=True, timeout=5)
                obj = cmd[len(cmd)-1]
                print("CC {}".format(obj))
                #call(" ".join(cmd), shell=True, env=env, cwd=base)
                p = Popen(" ".join(cmd), shell=True, cwd=base, env=env, stdout=PIPE, stderr=PIPE)
                try:
                    p.wait(timeout=15)
                except TimeoutExpired:
                    if p.poll() == None:
                        p.kill()
                if p.poll() == None:
                    p.kill()
            except queue.Empty:
                # get() is multithreads safe
                # 
                break
    
    def generate_dry_run_log(self, base):
        my_env = os.environ.copy()
        my_env["CC"] = os.path.join(self.llvm_build_path, "bin/clang")
        my_env["CXX"] = os.path.join(self.llvm_build_path, "bin/clang++")
        p = Popen("make -n > clang_log", shell=True, cwd=base, env=my_env, stdout=PIPE, stderr=PIPE)
        try:
            p.wait(timeout=500)
        except TimeoutExpired:
            if p.poll() == None:
                p.kill()
        if p.poll() == None:
            p.kill()
        return
    
    def build_modules(self, base, modules=False):
        if modules:
            call("make modules -j`nproc`", shell=True, cwd=base)
        else:
            call("make -j`nproc`", shell=True, cwd=base)
        return
    
    def link_library(self):
        dst = os.path.join(self.source_path, "kernel-open/nvidia/nv-kernel.o_binary")
        if not os.path.exists(dst):
            self._sym_link(os.path.join(self.source_path, "src/nvidia/_out/Linux_x86_64/nv-kernel.o"), dst)
        
        dst = os.path.join(self.source_path, "kernel-open/nvidia/nv-kernel.bc")
        if not os.path.exists(dst):
            self._sym_link(os.path.join(self.source_path, "src/nvidia/nv-kernel.bc"), dst)
        
        dst = os.path.join(self.source_path, "kernel-open/nvidia-modeset/nv-modeset-kernel.o_binary")
        if not os.path.exists(dst):
            self._sym_link(os.path.join(self.source_path, "src/nvidia-modeset/_out/Linux_x86_64/nv-modeset-kernel.o"), dst)
        
        dst = os.path.join(self.source_path, "kernel-open/nvidia-modeset/nv-modeset-kernel.bc")
        if not os.path.exists(dst):
            self._sym_link(os.path.join(self.source_path, "src/nvidia-modeset/nv-modeset-kernel.bc"), dst)
        
    def _sym_link(self, src, dst):
        os.symlink(src, dst)
    
    def _fix_asm_volatile_goto(self):
        regx = r'#define asm_volatile_goto'
        linux_repo = self.source_path
        compiler_gcc = os.path.join(linux_repo, "include/linux/compiler-gcc.h")
        buf = ''
        if os.path.exists(compiler_gcc):
            with open(compiler_gcc, 'r') as f_gcc:
                lines = f_gcc.readlines()
                for line in lines:
                    if regx_match(regx, line):
                        buf = line
                        break
            if buf != '':
                compiler_clang = os.path.join(linux_repo, "include/linux/compiler-clang.h")
                with open(compiler_clang, 'r+') as f_clang:
                    lines = f_clang.readlines()
                    data = [buf]
                    data.extend(lines)
                    f_clang.seek(0)
                    f_clang.writelines(data)
                    f_clang.truncate()
        return

    def _add_extra_options(self, opts):
        regx = r'KBUILD_CFLAGS[ \t]+:='
        linux_repo = self.source_path
        makefile = os.path.join(linux_repo, "Makefile")
        data = []
        with open(makefile, 'r+') as f:
            lines = f.readlines()
            for i in range(0, len(lines)):
                line = lines[i]
                if regx_match(regx, line):
                    parts = line.split(':=')
                    opts_str = " ".join(opts)
                    data.extend(lines[:i])
                    data.append(parts[0] + ":= " + opts_str + " " + parts[1])
                    data.extend(lines[i+1:])
                    f.seek(0)
                    f.writelines(data)
                    f.truncate()
                    break
    
    def _log_subprocess_output(self, pipe):
        try:
            for line in iter(pipe.readline, b''):
                try:
                    line = line.decode("utf-8").strip('\n').strip('\r')
                except:
                    print('bytes array \'{}\' cannot be converted to utf-8'.format(line))
                    continue
                print(line)
        except ValueError:
            if pipe.close:
                return
            
if __name__ == "__main__":
    compiler = BitcodeCompiler(os.getcwd(), "/root/llvm/clang+llvm-14.0.0-x86_64-linux-gnu-ubuntu-18.04")
    compiler.run()