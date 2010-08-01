fn void x() = { foo(); {}; };
fn i32 foo() = { main(5); main(1); main(2); 8; };
fn i32 main(i32) = foo();
