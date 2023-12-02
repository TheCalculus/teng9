hello world
my name is {{ name }} and i am {{ adjective }} cool

{{ CEXPR #expression1 }}
    int x = 0;
    return x;
{{ /expression1 }}

{{ JSEXPR #expression2 }}
    let x = 0;
    return x;
{{ /expression2 }}

this is a CEXPR, which says {{ #CEXPR }} sqrtf(1) {{ /CEXPR }}
this is a longer CEXPR, which says {{ CEXPR #expression1 }}
    int fib(int n) {
        if (n <= 1) return n;

        return fib(n - 1) + fib(n - 2);
    }

    return fib(10);
{{ /expression1 }}
