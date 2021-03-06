#!/bin/bash

assert(){
    expected="$1"
    input="$2"

    ./mcc "$input" > tmp.s
    cc -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected, but got $actual"
        exit 1
    fi
}
assert 0 '0;'
assert 42 '42;'
assert 2 '1+1;'
assert 47 '5+6*7;'
assert 15 '5*(9-6);'
assert 4 '(3+5)/2;'
assert 28 '(5+2)*5-4*2+1;'
assert 10 ' 12+ 3 - 5;'
assert 10 ' 12 + 3 - 5;'
assert 10 '-10+20;'
assert 0 '0==1;'
assert 1 '42==42;'
assert 1 '0!=1;'
assert 0 '42!=42;'
assert 1 '0<1;'
assert 0 '1<1;'
assert 0 '2<1;'
assert 1 '0<=1;'
assert 1 '1<=1;'
assert 0 '2<=1;'
assert 1 '1>0;'
assert 0 '1>1;'
assert 0 '1>2;'
assert 1 '1>=0;'
assert 1 '1>=1;'
assert 0 '1>=2;'
assert 12 'var1 = 10; var2 = var1 + 2; var3 = 2 * (var1+var2) + 2; return var2;'
assert 2 'return 2; return 3;'

echo OK
