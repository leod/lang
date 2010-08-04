extern fn void putc(char);

fn bool puts_impl(string x, i32 pos) = 
	if (x[pos] = 0) { true; }
	else { putc(x[pos]); puts_impl(x, pos + 1); };

fn bool puts(string x) = puts_impl(x, 0);

fn i32 main() = {
	puts("Hello, world!");
	0;
};
