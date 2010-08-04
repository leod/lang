// Parameter type is a hack for now, but it works :P
extern fn i32 putchar(char);

fn bool puts_impl(string x, i32 pos) = 
	if (x[pos] = 0) { true; }
	else { putchar(x[pos]); puts_impl(x, pos + 1); };

fn bool puts(string x) = puts_impl(x, 0);

fn i32 main() = {
	puts("Hello, world!");
	0;
};
