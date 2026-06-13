#!/bin/bash

r64() {
echo "    @property"
echo "    def _$1(self):"
echo "        return self.$1.r64"
echo "    @_$1.setter"
echo "    def _$1(self,v):"
echo "        if type(v) == tuple:"
echo "            self.$1.r64 = v[0]"
echo "        else:"
echo "            self.$1.r64 = v"
echo ""
}

s64() {
echo "    @property"
echo "    def s_$1(self):"
echo "        return self.$1.s64"
echo "    @s_$1.setter"
echo "    def s_$1(self,v):"
echo "        if type(v) == tuple:"
echo "            self.$1.s64 = v[0]"
echo "        else:"
echo "            self.$1.s64 = v"
echo ""
}
echo "    #---------------------------------------------------------------"
echo "    # 64"
for x in rax rbx rcx rdx r8 r9 r10 r11 r12 r13 r14 r15 rsi rdi rbp rsp; do
	r64 $x
	s64 $x
done

r32() {
echo "    @property"
echo "    def _$1(self):"
echo "        return self.$2.r32.l"
echo "    @_$1.setter"
echo "    def _$1(self,v):"
echo "        if type(v) == tuple:"
echo "            self.$2.r32.l = v[0]"
echo "        else:"
echo "            self.$2.r32.l = v"
echo ""
}

s32() {
echo "    @property"
echo "    def s_$1(self):"
echo "        return self.$2.s32.l"
echo "    @s_$1.setter"
echo "    def s_$1(self,v):"
echo "        if type(v) == tuple:"
echo "            self.$2.s32.l = v[0]"
echo "        else:"
echo "            self.$2.s32.l = v"
echo ""
}

echo "    # 32"
for x in eax,rax ebx,rbx ecx,rcx edx,rdx r8d,r8 r9d,r9 r10d,r10 r11d,r11 r12d,r12 r13d,r13 r14d,r14 r15d,r15 esi,rsi edi,rdi ebp,rbp esp,rsp; do
	a=$(echo $x | awk -F "," '{print $1}')
	b=$(echo $x | awk -F "," '{print $2}')
    r32 $a $b
	s32 $a $b
done

r16() {
echo "    @property"
echo "    def _$1(self):"
echo "        return self.$2.r16.l"
echo "    @_$1.setter"
echo "    def _$1(self,v):"
echo "        if type(v) == tuple:"
echo "            self.$2.r16.l = v[0]"
echo "        else:"
echo "            self.$2.r16.l = v"
echo ""
}

s16() {
echo "    @property"
echo "    def s_$1(self):"
echo "        return self.$2.s16.l"
echo "    @s_$1.setter"
echo "    def s_$1(self,v):"
echo "        if type(v) == tuple:"
echo "            self.$2.s16.l = v[0]"
echo "        else:"
echo "            self.$2.s16.l = v"
echo ""
}

echo "    # 16"
for x in ax,rax bx,rbx cx,rcx dx,rdx r8w,r8 r9w,r9 r10w,r10 r11w,r11 r12w,r12 r13w,r13 r14w,r14 r15w,r15 si,rsi di,rdi bp,rbp sp,rsp; do
    a=$(echo $x | awk -F "," '{print $1}')
    b=$(echo $x | awk -F "," '{print $2}')
    r16 $a $b
    s16 $a $b
done

r8() {
echo "    @property"
echo "    def _$1(self):"
echo "        return self.$2.r8.l"
echo "    @_$1.setter"
echo "    def _$1(self,v):"
echo "        if type(v) == tuple:"
echo "            self.$2.r8.l = v[0]"
echo "        else:"
echo "            self.$2.r8.l = v"
echo ""
}

s8() {
echo "    @property"
echo "    def s_$1(self):"
echo "        return self.$2.s8.l"
echo "    @s_$1.setter"
echo "    def s_$1(self,v):"
echo "        if type(v) == tuple:"
echo "            self.$2.s8.l = v[0]"
echo "        else:"
echo "            self.$2.s8.l = v"
echo ""
}

for x in al,rax bl,rbx cl,rcx dl,rdx r8b,r8 r9b,r9 r10b,r10 r11b,r11 r12b,r12 r13b,r13 r14b,r14 r15b,r15 sil,rsi dil,rdi bpl,rbp spl,rsp; do
    a=$(echo $x | awk -F "," '{print $1}')
    b=$(echo $x | awk -F "," '{print $2}')
    r8 $a $b
    s8 $a $b
done

r8h() {
echo "    @property"
echo "    def _$1(self):"
echo "        return self.$2.r8.h"
echo "    @_$1.setter"
echo "    def _$1(self,v):"
echo "        if type(v) == tuple:"
echo "            self.$2.r8.h = v[0]"
echo "        else:"
echo "            self.$2.r8.h = v"
echo ""
}

s8h() {
echo "    @property"
echo "    def s_$1(self):"
echo "        return self.$2.s8.h"
echo "    @s_$1.setter"
echo "    def s_$1(self,v):"
echo "        if type(v) == tuple:"
echo "            self.$2.s8.h = v[0]"
echo "        else:"
echo "            self.$2.s8.h = v"
echo ""
}

echo "    # 8"
for x in ah,rax bh,rbx ch,rcx dh,rdx; do
    a=$(echo $x | awk -F "," '{print $1}')
    b=$(echo $x | awk -F "," '{print $2}')
    r8h $a $b
    s8h $a $b
done

r128() {
echo "    @property"
echo "    def _xmm$1(self):"
echo "        return (self.xmm$1.l,self.xmm$1.h)"
echo "    @_xmm$1.setter"
echo "    def _xmm$1(self,v):"
echo "        if type(v) == tuple:"
echo "            self.xmm$1.l = v[0]"
echo "            self.xmm$1.h = v[1]"
echo "        else:"
echo "            self.xmm$1.l = v"
echo "            self.xmm$1.h = 0"
echo ""
}

for x in 0 1 2 3 4 5 6 7; do
    r128 $x
done

echo "    #---------------------------------------------------------------"
