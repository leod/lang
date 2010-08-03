fn bool makeTrue() = true;
fn bool makeFalse() = false;
fn bool not(bool b) = if (b) false else true;
fn bool equals(i32 a, i32 b) = { 
	var bool val = if (a=b) makeTrue() else makeFalse();
	if (val) not(val) else val;
};

fn void x() = { foo(); {}; };
fn i32 foo() = { main(5); main(1); main(2); 8; };
fn i32 main(i32 x) = main(x + 1);

fn void unnamedParam(i32) = {};

fn i32 test() = {
	var i32 x = 5;
	x + 5;
	//if (x = x) 1 else 2;
};
