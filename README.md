# Butcher
A binary deconstructor
## What is Butcher?
Butcher is a binary deconstructor, a tool to extract useful code from compiled programs.
## What can I do with Butcher?
With Butcher you can extract part of a binary as source code and use it in a new tool.
## Can you put and example?
Imagine you're analyzing a malware and you discover a function used to decrypt the configuration. With Butcher, you can extract this function as C or Python source code and use it into a different utility that decrypts malware configurations.
## Is Butcher a decompiler?
Yes, it could be called a generic decompiler
# Tutorial

## Butchering the **GetSecret** function.

Let's start with an easy example:
1. Clone this project, compile it and move to the tutorial directory:
```
git clone https://github.com/redsnk/butcher
sudo apt install build-essential
sudo apt install cmake
sudo apt-get install libcapstone-dev
cd butcher/
cmake CMakeLists.txt
make
cd tutorial
```
2. Unzip the sample:

> zip protected with the password "*infected*"

```
unzip -P infected libffi-6.zip
```

> *libffi-6.dll* is a malware called *Grandoreiro*, a tipical Brasilian RAT compiled with **Delphi**

If we examine this malware with [IDR](https://github.com/crypto2011/IDR) we can identify two functions used to decrypt hidden strings:

![Main.](./tutorial/main.png "Main.")

| Address  | Name | Function | Parameters |
| ------------- | ------------- | ------------- | ------------- |
| **047E3AE9** | **GetSecret1** | Retrieve encrypted string | **eax** = number of secret to retrieve |
| **047E3AF4** | **Decrypt**    | Decrypts the string | **eax** = encrypted string, **edx** = decrypted string |

Now we are going to clone the **GetSecret1** function.

The **GetSecret1** starts at **044FCB68**:

![Secret.](./tutorial/secret.png "Secret.")

3. Extract de **GetSecret** function from the malware:

```
../Butcher -lc -a -m -e0x40b830 "libffi-6.dll" "0x044FCB68" > secret.c
```

> -lc 
>> Creates C source code.

> -a
>> Include the original assembler code as commented lines.

> -m
>> Load the memory from the original file.

> -e0x40b830
>> Exclude the address 0x40b830 from the analisis, this address is used to create an internal Delphi string that is no necessary in this case.

4. Modify the **main** function at **secret.c** below the "*Insert code here ...*" lines:

```
int main (int argc, char **argv) {
struct _cpu c,*cpu;

    cpu = &c;
    init(cpu);
    add_mem(cpu,0xf000,NULL,10240);
    load_mem(cpu,"libffi-6.dll",0x400,0x4458a00,0x401000,0x4459000);
    load_mem(cpu,"libffi-6.dll",0x4458e00,0x8800,0x485a000,0x9000);
    load_mem(cpu,"libffi-6.dll",0x4461600,0x22200,0x4863000,0x23000);
    load_mem(cpu,"libffi-6.dll",0x0,0x0,0x4886000,0x9e000);
    load_mem(cpu,"libffi-6.dll",0x4483800,0x4e00,0x4924000,0x5000);
    load_mem(cpu,"libffi-6.dll",0x4488600,0x6400,0x4929000,0x7000);
    load_mem(cpu,"libffi-6.dll",0x448ea00,0x600,0x4930000,0x1000);
    load_mem(cpu,"libffi-6.dll",0x448f000,0x200,0x4931000,0x1000);
    load_mem(cpu,"libffi-6.dll",0x448f200,0xb4800,0x4932000,0xb5000);
    load_mem(cpu,"libffi-6.dll",0x4543a00,0x223000,0x49e7000,0x224000);
    _rsp = 0x10400;
    _rbp = _rsp;
    /* Insert code here ... */
    if (argc > 1) {
        _eax = atoi(argv[1]);
    }
    func_0x44fcb68(cpu);
    /* Insert code here ... */
    char *str = get_unicode_ptr(cpu,_edx);
    printf("%s\n",str);
    free(str);
    end(cpu);
    return (0);
}
```

5. Compile the new tool:

```
gcc -I../src/emu/ ../src/emu/butcher_x64.c secret.c -o secret
```

6. Execute the new tool:

```
./secret 5
```

> MkIzMjk1QzMwQzRBRjkxNQ==

7. Let's try with **Python**:

> *** **Warning, this version only works with Python 3.7** ***

```
../Butcher -lp -a -m -e0x40b830 "libffi-6.dll" "0x044FCB68" > secret.py
```

> -lp
>> Creates Python source code.

8. Modify the **main** function at **secret.py** below the "*Insert code here ...*" lines:

```
def main():
    cpu = _cpu()
    cpu.b32 = True
    cpu.add_zero_mem(0xf000,10240)
    cpu.load_mem("libffi-6.dll",0x400,0x4458a00,0x401000,0x4459000)
    cpu.load_mem("libffi-6.dll",0x4458e00,0x8800,0x485a000,0x9000)
    cpu.load_mem("libffi-6.dll",0x4461600,0x22200,0x4863000,0x23000)
    cpu.load_mem("libffi-6.dll",0x0,0x0,0x4886000,0x9e000)
    cpu.load_mem("libffi-6.dll",0x4483800,0x4e00,0x4924000,0x5000)
    cpu.load_mem("libffi-6.dll",0x4488600,0x6400,0x4929000,0x7000)
    cpu.load_mem("libffi-6.dll",0x448ea00,0x600,0x4930000,0x1000)
    cpu.load_mem("libffi-6.dll",0x448f000,0x200,0x4931000,0x1000)
    cpu.load_mem("libffi-6.dll",0x448f200,0xb4800,0x4932000,0xb5000)
    cpu.load_mem("libffi-6.dll",0x4543a00,0x223000,0x49e7000,0x224000)
    cpu._rsp = 0x10400
    cpu._rbp = cpu._rsp
    # Insert code here ...
    import sys
    if len(sys.argv) > 1:
        cpu._eax = int(sys.argv[1])
    func_0x44fcb68(cpu)
    # Insert code here ...
    str = cpu.get_unicode_ptr(cpu._edx)
    print(str)
```

9. Execute the new **Python** tool:

```
sudo pip install goto-statement
ln -s ../src/emu/butcher_x64.py .
python3 secret.py 10
```

> MkIzMjk1QzMwQzRBRjkxNQ==
