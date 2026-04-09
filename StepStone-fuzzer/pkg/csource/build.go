// Copyright 2017 syzkaller project authors. All rights reserved.
// Use of this source code is governed by Apache 2 LICENSE that can be found in the LICENSE file.

package csource

import (
	"bytes"
	"fmt"
	"os"
	"runtime"
	"strings"

	"github.com/google/syzkaller/pkg/osutil"
	"github.com/google/syzkaller/prog"
	"github.com/google/syzkaller/sys/targets"
)

// Build builds a C program from source src and returns name of the resulting binary.
func Build(target *prog.Target, calls []*prog.Call, src []byte) (string, error) {
	reproCProg, err := osutil.TempFile("repro.cprog")
	if err != nil {
		return "", err
	}
	reproCProg = reproCProg + ".c"
	err = os.WriteFile(reproCProg, src, 0644)
	if err != nil {
		return "", err
	}
	return BuildFile(target, calls, reproCProg, "")
}

// BuildNoWarn is the same as Build, but ignores all compilation warnings.
// Should not be used in tests, but may be used e.g. when we are bisecting and potentially
// using an old repro with newer compiler, or a compiler that we never seen before.
// In these cases it's more important to build successfully.
func BuildNoWarn(target *prog.Target, calls []*prog.Call, src []byte) (string, error) {
	reproCProg, err := osutil.TempFile("repro.cprog")
	if err != nil {
		return "", err
	}
	reproCProg = reproCProg + ".c"
	err = os.WriteFile(reproCProg, src, 0644)
	if err != nil {
		return "", err
	}
	return BuildFile(target, calls, reproCProg, "-fpermissive", "-w")
}

// BuildFile builds a C/C++ program from file src and returns name of the resulting binary.
func BuildFile(target *prog.Target, calls []*prog.Call, src string, cflags ...string) (string, error) {
	return build(target, calls, nil, src, cflags...)
}

func build(target *prog.Target, calls []*prog.Call, src []byte, file string, cflags ...string) (string, error) {
	sysTarget := targets.Get(target.OS, target.Arch)
	compiler := sysTarget.CCompiler
	syz_env := "tools/syz-env"
	// We call the binary syz-executor because it sometimes shows in bug titles,
	// and we don't want 2 different bugs for when a crash is triggered during fuzzing and during repro.
	bin, err := osutil.TempFile("syz-executor")
	if err != nil {
		return "", err
	}

	flags := []string{
		"-o", bin,
		"-DGOOS_" + target.OS + "=1",
		"-DGOARCH_" + target.Arch + "=1",
		"-DHOSTGOOS_" + runtime.GOOS + "=1",
	}
	if file == "" {
		flags = append(flags, "-x", "c", "-")
	} else {
		flags = append(flags, file)
	}
	flags = append(flags, sysTarget.CFlags...)
	if sysTarget.PtrSize == 4 {
		// We do generate uint64's for syscall arguments that overflow longs on 32-bit archs.
		flags = append(flags, "-Wno-overflow")
	}
	for _, call := range calls {
		if strings.HasPrefix(call.Meta.CallName, "syz_cu") {
			flags = append(flags, "-I /usr/local/cuda/include")
			flags = append(flags, "-lcuda")
			break
		}
	}

	for _, call := range calls {
		if strings.HasPrefix(call.Meta.CallName, "syz_cl") {
			flags = append(flags, "-I /usr/local/cuda/include")
			flags = append(flags, "-lOpenCL")
			break
		}
	}

	for _, call := range calls {
		if strings.HasPrefix(call.Meta.CallName, "syz_vk") {
			flags = append(flags, "-lvulkan")
			break
		}
	}

	flags = append(flags, cflags...)
	buildCmd := compiler + " " + strings.Join(flags, " ")
	cmd := osutil.Command(syz_env, buildCmd)
	if file == "" {
		cmd.Stdin = bytes.NewReader(src)
	}
	cmd.Env = os.Environ()
	cmd.Env = append(cmd.Env, "CI=1")
	out, err := cmd.CombinedOutput()
	if err != nil {
		os.Remove(bin)
		if file != "" {
			src, _ = os.ReadFile(file)
		}
		return "", fmt.Errorf("failed to build program:\n%s\n%s\ncompiler invocation: %v %v",
			src, out, compiler, flags)
	}
	return bin, nil
}

// Format reformats C source using clang-format.
func Format(src []byte) ([]byte, error) {
	stdout, stderr := new(bytes.Buffer), new(bytes.Buffer)
	cmd := osutil.Command("clang-format", "-assume-filename=/src.c", "-style", style)
	cmd.Stdin = bytes.NewReader(src)
	cmd.Stdout = stdout
	cmd.Stderr = stderr
	if err := cmd.Run(); err != nil {
		return src, fmt.Errorf("failed to format source: %w\n%v", err, stderr.String())
	}
	return stdout.Bytes(), nil
}

// Something acceptable for kernel developers and email-friendly.
var style = `{
BasedOnStyle: LLVM,
IndentWidth: 2,
UseTab: Never,
BreakBeforeBraces: Linux,
IndentCaseLabels: false,
DerivePointerAlignment: false,
PointerAlignment: Left,
AlignTrailingComments: true,
AllowShortBlocksOnASingleLine: false,
AllowShortCaseLabelsOnASingleLine: false,
AllowShortFunctionsOnASingleLine: false,
AllowShortIfStatementsOnASingleLine: false,
AllowShortLoopsOnASingleLine: false,
ColumnLimit: 80,
}`
